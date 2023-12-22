## Guide

- How to build external module (not build-in) ?

```markdown
make -C $KDIR M=$PWD

-C $KDIR
The directory where the kernel source is located. "make" will actually change to the specified directory when executing and will change back when finished.

M=$PWD
Informs kbuild that an external module is being built. The value given to "M" is the absolute path of the directory where the external module (kbuild file) is located.
```
> [Building External Modules](https://docs.kernel.org/kbuild/modules.html)

- What is the `__init` and `__exit` macro doing ?
	- The `__init` macro causes the init function to be discarded and its memory freed once the init function finishes for built-in drivers, but not loadable modules
	- The `__exit` macro causes the omission of the function when the module is built into the kernel, and like __init , has no effect for loadable modules.
	- for more information, check out [Kernel modules init macro in C](https://stackoverflow.com/questions/48558460/kernel-modules-init-macro-in-c)


## Usage

1. `make`
2. check module info: `modinfo helloWorld.ko`

```console
filename:       ~/Linux_Driver_Practice/hello_world/helloWorld.ko
description:    A hello world module
author:         Jimmy Lin
license:        GPL
srcversion:     95D3D10D08D29DAA8B9BE27
depends:
name:           helloWorld
vermagic:       5.15.0-1044-raspi SMP preempt mod_unload modversions aarch64
```

3. insert module into the kernel: `sudo insmod helloWorld.ko`
4. check if the module is successfully inserted: `sudo lsmod | grep helloWorld`
5. remove the module: `sudo rmmod helloWorld`
