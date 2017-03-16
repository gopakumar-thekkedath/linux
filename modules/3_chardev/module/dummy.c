/*
 * Copyright (C) 2015 Seckriti
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "dev_ioctl.h"

static int my_major;
static struct cdev dummy_cdev;


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
    ret = alloc_chrdev_region(&devid, 0, 1, "dummy_module");
    my_major = MAJOR(devid);
  }

  if (ret < 0) {
      printk(KERN_ERR "\nDUMMY character driver reg failed");
      return -1;
  }

  printk(KERN_CRIT "\n Major Number=%u", my_major);

  cdev_init(&dummy_cdev, NULL);
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

