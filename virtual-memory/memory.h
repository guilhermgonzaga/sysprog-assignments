/* Author(s): Felipe Hiroshi Baron and Guilherme Gonzaga de Andrade.
 * Defines the memory manager for the kernel.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"


enum {
  /* physical page facts */
  PAGE_SIZE = 4096,
  PAGE_N_ENTRIES = (PAGE_SIZE / sizeof(uint32_t)),
  SECTORS_PER_PAGE = (PAGE_SIZE / SECTOR_SIZE),

  PTABLE_SPAN = (PAGE_SIZE * PAGE_N_ENTRIES),

  /* page directory/table entry bits (PMSA p.235 and p.240) */
  PE_P = 1 << 0,    /* Present */
  PE_RW = 1 << 1,   /* Read/write */
  PE_US = 1 << 2,   /* User/supervisor */
  PE_PWT = 1 << 3,  /* Page write-through */
  PE_PCD = 1 << 4,  /* Page cache disable */
  PE_A = 1 << 5,    /* Accessed */
  PE_D = 1 << 6,    /* Dirty */
  PE_PIN = 1 << 9,  /* Pinned in memory (non-standard) */
  PE_BASE_ADDR_BITS = 12,  /* Position of base address */
  PE_BASE_ADDR_MASK = 0xfffff000,  /* Extracts the base address */

  /* Constants to simulate a very small physical memory. */
  MEM_START = 0x100000, /* 1MB */
  PAGEABLE_PAGES = 33,
  MAX_PHYSICAL_MEMORY = (MEM_START + PAGEABLE_PAGES * PAGE_SIZE),

  /* number of kernel page tables */
  N_KERNEL_PTS = 1,
  N_PROCESS_STACK_PAGES = 2,

  PAGE_DIRECTORY_BITS = 22,         /* position of page dir index */
  PAGE_TABLE_BITS = 12,             /* position of page table index */
  PAGE_DIRECTORY_MASK = 0xffc00000, /* page directory mask */
  PAGE_TABLE_MASK = 0x003ff000,     /* page table mask */
  PAGE_MASK = 0x00000fff,           /* page offset mask */
  /* used to extract the 10 lsb of a page directory entry */
  MODE_MASK = 0x000003ff,

  PAGE_TABLE_SIZE = (1024 * 4096 - 1) /* size of a page table in bytes */
};


/* Structure of an entry in the page map */
typedef struct {
  uint32_t swap_loc; /* Swap space base address */
  uint32_t vaddr;    /* Virtual address */

  /* Page directory/table entry bits */
  /* Total size must be 32 bits */
  union {
    uint32_t entry;
    struct {
      uint32_t p: 1;      /* Present */
      uint32_t rw: 1;     /* Read/write */
      uint32_t us: 1;     /* User/supervisor */
      uint32_t pwt: 1;    /* Page write-through */
      uint32_t pcd: 1;    /* Page cache disable */
      uint32_t a: 1;      /* Accessed */
      uint32_t d: 1;      /* Dirty */
      uint32_t : 2;       /* Padding */
      uint32_t pinned: 1; /* Page pinned in memory (non-standard) */
      uint32_t : 2;       /* Padding */
      uint32_t baddr: 20; /* Page base address */
    };
  };
} page_map_entry_t;


/* Prototypes */

/* Use virtual address to get index in page directory. */
uint32_t get_dir_idx(uint32_t vaddr);

/* Use virtual address to get index in a page table. */
uint32_t get_tab_idx(uint32_t vaddr);

/* Return the physical address of the i-th page */
uint32_t *page_addr(int i);

/* Allocate a page. If necessary, swap a page out.
 * On success, return the index of the page in the page map. On
 * failure, abort. BUG: pages are not made free when a process
 * exits.
 */
int page_alloc(int pinned);

/* Set up kernel memory, called from kernel.c: _start() */
void init_memory(void);

/* Set up a page directory and page table for the given process. Fill in
 * any necessary information in the pcb.
 */
void setup_page_table(pcb_t *p);

/* Swap into a free page upon a page fault.
 * This method is called from interrupt.c: exception_14().
 * Should handle demand paging.
 */
void page_fault_handler(void);

/* Swap the i-th page in from disk (i.e. the image file) */
void page_swap_in(int i);

/* Swap the i-th page out */
void page_swap_out(int i);

/* Decide which page to replace, return the page number  */
int page_replacement_policy(void);


#endif /* !MEMORY_H */
