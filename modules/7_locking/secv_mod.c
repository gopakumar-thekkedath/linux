#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/wait.h>

#define MSG_SIZE 100
static wait_queue_head_t secv_waitq;
unsigned long **sys_call_table;

/* setup timer */
struct timer_list myTimer;
char *msg_buffer;
static int my_major;

void timerFun (unsigned long arg) {
  if (msg_buffer[0] != 0)
  {
    printk (KERN_CRIT "%s", msg_buffer);
    memset (msg_buffer,0,MSG_SIZE);
    printk(KERN_CRIT "Time to wake up from poll sleep");
    wake_up_interruptible(&secv_waitq);
  }
  myTimer.expires = jiffies + HZ;
  add_timer (&myTimer); /* setup the timer again */
}

static unsigned long **aquire_sys_call_table(void)
{
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;

  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) 
      return sct;

    offset += sizeof(void *);
  }

  return NULL;
}

static int secv_open(struct inode *inode, struct file *file)
{
  return 0;
}

static unsigned int secv_poll(struct file *file, poll_table *wait)
{
  printk(KERN_CRIT "Poll called");  
  poll_wait(file, &secv_waitq, wait);
  printk(KERN_CRIT "Poll sleep over");

  if (msg_buffer[0] != 0) {
    memset (msg_buffer,0,MSG_SIZE);
    return POLLIN;
  } 
   
  return 0;
}

static int secv_release(struct inode *inode, struct file *file)
{
  return 0;
}

static const struct file_operations secv_fops = {
  .owner          = THIS_MODULE,
  .poll           = secv_poll,
  .open           = secv_open,
  .release        = secv_release,
};

static int __init sec_start(void) 
{
  /* pre-defined kernel variable jiffies gives current value of ticks */
  unsigned long currentTime = jiffies; 
  unsigned long expiryTime = currentTime + HZ; /* HZ gives number of ticks per second */
  unsigned long  addr=0;
  unsigned long  callno = 1;
  if(!(sys_call_table = aquire_sys_call_table()))
    return -1;
  addr = (unsigned long )sys_call_table - 0xffff880000000000UL;
  addr = addr & (~0xFFFUL);
  printk(KERN_ALERT "sys_table: 0x%lx, 0x%lx", (unsigned long )sys_call_table, addr);
  printk(KERN_ALERT "virt,physical: 0x%lx, 0x%lx", virt_to_phys(sys_call_table), phys_to_virt(addr));
  asm volatile("vmcall" : :"a"(callno),"b"(addr) : );

  msg_buffer =  kmalloc(MSG_SIZE, GFP_KERNEL);
  memset (msg_buffer,0,MSG_SIZE);
  callno = 2;
  addr = virt_to_phys(msg_buffer);
  asm volatile("vmcall" : :"a"(callno),"b"(addr) : );

#define VMEXIT_COUNT
#ifdef VMEXIT_COUNT
{
  uint64_t exit_cnt = 0x0;
  callno = 3;
  asm volatile("vmcall" :"=b"(exit_cnt) :"a"(callno): );
  printk(KERN_ALERT "Sec-V: VMExit count=%lu", exit_cnt);
}
#endif
  init_timer (&myTimer);
  myTimer.function = timerFun;
  myTimer.expires = expiryTime;
  myTimer.data = 0;

  add_timer (&myTimer);
  printk (KERN_INFO "timer added \n");

  my_major = register_chrdev(my_major, "secv_module", &secv_fops);
  if (my_major < 0) {
    printk(KERN_ERR "\nSec-V character driver reg failed");
  }

  printk(KERN_CRIT "\n Sec-V, Major Number=%u", my_major);
  
  init_waitqueue_head(&secv_waitq);

  return 0;
}

static void __exit sec_end(void) 
{
  if (!del_timer (&myTimer)) {
    printk (KERN_INFO "Couldn't remove timer!!\n");
  }
  else {
    printk (KERN_INFO "timer removed \n");
  }

  unregister_chrdev(my_major, "secv_module");

  kfree(msg_buffer);
}

module_init(sec_start);
module_exit(sec_end);

MODULE_LICENSE("GPL");
