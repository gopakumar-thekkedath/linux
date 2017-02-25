/*
 * Copyright (C) 2017  
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bootparam.h"
#include "elf.h"

#define KRN_HDR 1024
static enum elf_type 
parse_elfhdrs(struct elf_hdr_common *ehdr_comm)
{
  if (*((uint32_t *)(ehdr_comm->magic)) != ('\177' | 'E' << 8 |
        'L' << 16 | 'F' << 24)) { 
    return ELF_UNSUPPORTED;
  }

  return ehdr_comm->class;
}

enum elf_type
decode_bzImage(void *img_addr)
{
  struct boot_params boot;
#ifdef LOADER_COMPLETE
  struct boot_params *zero_page = 0x0;
  uint8_t guest_e820_cnt;
  char *cmdline = "earlyprintk=serial,ttyS0,115200 console=ttyS0,9600n8 maxcpus=1";
  void *cmdline_ptr = (void *)sizeof(boot); 
/* Modern bzImages get loaded at 1M. */
  void *p = 0x100000;
#endif
  int i;
  char *hdr = "HdrS";
  char hdr2[5];

  /*
   * As per boot.txt
   *In 32-bit boot protocol, the first step in loading a Linux kernel
   *should be to setup the boot parameters (struct boot_params,
   *traditionally known as "zero page"). The memory for struct boot_params
   *should be allocated and initialized to all zero. Then the setup header
   *from offset 0x01f1 of kernel image on should be loaded into struct
   *boot_params and examined. The end of setup header can be calculated as
   *follow:

   */
  memcpy(&boot, img_addr, sizeof(boot));
  memcpy(hdr2, &boot.hdr.header, 4);
  hdr2[4] = '\0';
    
  /* Inside the setup_hdr, we expect the magic "HdrS" */
  if (memcmp(hdr2, hdr,4) != 0) {
	printf("\n Did not find the header Hdrs");
    return ELF_UNSUPPORTED;
  }
  printf("\n Header, boot flag:%x", boot.hdr.boot_flag);
  printf("\n Header, boot protocol version:%x", boot.hdr.version);
  /* code32_start is jumped to immediately after transitioning to prot mode  */
  printf("\n Header, code32_start:%08x", boot.hdr.code32_start);
  /* relocatable kernel if non-zero value */
  printf("\n Header, relocatable kernel:%08x", boot.hdr.relocatable_kernel);
#ifdef LOADER_COMPLETE
  /* copy image into memory */
  memcpy(p, (img_addr+(boot.hdr.setup_sects+1) * 512),
		 ((guest_img_end - guest_img_start)-(boot.hdr.setup_sects+1) * 512));
  memset(zero_page, 0, sizeof(boot));
 
	if ((guest_e820_cnt = generate_guest_e820_map()) == 0 ) {
		printf("\n%s", "Unable to generate Guest E820 map !");
		cpu_hlt();
	}
 
  zero_page->e820_entries = guest_e820_cnt;
 
  for (i=0; i<guest_e820_cnt; i++)
    zero_page->e820_map[i] = guest_e820_mem[i];
  
  memcpy(cmdline_ptr,cmdline,strlen(cmdline));
  zero_page->hdr.cmd_line_ptr = cmdline_ptr;
  printf("\n%s","cmdline parameter: ");
  printf("%s",(char *)zero_page->hdr.cmd_line_ptr);

  zero_page->hdr.kernel_alignment = 0x1000000;

  boot.hdr.ramdisk_image = ramdisk_start;
  boot.hdr.ramdisk_size = ramdisk_size;
  boot.hdr.type_of_loader = 0xff;
  zero_page->hdr.ramdisk_image = ramdisk_start;
  zero_page->hdr.ramdisk_size = ramdisk_size;
  zero_page->hdr.type_of_loader = 0xff;
  printf("\n%s", "Supported image type bzImage !");
  *((uint32_t *)guest_start) = boot.hdr.code32_start;
#endif

  return BZIMAG;
}

void 
extract_elf(void *img_addr)
{
  uint16_t num_phdr;
  enum elf_type elftype;

  elftype = parse_elfhdrs((struct elf_hdr_common *)img_addr);

  switch (elftype) {
  case ELF_UNSUPPORTED:
    /* probable bzImage */
    if (decode_bzImage(img_addr) == BZIMAG) {
		printf("\n It is bzImage\n");
	} 
    break;
  case ELF_32:
    printf("\n Not bzImage, image type = 32 bit ELF\n");
    break;
  case ELF_64:
    printf("\nNot bzImage, image type = 64 bit ELF\n");
	break;
  default:
	printf("\n Unknown image type\n");
  }
  
}

/*
   In 64-bit boot protocol, the kernel is started by jumping to the
64-bit kernel entry point, which is the start address of loaded
64-bit kernel plus 0x200.

At entry, the CPU must be in 64-bit mode with paging enabled.
The range with setup_header.init_size from start address of loaded
kernel and zero page and command line buffer get ident mapping;
a GDT must be loaded with the descriptors for selectors
__BOOT_CS(0x10) and __BOOT_DS(0x18); both descriptors must be 4G flat
segment; __BOOT_CS must have execute/read permission, and __BOOT_DS
must have read/write permission; CS must be __BOOT_CS and DS, ES, SS
must be __BOOT_DS; interrupt must be disabled; %rsi must hold the base
address of the struct boot_params.
 */

void main(int argc, char *argv[])
{
	FILE *fptr;
	char buf[KRN_HDR];
	uint32_t bytes;

	if (argc != 2) {
		printf("\n Please provide path to bzImage");
		return;
	}
	
	if ((fptr = fopen(argv[1], "r")) == NULL) {
		printf("\n Unable to open file at :%s", argv[1]);
		return;
	}

	/*
	 *  From x86/boot.txt
	 * The first step in loading a Linux kernel should be to load the
	 * real-mode code (boot sector and setup code) and then examine the
	 * following header at offset 0x01f1.  The real-mode code can total up to
	 * 32K, although the boot loader may choose to load only the first two
	 * sectors (1K) and then examine the bootup sector size.
	*/

	if ((bytes = fread(buf, 1, KRN_HDR, fptr)) != KRN_HDR) {
		printf("\n Are you sure it is a Linux kernel image:%u ??", bytes);
		return;
	}

	extract_elf(buf);
}


