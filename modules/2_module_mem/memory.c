#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#define KMALLOC_AREA_SIZE	4096	
#define PAGES_ORDER			1	
#define VMALLOC_AREA_SIZE	8192
	
void *kmalloc_ptr;
void *gfp_ptr;
void *vmalloc_ptr;

static void mem_kmalloc(void)
{
	printk(KERN_CRIT "Allocting memory using kmalloc\n");
	kmalloc_ptr = kmalloc(KMALLOC_AREA_SIZE, GFP_KERNEL);

	if (kmalloc_ptr) {
		printk(KERN_CRIT "Memory allocation using kmalloc succeeded,"
				"kmalloced memory starting address:%p"
				" initial contents:%x\n", kmalloc_ptr, *((uint32_t*)kmalloc_ptr));
		*((uint32_t*)kmalloc_ptr) = 0x55aa;
		printk(KERN_CRIT "Memory contents now:%x", *((uint32_t*)kmalloc_ptr));
	}	
}

static void mem_get_free_pages(void)
{
	printk(KERN_CRIT "Allocting memory using __get_free_pages\n");
	gfp_ptr = __get_free_pages(GFP_KERNEL, PAGES_ORDER);

	if (gfp_ptr) {
		printk(KERN_CRIT "Memory allocation using __get_free_pages succeeded,"
				" initial contents:%x\n", *((uint32_t*)gfp_ptr));
		*((uint32_t*)gfp_ptr) = 0x55aa;
		printk(KERN_CRIT "Memory contents now:%x", *((uint32_t*)gfp_ptr));
	}	
}

static void mem_vmalloc(void)
{
	printk(KERN_CRIT "Allocting memory using vmalloc\n");
	vmalloc_ptr = vmalloc(VMALLOC_AREA_SIZE);

	if (vmalloc_ptr) {
		printk(KERN_CRIT "Memory allocation using vmalloc succeeded,"
				"vmalloced memory starting address:%p"
				" initial contents:%x\n", vmalloc_ptr, *((uint32_t*)vmalloc_ptr));
		*((uint32_t*)vmalloc_ptr) = 0x55aa;
		printk(KERN_CRIT "Memory contents now:%x", *((uint32_t*)vmalloc_ptr));
	}	
}

static int __init memory_init(void) 
{
	printk(KERN_CRIT "Module Loaded\n");
	mem_kmalloc();
	mem_get_free_pages();  
	mem_vmalloc();  
	return 0;
}

static void __exit memory_exit(void) 
{
	printk(KERN_CRIT "Module_exit\n");	
	if (kmalloc_ptr) {
		printk(KERN_CRIT "Freeing memory allocated using kmalloc");
		kfree(kmalloc_ptr);
	}
	if (gfp_ptr) {
		printk(KERN_CRIT "Freeing memory allocated using __get_free_pages");
		free_pages(gfp_ptr,PAGES_ORDER);
	}
	if (vmalloc_ptr) {
		printk(KERN_CRIT "Freeing memory allocated using vmalloc");
		kfree(vmalloc_ptr);
	}
}

module_init(memory_init);
module_exit(memory_exit);

MODULE_LICENSE("GPL");
