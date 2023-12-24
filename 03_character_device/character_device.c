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

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUFF_LEN + 1];

static int major;

static struct class* cls;

static struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

static int __init chardev_init(void) {
	major = register_chrdev(0, DEVICE_NAME, &chardev_fops);

	if(major < 0) {
		pr_alert("Register character device failed with major = %d\n", major);
		return major;
	}

	pr_info("Register character device success, major = %d\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	/* create a struct class pointer that can then be used in calls to device_create() */
	cls = class_create(DEVICE_NAME);
#else
	cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
	/* This function can be used by char device classes. A struct device will be created in sysfs, registered to the specified class.
	   A "dev" file will be created, showing the dev_t for the device, if the dev_t is not 0,0.
	*/
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	pr_info("Device created on /dev/%s\n", DEVICE_NAME);

	return SUCCESS;
}


static void __exit chardev_exit(void) {
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	
	unregister_chrdev(major, DEVICE_NAME);	
}

/* Methods:
	Being called when a process tries to open the device file
	e.g. "sudo cat /dev/chardev"
*/
static int device_open(struct inode* inode, struct file* file) {
	static int counter = 0;
	/* 
	 atomic compare and exchange with full ordering.
	 int atomic_cmpxchg(atomic_t *v, int old, int new);
	 If (v == old), atomically updates v to new with full ordering.

	 Return: The original value of v.
	 */
	if(atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) {
		return -EBUSY;
	}	

	sprintf(msg, "I already told you %d times Hello world!\n", counter++); 
	/* take module refcount unless module is being removed */
    try_module_get(THIS_MODULE); 

	return SUCCESS;
}

static int device_release(struct inode* inode, struct file* file) {
	atomic_set(&already_open, CDEV_NOT_USED);
	/* release a reference count to a module.
	   If you successfully bump a reference count to a module with try_module_get(),
	   when you are finished you must call module_put() to release that reference count.
	*/
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file* filp, char __user* buffer, size_t length, loff_t* offset) {
	int bytes_read = 0;
	const char* msg_ptr = msg;
	
	if(!*(msg_ptr + *offset)) { // at the end of message
		*offset = 0;
		return 0;
	}	

	msg_ptr += *offset;

	while(length && *msg_ptr) {
		put_user(*(msg_ptr++), buffer++); /* Write a simple value into user space. */
		length--;
		bytes_read++;
	}

	offset += bytes_read;
	
	return bytes_read;

}

static ssize_t device_write(struct file* filp, const char __user* buff, size_t length, loff_t* offset) {
	pr_alert("Sorry, this operation is not supported!");
	return -EINVAL;
}


module_init(chardev_init);
module_exit(chardev_exit);
