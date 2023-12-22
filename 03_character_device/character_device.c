#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/atomic.h> 
#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>

#include <asm/errno.h>

#include "character_device.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jimmy Lin");
MODULE_DESCRIPTION("A simple character device");

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUFF_LEN 80

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

/* Is device open? Used to prevent multiple access to device */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN + 1]; /* The msg the device will give when asked */

static int major;

static struct class* cls;

static struct file_operations chardev_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

static int __init chardev_init(void) {
	major = register_chardev(0, DEVICE_NAME, &chardev_fops);

	if(major < 0) {
		pr_alert("Register character device failed with major = %d\n", major);
		return major;
	}

	pr_info("Register character device success, major = %d\n", major);

#if LINUX_VERSION_NUMBER >= KERNEL_VERSION(6, 4, 0)
	cls = class_create(DEVICE_NAME);
#else
	cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif

	device_create(cls, NULL, MEDEV(major, 0), NULL, DEVICE_NAME);
	pr_info("Device created on /dev/%s\n", DEVICE_NAME);

	return SUCCESS;
}


static void __exit chardev_exit(void) {
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	
	unregister_chrdev(major, DEVICE_NAME);	
}

/* Methods
	calls when a process tries to open the device file
	e.g. "sudo cat /dev/chardev"
*/
static int device_open(struct inode *, struct file *) {
	static int counter = 0;
	if(atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) {
		return -EBUSY;
	}	

	sprintf(msg, "I already told you %d times Hello world!\n", counter++); 
    try_module_get(THIS_MODULE); 
}

static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *); 


module_init(chardev_init);
module_exit(chardev_exit);
