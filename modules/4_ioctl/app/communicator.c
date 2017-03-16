#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../module/dev_ioctl.h"

void main(void)
{
  int i;

  int fd = open("/dev/dummy_ioctl_mod", O_RDWR);

  if (fd < 0) { 
    printf("\n Unable to open device file, /dev/dummy_ioctl_module!!\n");
    return;
  }

  if (ioctl(fd, CMD_DUMMY) != 0)
    printf("\n ioctl failed !!\n");
  else 
	printf("\n ioctl success !!\n");

  close(fd);

}
