## Guide

### struct file_operations

Defined in [linux/fs.h](https://github.com/torvalds/linux/blob/master/include/linux/fs.h)

```clike
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll)(struct kiocb *kiocb, struct io_comp_batch *,
			unsigned int flags);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	void (*splice_eof)(struct file *file);
	int (*setlease)(struct file *, int, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
				   struct file *file_out, loff_t pos_out,
				   loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);
	int (*uring_cmd)(struct io_uring_cmd *ioucmd, unsigned int issue_flags);
	int (*uring_cmd_iopoll)(struct io_uring_cmd *, struct io_comp_batch *,
				unsigned int poll_flags);
} __randomize_layout;
```


### Register a driver to the kernel

Adding a driver to your system means registering it with the kernel, but how?

Defined in [linux/fs.h](https://github.com/torvalds/linux/blob/master/include/linux/fs.h), there's
```clike
int register_chrdev(unsigned int major, const char *name, struct file_operations *fops);
```

但是 `register_chrdev` 有浪費空間的問題（會一起註冊一連串的 minor number），所以最好使用

```clike
// if the major number is known
int register_chrdev_region(dev_t from, unsigned count, const char *name); 

// if the major number is unknown, request a  dynamicly-allocated major number from the kernel
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name); 
```

然後用 `cdev_init` 將`struct cdev` embed 到 `struct file_operations`（Initialize device file），然後用 `cdev_add` 將這個 device 註冊到 kernel 當中。

```clike
void cdev_init(struct cdev *cdev, const struct file_operations *fops);

int cdev_add(struct cdev *p, dev_t dev, unsigned count);
```

- [What is the purpose of class and class device?](https://stackoverflow.com/questions/48832742/what-is-the-purpose-of-class-and-class-device)
- [struct class](https://www.linuxtv.org/downloads/v4l-dvb-internals/device-drivers/API-struct-class.html)
- [device_create](https://www.kernel.org/doc/html/latest/driver-api/infrastructure.html?highlight=device_create)
- [class_create](https://www.kernel.org/doc/html/latest/driver-api/infrastructure.html?highlight=device_create#c.class_create)


在 [linux/kdev_t.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/kdev_t.h) 中定義了以下 macro
```clike
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_KDEV_T_H
#define _UAPI_LINUX_KDEV_T_H
#ifndef __KERNEL__

/*
Some programs want their definitions of MAJOR and MINOR and MKDEV
from the kernel sources. These must be the externally visible ones.
*/
#define MAJOR(dev)	((dev)>>8)
#define MINOR(dev)	((dev) & 0xff)
#define MKDEV(ma,mi)	((ma)<<8 | (mi))
#endif /* __KERNEL__ */
#endif /* _UAPI_LINUX_KDEV_T_H */
```

其他 `character_device.c` 中有用到 api 都可以在 [The Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/index.html#the-linux-kernel-documentation) 中找到定義，並瞭解該如何使用。

其中 `device_read()` 會使用到 locking 的觀念，避免 concurrent read。

## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-1044-raspi/build M=/home/jimmylin/Linux_Driver_Practice/03_character_device modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-1044-raspi'
  CC [M]  /home/jimmylin/Linux_Driver_Practice/03_character_device/character_device.o
  MODPOST /home/jimmylin/Linux_Driver_Practice/03_character_device/Module.symvers
  CC [M]  /home/jimmylin/Linux_Driver_Practice/03_character_device/character_device.mod.o
  LD [M]  /home/jimmylin/Linux_Driver_Practice/03_character_device/character_device.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-1044-raspi'
gcc test.c -o test.out
```

2. check module info: `modinfo character_device.ko`

```console
filename:       /home/jimmylin/Linux_Driver_Practice/03_character_device/character_device.ko
description:    A simple character device
author:         Jimmy Lin
license:        GPL
srcversion:     8122F72144B1E3040A53148
depends:
name:           character_device
vermagic:       5.15.0-1044-raspi SMP preempt mod_unload modversions aarch64
```

3. insert the module with parameters: `sudo insmod character_device.ko`
4. test is our simple character device is successfully inserted into the kernel by opening it: `sudo ./test.out`

```console
[SUCCESS]: file opening successful!
```
5. remove module inserted: `sudo rmmod character_device`
