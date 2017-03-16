/*
 * Copyright (C) 2015 Seckriti
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>
#include "dev_ioctl.h"

static int my_major;
static struct cdev dummy_cdev;
static struct communicator comm;
static int my_open(struct inode *inode, struct file *file)
{
  printk(KERN_CRIT "\n my_open");
  return 0;
}

static long  my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  int ret = -1;

  switch (cmd) {
	case CMD_DUMMY_XFR:
      printk(KERN_CRIT "ioctl CMD_DUMMY_XFR\n");
      if ((ret = copy_from_user(&comm, (void __user *)arg, sizeof(struct communicator))) != 0) {
          printk(KERN_ERR "\n Copy from user failed ");
          return ret;
      }
      printk(KERN_CRIT "index=%d value=%d", comm.index, comm.value);
      break;

    default:
      printk(KERN_ERR "\n Unsupported ioctl");
   }

  return ret;
}

static const struct file_operations my_fops = {
  .owner          = THIS_MODULE,
  .unlocked_ioctl = my_ioctl,
  .open           = my_open,
};

static int __init dummy_start(void) 
{
 
  int ret; 
  dev_t devid;

  if (my_major) {
    devid = MKDEV(my_major, 0);
    printk(KERN_CRIT "register_chrdev_region\n");
    ret = register_chrdev_region(devid, 1, "dummy_module");
  } else {
    printk(KERN_CRIT "alloc_chrdev_region\n");
    ret = alloc_chrdev_region(&devid, 0, 1, "dymmy_module");
    my_major = MAJOR(devid);
  }

  if (ret < 0) {
      printk(KERN_ERR "\nDUMMY IOCTL character driver reg failed");
      return -1;
  }

  printk(KERN_CRIT "\n Major Number=%u", my_major);

  cdev_init(&dummy_cdev, &my_fops);
  cdev_add(&dummy_cdev, devid, 1);

  return 0;
}

static void __exit dummy_end(void) 
{
  cdev_del(&dummy_cdev);
  unregister_chrdev_region(MKDEV(my_major, 0), 1);
}

module_init(dummy_start);
module_exit(dummy_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seclearn");
MODULE_DESCRIPTION("Test Driver");

