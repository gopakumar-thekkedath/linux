#include <linux/module.h>
#include <linux/kernel.h>

void
my_dummy_fun(void)
{
	printk(KERN_CRIT "Wastage of space\n");
}

static int __init hello_init(void) 
{
	printk(KERN_ERR "Hello Module Loaded\n");
	return 0;
}

static void __exit hello_exit(void) 
{
	printk(KERN_CRIT "Module_exit\n");	
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(my_dummy_fun);
