#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A hello world program for proc file system");
MODULE_AUTHOR("Jimmy Lin");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define proc_fs_name "hello world"
#define PROCFS_MAX_SIZE 1024

static struct proc_dir_entry* proc_file;
static char proc_file_buffer[PROCFS_MAX_SIZE];
static unsigned long proc_file_buffer_size = 0;

static ssize_t proc_file_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) {
	char s[13] = "Hello World!\n";
	int len = sizeof(s);
	ssize_t ret = len;

	if(*offset >= len || copy_to_user(buffer, s, len)) {
		pr_info("copy_to_user failed\n");
        ret = 0;
	} else {
		pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
		*offset += len;
	}

	return ret;
}

static ssize_t proc_file_write(struct file *file_pointer, const char __user* buffer, size_t len, loff_t *offset) {
	proc_file_buffer_size = len;
	if(proc_file_buffer_size > PROCFS_MAX_SIZE) {
		proc_file_buffer_size = PROCFS_MAX_SIZE;
	}

	if (copy_from_user(proc_file_buffer, buffer, proc_file_buffer_size))
        return -EFAULT;

    proc_file_buffer[proc_file_buffer_size & (PROCFS_MAX_SIZE - 1)] = '\0';
    *offset += proc_file_buffer_size;
    pr_info("procfile write %s\n", proc_file_buffer);

    return proc_file_buffer_size;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_ops = {
	.proc_read = proc_file_read,
	.proc_write = proc_file_write
};
#else
static const struct file_operations proc_file_ops = {
	.read = proc_file_read,
	.write = proc_file_write
};
#endif

static int __init proc_init(void) {
	proc_file = proc_create(proc_fs_name, 0644, NULL, &proc_file_ops);
	if(proc_file == NULL) {
		proc_remove(proc_file);
		pr_alert("[Error]: /proc/%s initialization failed!\n", proc_fs_name);
		return -ENOMEM;
	}

	pr_info("/proc/%s created\n", proc_fs_name);
	return 0;
}

static void __exit proc_exit(void) {
	proc_remove(proc_file);
	pr_info("/proc/%s removed!\n", proc_fs_name);	
}

module_init(proc_init);
module_exit(proc_exit);
