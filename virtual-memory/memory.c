/* Author(s): Felipe Hiroshi Baron and Guilherme Gonzaga de Andrade.
 * Implementation of the memory manager for the kernel.
 */

/* memory.c
 *
 * Note:
 * There is no separate swap area. When a data page is swapped out,
 * it is stored in the location it was loaded from in the process'
 * image. This means it's impossible to start two processes from the
 * same image without screwing up the running. It also means the
 * disk image is read once. And that we cannot use the program disk.
 */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "memory.h"
#include "thread.h"
#include "util.h"
#include "interrupt.h"
#include "tlb.h"
#include "usb/scsi.h"


/* Static global variables */


// Keep track of all pages: their vaddr, status, and other properties
static page_map_entry_t page_map[PAGEABLE_PAGES];

// address of the kernel page directory (shared by all kernel threads)
static uint32_t *kernel_pdir;

/* The kernel page tables. */
static uint32_t *kernel_ptabs[N_KERNEL_PTS];

/* Normal mode for kernel pages to be allocated in. */
static const uint32_t kernel_mode = PE_RW;  // XXX RW?

/* Set up n_pages page tables to mode starting at vaddr. */
static void setup_ptabs(uint32_t *pdir, uint32_t vaddr, int n_ptabs,
                        uint32_t mode);

/* Main API */


/* Use virtual address to get index in page directory. */
uint32_t get_dir_idx(uint32_t vaddr) {
  return (vaddr & PAGE_DIRECTORY_MASK) >> PAGE_DIRECTORY_BITS;
}


/* Use virtual address to get index in a page table. */
uint32_t get_tab_idx(uint32_t vaddr) {
  return (vaddr & PAGE_TABLE_MASK) >> PAGE_TABLE_BITS;
}


/* Return physical address of page number i.
 */
uint32_t *page_addr(int i) {
  uint32_t *addr = NULL;

  if (0 <= i && i < PAGEABLE_PAGES) {
    addr = (uint32_t *) (page_map[i].baddr << PE_BASE_ADDR_BITS);
  }

  return addr;
}


/* Set flags in a page table entry to 'mode' */
void set_ptab_entry_flags(uint32_t *pdir, uint32_t vaddr, uint32_t mode) {
  uint32_t dir_idx = get_dir_idx((uint32_t) vaddr);
  uint32_t tab_idx = get_tab_idx((uint32_t) vaddr);
  uint32_t dir_entry = pdir[dir_idx];
  uint32_t *tab;
  uint32_t entry;

  ASSERT(dir_entry & PE_P); /* dir entry present */
  tab = (uint32_t *) (dir_entry & PE_BASE_ADDR_MASK);
  /* Clear table[index] bits 11..0 */
  entry = tab[tab_idx] & PE_BASE_ADDR_MASK;

  /* Set table[index] bits 11..0 */
  entry |= mode & ~PE_BASE_ADDR_MASK;
  tab[tab_idx] = entry;

  /* Flush TLB because we just changed a page table entry in memory */
  flush_tlb_entry(vaddr);
}


/* Initialize a page table entry
 *
 * 'vaddr' is the virtual address which is mapped to the physical
 * address 'paddr'. 'mode' sets bit [11..0] in the page table entry.
 *
 * If user is nonzero, the page is mapped as accessible from a user
 * application.
 */
void init_ptab_entry(uint32_t *table, uint32_t vaddr, uint32_t paddr,
                     uint32_t mode) {
  int index = get_tab_idx(vaddr);
  table[index] = (paddr & PE_BASE_ADDR_MASK) | (mode & ~PE_BASE_ADDR_MASK);
  flush_tlb_entry(vaddr);
}


/* Insert a page table entry into the page directory.
 *
 * 'mode' sets bit [11..0] in the page table entry.
 */
void insert_ptab_dir(uint32_t *dir, uint32_t *tab, uint32_t vaddr,
                     uint32_t mode) {

  uint32_t access = mode & MODE_MASK;
  int idx = get_dir_idx(vaddr);

  dir[idx] = ((uint32_t)tab & PE_BASE_ADDR_MASK) | access;
}


/* Allocate a page. Return page index in the page_map directory.
 *
 * Marks page as pinned if pinned == TRUE.
 * Swap out a page if no space is available.
 */
int page_alloc(int pinned) {
  int page_idx = page_replacement_policy();

  if (page_idx != -1) {
    page_map_entry_t *entry = &page_map[page_idx];

    if (entry->p) {
      if (entry->d || entry->pwt) {
        page_swap_out(page_idx);

        /* Flush TLB because we just changed a page table entry in memory */
        flush_tlb_entry(entry->vaddr);
      }

      /* Zero-out the page */
      bzero((char *) page_addr(page_idx), PAGE_SIZE);
    }

    entry->pinned = pinned;
  }

  return page_idx;
}


/* Set up kernel memory for kernel threads to run.
 *
 * This method is only called once by _start() in kernel.c, and is only
 * supposed to set up the page directory and page tables for the kernel.
 */
void init_memory(void) {
  /* Set up kernel page directory */
  kernel_pdir = (uint32_t *) &kernel_ptabs[0];

  /* Map kernel page tables */
  setup_ptabs(kernel_pdir, MEM_START, N_KERNEL_PTS, kernel_mode);

  /* Give userland permission to access screen memory */
  set_ptab_entry_flags(kernel_pdir, SCREEN_ADDR, kernel_mode | PE_US);
}


/* Set up a page directory and page table for a new
 * user process or thread.
 */
void setup_page_table(pcb_t *p) {
  uint32_t code_mode  = PE_US;
  uint32_t stack_mode = PE_US | PE_RW;

  if (p->is_thread) {
    p->page_directory = kernel_pdir;
    // TODO: setup p->kernel_stack and p->base_kernel_stack;
  }
  else {
    /* Map code and data segments together */
    setup_ptabs(p->page_directory, p->start_pc, 1/*XXX*/, code_mode);

    /* Map kernel page tables */
    setup_ptabs(p->page_directory, MEM_START, N_KERNEL_PTS, kernel_mode);

    /* Give userland permission to access screen memory */
    set_ptab_entry_flags(p->page_directory, SCREEN_ADDR, kernel_mode | PE_US);

    /* Set up stack pages */
    for (int i = 0; i < N_PROCESS_STACK_PAGES; i++) {
      set_ptab_entry_flags(p->page_directory, p->user_stack, stack_mode);
    }
  }
}


/* Swap into a free page upon a page fault.
 *
 * This method is called from interrupt.c: exception_14().
 * Should handle demand paging.
 */
void page_fault_handler(void) {
  static lock_t lock;
  static _Bool lock_initialized = 0;

  pcb_t *task = current_running;
  int dir_idx = get_dir_idx(task->fault_addr);
  int tab_idx = get_tab_idx(task->fault_addr);
  uint32_t *ptab = (uint32_t *) task->page_directory[dir_idx];
  uint32_t paddr = ptab[tab_idx];
  int page_idx;

  if (!lock_initialized) {
    lock_initialized = 1;
    lock_init(&lock);
  }

  lock_acquire(&lock);

  page_alloc(page_idx);
  page_swap_in(page_idx);

  lock_release(&lock);
}


/* Get the sector number on disk of a process image.
 * Used for page swapping.
 */
int get_disk_sector(page_map_entry_t *page) {
  return page->swap_loc +
    ((page->vaddr - PROCESS_START) / PAGE_SIZE) * SECTORS_PER_PAGE;
}


/* Swap i-th page in from disk (i.e. the image file)
 */
void page_swap_in(int i) {
  void *data = page_addr(i);

  scsi_read(page_map[i].swap_loc, SECTORS_PER_PAGE, data);
}


/* Swap i-th page out to disk.
 *
 * Write the page back to the process image.
 * There is no separate swap space on the USB.
 */
void page_swap_out(int i) {
  void *data = page_addr(i);

  scsi_write(page_map[i].swap_loc, SECTORS_PER_PAGE, data);
}


/* Decide which page to replace, return the page number.
 *
 * Return -1 if no pages can be replaced (i.e. all pinned).
 */
int page_replacement_policy(void) {
  int min_idx = -1;
  uint32_t ad_bits_min = 4;

  for (int i = 0; i < PAGEABLE_PAGES; i++) {
    page_map_entry_t *entry = &page_map[i];

    if (!entry->pinned) {
      if (!entry->p) {  /* Available slots have highest priority */
        return i;
      }

      /* Accessed and dirty bits make up the page's fitness;
       * a lower score is preferable to replacement.
       * Write-through pages are considered always dirty.
       */
      uint32_t ad_bits = (entry->a << 1) | (entry->pwt || entry->d);

      if (ad_bits < ad_bits_min) {
        ad_bits_min = ad_bits;
        min_idx = i;
      }

      // TODO: reset accessed bits of all pages indexed
    }
  }

  return min_idx;
}


static void setup_ptabs(uint32_t *pdir, uint32_t vaddr, int n_ptabs,
                        uint32_t mode) {
  uint32_t paddr = (mode & PE_US) ? 0: vaddr;

  /* Set up page directory */
  for (int i = 0; i < n_ptabs; i++) {
    uint32_t ptab_address = vaddr + (i << PAGE_DIRECTORY_BITS);

    /* Set up page tables */
    for (int j = 0; j < PAGE_N_ENTRIES; j++) {
      uint32_t page_address = ptab_address + (j << PAGE_TABLE_BITS);
      init_ptab_entry(pdir[i], page_address, paddr, mode);
    }

    insert_ptab_dir(pdir, pdir[i], ptab_address, mode);
  }
}
