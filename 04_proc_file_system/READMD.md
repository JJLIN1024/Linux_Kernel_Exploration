# Guide

## What is /proc in linux kernel ?

- the /proc file system is for the kernel and kernel modules to send information to processes
- /proc是一種偽檔案系統（也即虛擬檔案系統），儲存的是當前核心運作狀態的一系列特殊檔案，使用者可以透過這些檔案查看有關係統硬體及當前正在運行進程的資訊，甚至可以透過更改其中某些檔案來改變核心的運行狀態。

for more information on /proc, see [procfs](https://zh.wikipedia.org/zh-tw/Procfs).


## API

```clike
struct proc_dir_entry *proc_create(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct proc_ops *proc_ops);

void proc_remove(struct proc_dir_entry *);
```

## VFS: struct file & struct path

基礎知識：[Linux中的VFS實作[一]](https://zhuanlan.zhihu.com/p/100329177)


```clike
struct file {
    ...
    ...
	struct path		f_path;
	...
    ...
} __randomize_layout
  __attribute__((aligned(4)));	/* lest something weird decides that 2 is OK */


struct path {
	struct vfsmount *mnt;
	struct dentry *dentry;
} __randomize_layout;


struct dentry {
    ...
    ...
	struct qstr d_name;
    ...
    ...
} __randomize_layout;
```
> - [struct file](https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h#L992)
> - [struct path](https://elixir.bootlin.com/linux/latest/source/include/linux/path.h#L8)


## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-1044-raspi/build M=/home/jimmylin/Linux_Driver_Practice/04_proc_file_system modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-1044-raspi'
  CC [M]  /home/jimmylin/Linux_Driver_Practice/04_proc_file_system/proc_file_system.o
  MODPOST /home/jimmylin/Linux_Driver_Practice/04_proc_file_system/Module.symvers
  CC [M]  /home/jimmylin/Linux_Driver_Practice/04_proc_file_system/proc_file_system.mod.o
  LD [M]  /home/jimmylin/Linux_Driver_Practice/04_proc_file_system/proc_file_system.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-1044-raspi'
```

2. check module info: `modinfo proc_file_system.ko `

```console
filename:       /home/jimmylin/Linux_Driver_Practice/04_proc_file_system/proc_file_system.ko
author:         Jimmy Lin
description:    A hello world program for proc file system
license:        GPL
srcversion:     721230FA77553942764D050
depends:
name:           proc_file_system
vermagic:       5.15.0-1044-raspi SMP preempt mod_unload modversions aarch64
```

3. insert the module with parameters: `sudo insmod proc_file_system.ko`
4. read the module: `cat /proc/hello\ world`

```console
Hello World!
```
5. check kernel debug message: `sudo dmesg | tail -7`:

5. remove module inserted: `sudo rmmod proc_file_system.ko`
