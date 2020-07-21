/* Author(s): Felipe Hiroshi Baron and Guilherme Gonzaga de Andrade
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

// allocate the kernel page tables
static uint32_t *kernel_ptabs[N_KERNEL_PTS];


// XXX static void setup_kernel_tables(void);

/* Main API */


/* Use virtual address to get index in page directory. */
uint32_t get_dir_idx(uint32_t vaddr) {
  return (vaddr & PAGE_DIRECTORY_MASK) >> PAGE_DIRECTORY_BITS;
}


/* Use virtual address to get index in a page table. */
uint32_t get_tab_idx(uint32_t vaddr) {
  return (vaddr & PAGE_TABLE_MASK) >> PAGE_TABLE_BITS;
}


/* Return physical address of page number i. */
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
  /* clear table[index] bits 11..0 */
  entry = tab[tab_idx] & PE_BASE_ADDR_MASK;

  /* set table[index] bits 11..0 */
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


/* TODO: Allocate a page. Return page index in the page_map directory.
 *
 * Marks page as pinned if pinned == TRUE.
 * Swap out a page if no space is available.
 */
int page_alloc(int pinned) {
  int page_idx = page_replacement_policy();

  if (page_map[page_idx].p) {
    page_swap_out(page_map[page_idx].baddr);
  }

  page_map[page_idx].pinned = pinned;

  // TODO: zero-out the page?

  return page_idx;
}


/* Set up kernel memory for kernel threads to run.
 *
 * This method is only called once by _start() in kernel.c, and is only
 * supposed to set up the page directory and page tables for the kernel.
 */
void init_memory(void) {
  const uint32_t entry_mode = /*... |*/ PE_RW;

  kernel_pdir = (uint32_t *) get_dir_idx(MEM_START);

  /* Set up kernel page directory */
  for (int i = 0; i < N_KERNEL_PTS; i++) {
    uint32_t ptab_address = MEM_START + i * PAGE_N_ENTRIES;

    /* Set up kernel page tables */
    for (int j = 0; j < PAGE_N_ENTRIES; j++) {
      uint32_t page_address = ptab_address + j * PAGE_SIZE;
      init_ptab_entry(kernel_ptabs[i], page_address, page_address, entry_mode);
    }

    insert_ptab_dir(kernel_pdir, kernel_ptabs[i], ptab_address, entry_mode);
  }

  /* Give userland permission to access screen memory */
  ASSERT(get_dir_idx(SCREEN_ADDR) < ((N_KERNEL_PTS - 1) / PAGE_N_ENTRIES));
  int screen_tab = get_tab_idx(SCREEN_ADDR);
  init_ptab_entry(kernel_ptabs[screen_tab], SCREEN_ADDR, SCREEN_ADDR,
                  entry_mode | PE_US);
}


/* Set up a page directory and page table for a new
 * user process or thread.
 */
void setup_page_table(pcb_t *p) {
  const uint32_t entry_mode = /*... |*/ PE_US;

  // TODO: if (p->is_thread) {} else {}  (kernel_stack, base_kernel_stack)


  /* Map code and data segments together */

  /* Set up process page directory */
  for (int i = 0; i < 1/*BUG*/; i++) {
    uint32_t ptab_address = p->start_pc + i * PAGE_N_ENTRIES;

    /* Set up process page tables */
    for (int j = 0; j < PAGE_N_ENTRIES; j++) {
      uint32_t page_address = ptab_address + j * PAGE_SIZE;
      init_ptab_entry(p->page_directory[i], page_address, 0, entry_mode);
    }

    insert_ptab_dir(p->page_directory, p->page_directory[i], ptab_address, entry_mode);
  }

  /* Set up stack pages */
  for (int i = 0; i < N_PROCESS_STACK_PAGES; i++) {
    uint32_t page_address = p->user_stack + i * PAGE_SIZE;
    init_ptab_entry(p->page_directory[i], page_address, 0, entry_mode | PE_RW);
  }

  // XXX: map kernel and video memory?
  /* Give userland permission to access screen memory */
  int screen_tab = get_tab_idx(SCREEN_ADDR);
  init_ptab_entry(kernel_ptabs[screen_tab], SCREEN_ADDR, SCREEN_ADDR,
                  entry_mode | PE_RW);
}


/* TODO: Swap into a free page upon a page fault.
 * This method is called from interrupt.c: exception_14().
 * Should handle demand paging.
 */
void page_fault_handler(void) {

}


/* Get the sector number on disk of a process image.
 * Used for page swapping. */
int get_disk_sector(page_map_entry_t *page) {
  return page->swap_loc +
    ((page->vaddr - PROCESS_START) / PAGE_SIZE) * SECTORS_PER_PAGE;
}


// TODO: Swap i-th page in from disk (i.e. the image file)
void page_swap_in(int i) {

}


/* TODO: Swap i-th page out to disk.
 *
 * Write the page back to the process image.
 * There is no separate swap space on the USB.
 */
void page_swap_out(int i) {

}


/* Decide which page to replace, return the page number. */
int page_replacement_policy(void) {
  int min_idx = 0;

  for (int i = 0; i < PAGEABLE_PAGES; i++) {
    if (!page_map[i].pinned) {
      if (!page_map[i].p) {  /* Available slots have highest priority */
        return i;
      }

      uint32_t ad_bits_min = (page_map[min_idx].a << 1) | page_map[min_idx].d;
      uint32_t ad_bits = (page_map[i].a << 1) | page_map[i].d;

      if (ad_bits < ad_bits_min) {
        min_idx = i;
      }
    }
  }

  return min_idx;
}
