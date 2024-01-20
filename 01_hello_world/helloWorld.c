#include <linux/init.h>   // for macros
#include <linux/module.h> // needed for all module

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jimmy Lin");
MODULE_DESCRIPTION("A hello world module");

static int __init init_hello(void) 
{
	printk("Hello World!\n");
	return 0;
}

static void __exit cleanup_hello(void) {
	printk("Goodbye, world!\n");
}

module_init(init_hello);
module_exit(cleanup_hello);
