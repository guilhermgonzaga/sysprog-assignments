/* Author(s): <Your name(s) here>
 * Creates operating system image suitable for placement on a boot disk
*/
/* TODO: Comment on the status of your submission. Largely unimplemented */
#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMAGE_FILE "./image"
#define ARGS "[--extended] [--vm] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512       /* floppy sector size in bytes */
#define BOOTLOADER_SIG_OFFSET 0x1fe /* offset for boot loader signature */
// more defines...

typedef long size_t;

/* Reads in an executable file in ELF format*/
Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr **ehdr){
  // const int ofssets [] = {0x00, 0x10, 0x12, 0x14, 0x18, 
  //                         0x1C, 0x20, 0x24, 0x28, 0x2A,
  //                         0x2C, 0x2E, 0x30, 0x32}; 
  Elf32_Phdr *phdr = malloc(sizeof(Elf32_Phdr));

  fseek(*execfile, 0, SEEK_SET);
  size_t bytes_read = fread((void*)*ehdr, 52, 1, *execfile);

  assert(bytes_read == 52);
  
  fseek(*execfile, (*ehdr)->e_phoff, SEEK_SET);
  bytes_read = fread((void*)phdr, (*ehdr)->e_phnum, 1, *execfile);
  
  assert(bytes_read == (*ehdr)->e_phnum);

  return phdr;
}

/* Writes the bootblock to the image file */
void write_bootblock(FILE **imagefile,FILE *bootfile,Elf32_Ehdr *boot_header, Elf32_Phdr *boot_phdr){
 

}

/* Writes the kernel to the image file */
void write_kernel(FILE **imagefile,FILE *kernelfile,Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){

 
}

/* Counts the number of sectors in the kernel */
int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
   
    return 0;
}

/* Records the number of sectors in the kernel */
void record_kernel_sectors(FILE **imagefile,Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr, int num_sec){
    
}


/* Prints segment information for --extended option */
void extended_opt(Elf32_Phdr *bph, int k_phnum, Elf32_Phdr *kph, int num_sec){

  /* print number of disk sectors used by the image */

  
  /*bootblock segment info */
 

  /* print kernel segment info */
  

  /* print kernel size in sectors */
}
// more helper functions...

/* MAIN */
// ignore the --vm argument when implementing (project 1)
int main(int argc, char **argv){
  FILE *kernelfile, *bootfile,*imagefile;  //file pointers for bootblock,kernel and image
  Elf32_Ehdr *boot_header = malloc(sizeof(Elf32_Ehdr));//bootblock ELF header
  Elf32_Ehdr *kernel_header = malloc(sizeof(Elf32_Ehdr));//kernel ELF header

  Elf32_Phdr *boot_program_header; //bootblock ELF program header
  Elf32_Phdr *kernel_program_header; //kernel ELF program header
  /* build image file */
  imagefile = fopen(IMAGE_FILE, "wb");
  kernelfile = fopen("kernel", "rb");
  bootfile = fopen("bootblock", "rb");
  
  assert(imagefile != NULL);
  
  /* read executable bootblock file */  
  boot_program_header = read_exec_file(&bootfile, "bootblock", &boot_program_header);
  
  /* write bootblock */  

  /* read executable kernel file */
  kernel_program_header = read_exec_file(&kernelfile, "kernel", &kernel_header);
 
  /* write kernel segments to image */

  /* tell the bootloader how many sectors to read to load the kernel */

  /* check for  --extended option */
  if(!strncmp(argv[1],"--extended",11)){
	/* print info */
  }
  
  return 0;
} // ends main()



