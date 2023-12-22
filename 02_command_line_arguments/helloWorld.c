#include <linux/init.h>   // for macros
#include <linux/module.h> // needed for all module
#include <linux/printk.h> // pr_info()
#include <linux/kernel.h>       // ARRAY_SIZE()
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LKMPG");
MODULE_DESCRIPTION("A sample driver");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myintarray[2] = {420, 420};
static int arr_argc = 0;

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
MODULE_PARM_DESC(myint, "An integer"); 
module_param(mylong, long, S_IRUSR); 
MODULE_PARM_DESC(mylong, "A long integer"); 
module_param(mystring, charp, 0000); 
MODULE_PARM_DESC(mystring, "A character string"); 

module_param_array(myintarray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintarray, "An array of integers"); 


static int __init init_hello(void) 
{
	int i; 
 
    pr_info("Hello, world 5\n=============\n"); 
    pr_info("myshort is a short integer: %hd\n", myshort); 
    pr_info("myint is an integer: %d\n", myint); 
    pr_info("mylong is a long integer: %ld\n", mylong); 
    pr_info("mystring is a string: %s\n", mystring); 

	for(i = 0; i < ARRAY_SIZE(myintarray); i++) {
		pr_info("myintarray[%d] = %d\n", i, myintarray[i]);
	}
	pr_info("got %d arguments for myintarray.\n", arr_argc);
	return 0;
}

static void __exit cleanup_hello(void) {
	pr_info("Goodbye, world!\n");
}
