/* Author(s): Felipe Hiroshi Baron and Guilherme Gonzaga de Andrade
 * Creates operating system image suitable for placement on a boot disk
*/
/* Implementation complete. */
#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define IMAGE_FILE "./image"
#define ARGS "[--extended] [--vm] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512             /* floppy sector size in bytes */
#define BOOTLOADER_SIG_OFFSET 0x1fe /* offset for boot loader signature */
#define BOOTLOADER_ADRESS 0X7c00    /* boot loader address in memory */
#define ELF32_HEADER_SIZE 52        /* Elf32 header size in bytes */


/* Reads in an executable file in ELF format*/
Elf32_Phdr *read_exec_file(FILE **execfile, const char *filename, Elf32_Ehdr **ehdr){
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
  uint8_t *buffer = malloc(boot_phdr->p_filesz);
  const size_t padding_length = SECTOR_SIZE - (boot_phdr->p_filesz % SECTOR_SIZE);
  size_t items_rw;

  fseek(bootfile, boot_phdr->p_offset, SEEK_SET);
  items_rw = fread(buffer, boot_phdr->p_filesz, 1, bootfile);
  assert(items_rw == 1);

  fseek(*imagefile, 0, SEEK_SET);
  items_rw = fwrite(buffer, boot_phdr->p_filesz, 1, *imagefile);
  assert(items_rw == 1);

  uint8_t *buffer_padding = calloc(padding_length, 1);
  items_rw = fwrite(buffer_padding, 1, padding_length, *imagefile);
  assert(items_rw == padding_length);

  /* mark the bootloader sector as bootable */
  fseek(*imagefile, BOOTLOADER_SIG_OFFSET, SEEK_SET);
  buffer[0] = 0x55;
  buffer[1] = 0xAA;
  items_rw = fwrite(buffer, 2, 1, *imagefile);
  assert(items_rw == 1);

  free(buffer);
  free(buffer_padding);
}


/* Writes the kernel to the image file */
void write_kernel(FILE **imagefile, FILE *kernelfile, Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
  uint8_t *buffer = malloc(kernel_phdr->p_filesz);
  const size_t padding_length = SECTOR_SIZE - (kernel_phdr->p_filesz % SECTOR_SIZE);
  size_t items_rw;

  fseek(kernelfile, kernel_phdr->p_offset, SEEK_SET);
  items_rw = fread(buffer, kernel_phdr->p_filesz, 1, kernelfile);
  assert(items_rw == 1);

  fseek(*imagefile, SECTOR_SIZE, SEEK_SET);
  items_rw = fwrite(buffer, kernel_phdr->p_filesz, 1, *imagefile);
  assert(items_rw == 1);

  uint8_t *buffer_padding = calloc(padding_length, 1);
  items_rw = fwrite(buffer_padding, 1, padding_length, *imagefile);
  assert(items_rw == padding_length);

  free(buffer);
  free(buffer_padding);
}


/* Counts the number of sectors in the kernel */
int32_t count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
  float sectors = (float)kernel_phdr->p_filesz / SECTOR_SIZE;
  if(kernel_phdr->p_filesz % SECTOR_SIZE != 0){
    sectors = (int32_t)sectors + 1;
  }
  return sectors;
}


/* Records the number of sectors in the kernel */
void record_kernel_sectors(FILE **imagefile, Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr, int32_t num_sec){
  fseek(*imagefile, 2, SEEK_SET);
  fwrite(&num_sec, sizeof(int32_t), 1, *imagefile);
}


/* Prints segment information for --extended option */
void extended_opt(Elf32_Phdr *bph, int k_phnum, Elf32_Phdr *kph, int32_t num_sec){
  int padding_up_boot = (bph->p_filesz/SECTOR_SIZE)+ SECTOR_SIZE;
  int padding_up_kernel;

  /* print number of disk sectors used by the image */
  //printf("%x", k_phnum);

  /* bootblock segment info */
  printf("0x%04x: ./bootblock\n", bph->p_paddr);
  printf("\tsegment %d\n", bph->p_paddr);
  printf("\t\toffset 0x%04x\t\tvaddr 0x%04x\n", bph->p_offset, bph->p_vaddr);
  printf("\t\tfilesz 0x%04x\t\tmemsz 0x%04x\n", bph->p_filesz, bph->p_memsz);
  printf("\t\twriting 0x%04x bytes\n", bph->p_memsz);
  printf("\t\tpadding up to 0x%04x bytes\n", padding_up_boot);

  /* print kernel segment info */
  padding_up_kernel =(SECTOR_SIZE) * num_sec + 512;
  printf("0x%04x: ./kernel\n", kph->p_paddr);
  printf("\tsegment %d\n", kph->p_paddr);
  printf("\t\toffset 0x%04x\t\tvaddr 0x%04x\n", kph->p_offset, kph->p_vaddr);
  printf("\t\tfilesz 0x%04x\t\tmemsz 0x%04x\n", kph->p_filesz, kph->p_memsz);
  printf("\t\twriting 0x%04x bytes\n", kph->p_memsz);
  printf("\t\tpadding up to 0x%04x bytes\n", padding_up_kernel);

  /* print kernel size in sectors */
  printf("os_size: %d sectors\n", num_sec);
}


/* MAIN */
int main(int argc, char **argv){
  if (argc < 3 || argc > 5) {
    printf("Usage: %s %s", argv[0], ARGS);
    return EXIT_FAILURE;
  }

  const char *bootblock_filename = argv[argc-2];
  const char *kernel_filename = argv[argc-1];

  FILE *kernelfile, *bootfile,*imagefile;  //file pointers for bootblock,kernel and image
  Elf32_Ehdr *boot_header = malloc(sizeof(Elf32_Ehdr)); //bootblock ELF header
  Elf32_Ehdr *kernel_header = malloc(sizeof(Elf32_Ehdr)); //kernel ELF header

  Elf32_Phdr *boot_program_header;  //bootblock ELF program header
  Elf32_Phdr *kernel_program_header;  //kernel ELF program header

  int32_t num_sec;

  /* build image file */
  imagefile = fopen(IMAGE_FILE, "wb");
  assert(imagefile != NULL);

  /* read executable bootblock file */
  bootfile = fopen(bootblock_filename, "rb");
  assert(bootfile != NULL);
  boot_program_header = read_exec_file(&bootfile, bootblock_filename, &boot_header);

  /* write bootblock to image*/
  write_bootblock(&imagefile, bootfile, boot_header, boot_program_header);

  /* read executable kernel file */
  kernelfile = fopen(kernel_filename, "rb");
  assert(kernelfile != NULL);
  kernel_program_header = read_exec_file(&kernelfile, kernel_filename, &kernel_header);

  /* write kernel segments to image */
  write_kernel(&imagefile, kernelfile, kernel_header, kernel_program_header);

  /* tell the bootloader how many sectors to read to load the kernel */
  num_sec = count_kernel_sectors(kernel_header, kernel_program_header);
  record_kernel_sectors(&imagefile, kernel_header, kernel_program_header, num_sec);

  /* check for --extended option */
  if(!strncmp(argv[1],"--extended",11)){
    extended_opt(boot_program_header, kernel_header->e_phnum, kernel_program_header, num_sec);
  }

  fclose(imagefile);
  fclose(bootfile);
  fclose(kernelfile);

  return EXIT_SUCCESS;
} // ends main()
