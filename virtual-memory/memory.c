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
 *
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

//other global variables...

/* Main API */

/* Use virtual address to get index in page directory. */
uint32_t get_dir_idx(uint32_t vaddr) {
  return (vaddr & PAGE_DIRECTORY_MASK) >> PAGE_DIRECTORY_BITS;
}

/* Use virtual address to get index in a page table. */
uint32_t get_tab_idx(uint32_t vaddr) {
  return (vaddr & PAGE_TABLE_MASK) >> PAGE_TABLE_BITS;
}

// TODO: Returns physical address of page number i
uint32_t* page_addr(int i) {
  uint32_t *addr = NULL;

  if (i <= PAGEABLE_PAGES) {
    addr = (page_map[i].ppage_num << PE_BASE_ADDR_BITS);
  }

  return addr;
}

/* Set flags in a page table entry to 'mode' */
void set_ptab_entry_flags(uint32_t * pdir, uint32_t vaddr, uint32_t mode) {
  uint32_t dir_idx = get_dir_idx((uint32_t) vaddr);
  uint32_t tab_idx = get_tab_idx((uint32_t) vaddr);
  uint32_t dir_entry;
  uint32_t *tab;
  uint32_t entry;

  dir_entry = pdir[dir_idx];
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
 * address 'paddr'. 'mode' sets bit [12..0] in the page table entry.
 *
 * If user is nonzero, the page is mapped as accessible from a user
 * application.
 */
void init_ptab_entry(uint32_t * table, uint32_t vaddr,
         uint32_t paddr, uint32_t mode) {
  int index = get_tab_idx(vaddr);
  table[index] =
    (paddr & PE_BASE_ADDR_MASK) | (mode & ~PE_BASE_ADDR_MASK);
  flush_tlb_entry(vaddr);
}

/* Insert a page table entry into the page directory.
 *
 * 'mode' sets bit [12..0] in the page table entry.
 */
void insert_ptab_dir(uint32_t * dir, uint32_t *tab, uint32_t vaddr,
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
  // FIXME: Assume the number of pages is less than PAGEABLE_PAGES
  int page_idx = page_replacement_policy();

  if (page_map[page_idx].p) {
    page_swap_out(page_map[page_idx].ppage_num);
  }

  page_map[page_idx].p = 1;

  if (pinned == TRUE) {
    page_map[page_idx].pinned = TRUE;
  }

  return page_idx;
}

/* TODO: Set up kernel memory for kernel threads to run.
 *
 * This method is only called once by _start() in kernel.c, and is only
 * supposed to set up the page directory and page tables for the kernel.
 */
void init_memory(void) {
  // for (int i = 0; i < PAGEABLE_PAGES; i++) {
  //   page_map[i] = (page_map_entry_t) {
  //     .swap_loc = 0,
  //     .vaddr = 0,
  //     .ppage_num = 0,
  //     .bits = 0,
  //   };
  // }

  // TODO alloc page for directory page
  kernel_pdir = KERNEL_LOCATION;
  page_map[page_alloc(TRUE)];

  for (int i = 0; i < N_KERNEL_PTS; i++) {
    page_alloc(TRUE);

    for (int j = 0; j < (MAX_PHYSICAL_MEMORY >> PE_BASE_ADDR_BITS); j++) {
      // TODO: alloc kernel pages
    }
  }
}


/* TODO: Set up a page directory and page table for a new
 * user process or thread. */
void setup_page_table(pcb_t * p) {

}

/* TODO: Swap into a free page upon a page fault.
 * This method is called from interrupt.c: exception_14().
 * Should handle demand paging.
 */
void page_fault_handler(void) {

}

/* Get the sector number on disk of a process image.
 * Used for page swapping. */
int get_disk_sector(page_map_entry_t * page) {
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
  uint8_t bits, bits_nru = 4;

  for (int i = 0; i < PAGEABLE_PAGES; i++) {
    bits_nru = (page_map[min_idx].a << 1) | page_map[min_idx].d;
    bits = (page_map[i].a << 1) | page_map[i].d;

    if (!page_map[i].p) {
      return i;
    }

    if (bits < bits_nru) {
      min_idx = i;
    }
  }

  return min_idx;
}
