#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../module/dev_ioctl.h"

void main(int argc, char **argv)
{
  int action;

  if (argc != 2) {
    printf ("\nUsage communicator <action, 1 = sleep, 2 = unintr sleep,  3 = wakeup>\n");
    return;
  }

  action = atoi(argv[1]);

  int fd = open("/dev/dummy_sleep", O_RDWR);

  if (fd < 0) { 
    printf("\n Unable to open device file, /dev/dummy_sleep!!\n");
    return;
  }

  switch(action) {
  case 1:
    if (ioctl(fd, CMD_DUMMY_INTR_SLEEP) != 0)
      printf("\n ioctl failed !!\n");
    else 
	  printf("\n ioctl success !!\n");
	break;
  case 2:
    if (ioctl(fd, CMD_DUMMY_UNINTR_SLEEP) != 0)
      printf("\n ioctl failed !!\n");
    else 
	  printf("\n ioctl success !!\n");
	break;
  case 3:
    if (ioctl(fd, CMD_DUMMY_COND_WAKE) != 0)
      printf("\n ioctl failed !!\n");
    else 
	  printf("\n ioctl success !!\n");
	break;
  default:
    printf("\n Invalid action");
  }

  close(fd);
}
