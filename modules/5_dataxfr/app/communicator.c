#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../module/dev_ioctl.h"

static struct communicator comm;
void main(int argc, char **argv)
{
  int i;

  if (argc != 2) {
    printf ("\nUsage communicator <index>");
    return;
  }

  comm.index = atoi(argv[1]);
  comm.value = comm.index + 5;

  int fd = open("/dev/dummy_dataxfr", O_RDWR);

  if (fd < 0) { 
    printf("\n Unable to open device file, /dev/dummy_dataxfr!!\n");
    return;
  }

  if (ioctl(fd, CMD_DUMMY_XFR, &comm) != 0)
    printf("\n ioctl failed !!\n");
  else 
	printf("\n ioctl success !!\n");

  close(fd);

}
