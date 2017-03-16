#ifndef _DEV_IOCTL_H_
#define _DEV_IOCTL_H_


struct communicator {
  unsigned int index;
  unsigned int value;
};

#define CMD_WRITE_GRP _IOWR ('x', 0x1, struct communicator)
#endif /* _DEV_IOCTL_H_ */
