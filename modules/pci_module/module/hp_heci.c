/*
 * Device driver for Intel Corporation Lynx Point-LP HECI
 * Copyright (C) 2015 Seckriti
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/pci.h>
#include <linux/cdev.h>

#include <asm/paravirt.h>
#include <asm/segment.h>
#include <asm/uaccess.h>

#include "dev_ioctl.h"

#define DEV_MAGIC 0x55aa55aa
#define MAX_SIZE 3 

#define DRV_NAME "HECI Driver"
#define HECI_VID 0x8086
#define HECI_DID 0x9c3a

static DEFINE_PCI_DEVICE_TABLE(heci_pci_table) = {
  {PCI_DEVICE(HECI_VID, HECI_DID), },
  { },
};

MODULE_DEVICE_TABLE(pci, heci_pci_table);

struct device_struct {
  unsigned int attribute;
  unsigned int (*funptr)(unsigned int);
};

struct deflt {
  unsigned int magic1;
  unsigned int magic2;
  unsigned int size1;
  unsigned int size2;
};

static struct deflt def;
static struct device_struct dev_struct[MAX_SIZE];

static int my_major = 240;
static struct cdev heci_cdev;

static int my_open(struct inode *inode, struct file *file)
{
  printk(KERN_CRIT "\n my_open");
  return 0;
}

/*
 * Enable GPIO bit 3 for enabling group 0
 */
static unsigned int 
handle_group0(unsigned int offset)
{
  //XXX: Enable GPIO bit 3
  printk(KERN_CRIT "\n handle_group 0");
  return 0x11111110 + offset;
}

/*
 * Set bit 1 of IO mapped control register at offset 2
 * for enabling group 1
 */
static unsigned int 
handle_group1(unsigned int offset)
{
  printk(KERN_CRIT "\n handle_group 1");
  return 0x22222220 + offset;
}

/*
 * Set bit 2 of IO mapped control register at offset 2
 * for enabling group 2 
 */
static unsigned int 
handle_group2(unsigned int offset)
{
  printk(KERN_CRIT "\n handle_group 2");
  return 0x33333330 + offset;
}

/*
 * Based on the input, index of the array is calculated
 * Each index has 10 registers, so 
 * input 0 - 9 = index 0
 * input 10 - 19 = index 1
 * input 20 - 29 = index 2
 */

static unsigned int device_access(unsigned int index)
{
  unsigned int group = index / 10;
  unsigned int offset = index % 10;

  if (dev_struct[group].funptr) {
    return dev_struct[group].funptr(offset);
  }

  return ~0x0U;

}

static long  my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

  struct communicator comm;
  
  int ret = -1;

  switch (cmd) {
    case CMD_WRITE_GRP:
      printk(KERN_CRIT "hp_heci, ioctl CMD_WRITE_GRP\n");
      if (copy_from_user(&comm, (void __user *)arg, sizeof(struct communicator))) {
          printk(KERN_ERR "\n Copy from user failed ");
          return -EFAULT;
      }
      printk(KERN_CRIT "arg=%x", comm.index);
      if ((comm.value = device_access(comm.index)) != ~0x0U) 
        if (copy_to_user((void __user *)arg, &comm, sizeof(struct communicator))) {
          printk(KERN_ERR "\n Copy to user failed ");
          return -EFAULT;
      }
        ret = 0;
      break;

    default:
      printk(KERN_ERR "\n Unsupported command");
   }

  return ret;
}

static const struct file_operations my_fops = {
  .owner          = THIS_MODULE,
  .unlocked_ioctl = my_ioctl,
  .open           = my_open,
};


static void
heci_initialize(void)
{
  def.magic1 = 0xdeadbeef;
  def.magic2 = 0x55aa55aa;
  def.size1 = (unsigned int)0x1a000U;
  def.size2 = (unsigned int)0x0U;

  /*
   * Initialize the dev_struct with attributes and device handling
   * functions.
   */
  dev_struct[0].attribute = 0xaa;
  dev_struct[0].funptr = handle_group0; 
  
  dev_struct[1].attribute = 0xab;
  dev_struct[1].funptr = handle_group1; 
  
  dev_struct[2].attribute = 0xac;
  dev_struct[2].funptr = handle_group2; 
}

static int
heci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
  uint8_t irq;
  int ret;
  dev_t devid;
  void *phys_addr;
  void __iomem *virt_addr;

  printk(KERN_ALERT "\nProbed");
  pci_enable_device(pdev);

  pci_read_config_byte(pdev, PCI_INTERRUPT_LINE, &irq);

  printk(KERN_ALERT "irq %u", irq);
  
  heci_initialize();

  if (my_major) {
	devid = MKDEV(my_major, 0);
	printk(KERN_CRIT "register_chrdev_region\n");
	ret = register_chrdev_region(devid, 1, "heci_module");
  } else { 
	printk(KERN_CRIT "alloc_chrdev_region\n");
    ret = alloc_chrdev_region(&devid, 0, 1, "heci_module");
    my_major = MAJOR(devid); 
  }
	
  if (ret < 0) { 
    printk(KERN_ERR "\nHECI character driver reg failed");
    return ret;
  }
  printk(KERN_CRIT "\n HECI, Major Number=%u\n", my_major);

  cdev_init(&heci_cdev, &my_fops);
  cdev_add(&heci_cdev, devid, 1);

  phys_addr = pci_resource_start(pdev, 0);
  printk(KERN_CRIT "HECI, Physical Address Start :%p\n",phys_addr);
 
  if (pci_enable_device(pdev)) {
    virt_addr = ioremap_nocache(phys_addr, 128);
    if (virt_addr) {
	  uint8_t byte;

	  byte = readb(virt_addr);
	  printk(KERN_ERR "HECI, First byte:%x\n", byte);
      iounmap(virt_addr);
      pci_disable_device(pdev);
    } else 
	  printk(KERN_ERR "ioremap failed\n");
	
  }
    
 
  return 0;
}

static void
heci_remove(struct pci_dev *pdev)
{
  cdev_del(&heci_cdev);
  unregister_chrdev_region(MKDEV(my_major, 0), 1);
}

static struct pci_driver heci_pci_driver = {
  .name           = DRV_NAME,
  .id_table       = heci_pci_table,
  .probe          = heci_probe,
  .remove         = heci_remove,
};


static int __init heci_start(void) 
{
  printk(KERN_CRIT "heci module init");
  return pci_register_driver(&heci_pci_driver);

  return 0;
}

static void __exit heci_end(void) 
{
  printk(KERN_CRIT "heci module exit");
  pci_unregister_driver(&heci_pci_driver);
}

module_init(heci_start);
module_exit(heci_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seclearn");
MODULE_DESCRIPTION("HECI Driver");

