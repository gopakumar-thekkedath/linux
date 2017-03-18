#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

struct timer_list my_timer1;
struct timer_list my_timer2;
struct workqueue_struct *wq1;
struct workqueue_struct *wq2;
struct work_struct my_wstruct1;
struct work_struct my_wstruct2;

static uint64_t timer_cnt;
DEFINE_MUTEX(my_mutex);
#ifdef USE_TIMER
spinlock_t my_slock;
void timer1_fun (unsigned long arg) 
{
  printk(KERN_CRIT "%lu:timer 1 triggered, try to acquire lock\n", timer_cnt);
   spin_lock(&my_slock);
  printk(KERN_CRIT "timer 1, acquired spin_lock\n");
  mdelay(3000);
  timer_cnt++;
  my_timer1.expires = jiffies + 50;
  add_timer (&my_timer1); /* setup the timer again */
  printk(KERN_CRIT "timer 1, release spin_lock\n");
  spin_unlock(&my_slock);
}

void timer2_fun (unsigned long arg) 
{
  printk(KERN_CRIT "%lu:timer 2 triggered, try to acquire lock\n", timer_cnt);
  spin_lock(&my_slock);
  printk(KERN_CRIT "timer 2, acquired spin_lock\n");
  timer_cnt++;
  my_timer2.expires = jiffies + 60;
  add_timer (&my_timer2); /* setup the timer again */
  printk(KERN_CRIT "timer 2, release spin_lock\n");
  spin_unlock(&my_slock);
}

#endif
static void wqueue_fun1(struct work_struct *work)
{ 
  printk(KERN_CRIT "wqueue 1, attempt to grab semaphore\n");
  mutex_lock(&my_mutex);
  printk(KERN_CRIT "wqueue 1, acquired mutex, sleep for 3 secs");
  msleep(3000);
  printk(KERN_CRIT "wqueue 1, release mutex");
  mutex_unlock(&my_mutex);

}

static void wqueue_fun2(struct work_struct *work)
{ 
  printk(KERN_CRIT "wqueue 2, attempt to grab semaphore\n");
  mutex_lock(&my_mutex);
  printk(KERN_CRIT "wqueue 2, acquired mutex, sleep for 3 secs");
  msleep(3000);
  printk(KERN_CRIT "wqueue 2, release mutex");
  mutex_unlock(&my_mutex);
}

static int __init lock_start(void) 
{
  /* pre-defined kernel variable jiffies gives current value of ticks */
  unsigned long expiryTime = jiffies + HZ; /* HZ gives number of ticks per second */
  
#ifdef USE_TIMER
  init_timer (&my_timer1);
  init_timer (&my_timer2);
  my_timer1.function = timer1_fun;
  my_timer1.expires = expiryTime;
  my_timer1.data = 0;
  my_timer2.function = timer2_fun;
  my_timer2.expires = expiryTime;
  my_timer2.data = 0;
#endif
  wq1 = create_workqueue("my wq1");

  if (!wq1) {
    printk(KERN_ERR "wq1 creation failed\n");
    return -1;
  }
  
  wq2 = create_workqueue("my wq2");
  if (!wq2) {
    printk(KERN_ERR "wq2 creation failed\n");
    destroy_workqueue(wq1);
    return -1;
  }
  INIT_WORK(&my_wstruct1, wqueue_fun1);
  INIT_WORK(&my_wstruct2, wqueue_fun2);

#ifdef USE_TIMER
  spin_lock_init(&my_slock);
  /* XXX: Check add_timer return value */
  add_timer (&my_timer1);
  printk (KERN_INFO "timer 1 added \n");
  add_timer (&my_timer2);
  printk (KERN_INFO "timer 2 added \n");
#endif
  queue_work(wq1, &my_wstruct1);
  queue_work(wq2, &my_wstruct2);

  return 0;
}

static void __exit lock_end(void) 
{
#ifdef USE_TIMER
  if (!del_timer_sync(&my_timer1)) {
    printk (KERN_INFO "Couldn't remove timer 1!!\n");
  }
  else {
    printk (KERN_INFO "timer 1 removed \n");
  }
  
  if (!del_timer_sync(&my_timer2)) {
    printk (KERN_INFO "Couldn't remove timer 2!!\n");
  }
  else {
    printk (KERN_INFO "timer 2 removed \n");
  }
#endif
  destroy_workqueue(wq1);
  destroy_workqueue(wq2);
}

module_init(lock_start);
module_exit(lock_end);

MODULE_LICENSE("GPL");
