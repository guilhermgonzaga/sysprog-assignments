/* Author(s): Felipe Hiroshi Baron e Guilherme Gonzaga
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

#define SECTOR_SIZE 512             /* floppy sector size in bytes */
#define BOOTLOADER_SIG_OFFSET 0x1fe /* offset for boot loader signature */
#define BOOTLOADER_ADRESS 0X7c00    /* boot loader address in memory */
#define ELF32_HEADER_SIZE 52        /* Elf32 header size in bytes */
// more defines...

/* Reads in an executable file in ELF format*/
Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr **ehdr){
  Elf32_Phdr *phdr = malloc(sizeof(Elf32_Phdr));
  size_t items_rw;

  fseek(*execfile, 0, SEEK_SET);
  items_rw = fread(*ehdr, ELF32_HEADER_SIZE, 1, *execfile);
  assert(items_rw == 1);

  fseek(*execfile, (*ehdr)->e_phoff, SEEK_SET);
  items_rw = fread(phdr, (*ehdr)->e_phentsize, (*ehdr)->e_phnum, *execfile);
  assert(items_rw == (*ehdr)->e_phnum);

  return phdr;
}

/* Writes the bootblock to the image file */
void write_bootblock(FILE **imagefile, FILE *bootfile, Elf32_Ehdr *boot_header, Elf32_Phdr *boot_phdr){
  unsigned char *buffer = malloc(boot_phdr->p_filesz);
  const size_t padding_length = SECTOR_SIZE - (boot_phdr->p_filesz % SECTOR_SIZE);
  const char zero = 0;
  size_t items_rw;

  fseek(bootfile, boot_phdr->p_offset, SEEK_SET);
  items_rw = fread(buffer, boot_phdr->p_filesz, 1, bootfile);
  assert(items_rw == 1);

  fseek(*imagefile, 0, SEEK_SET);
  items_rw = fwrite(buffer, boot_phdr->p_filesz, 1, *imagefile);
  assert(items_rw == 1);

  buffer[0] = 0;
  items_rw = fwrite(buffer, 1, padding_length, *imagefile);
  assert(items_rw == padding_length);

  /* mark the bootloader sector as bootable */
  fseek(*imagefile, BOOTLOADER_SIG_OFFSET, SEEK_SET);
  buffer[0] = 0x55;
  buffer[1] = 0xAA;
  items_rw = fwrite(buffer, 2, 1, *imagefile);
  assert(items_rw == 1);

  free(buffer);
}

/* Writes the kernel to the image file */
void write_kernel(FILE **imagefile, FILE *kernelfile, Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){


}

/* Counts the number of sectors in the kernel */
int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
  float sectors;
  sectors = (float)kernel_phdr->p_filesz/SECTOR_SIZE;
  if(sectors - (int)sectors/1 != 0){
    sectors = (int)sectors+1;
  }
  return sectors;
}

/* Records the number of sectors in the kernel */
void record_kernel_sectors(FILE **imagefile, Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr, int num_sec){
  // (int)kernel_header->e_shnum
}


/* Prints segment information for --extended option */
void extended_opt(Elf32_Phdr *bph, int k_phnum, Elf32_Phdr *kph, int num_sec){

  /* print number of disk sectors used by the image */


  /* bootblock segment info */


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
  assert(imagefile != NULL);

  /* read executable bootblock file */
  bootfile = fopen("bootblock", "rb");
  assert(bootfile != NULL);
  boot_program_header = read_exec_file(&bootfile, "bootblock", &boot_header);

  /* write bootblock */
  write_bootblock(&imagefile, bootfile, boot_header, boot_program_header);

  /* read executable kernel file */
  kernelfile = fopen("kernel", "rb");
  assert(kernelfile != NULL);
  kernel_program_header = read_exec_file(&kernelfile, "kernel", &kernel_header);

  /* write kernel segments to image */

  /* tell the bootloader how many sectors to read to load the kernel */
  count_kernel_sectors(kernel_header, boot_program_header);


  /* check for  --extended option */
  if(!strncmp(argv[1],"--extended",11)){
	/* print info */
  }

  return 0;
} // ends main()
