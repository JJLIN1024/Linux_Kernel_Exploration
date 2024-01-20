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

## Code Tracing

### module.h

```c
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jimmy Lin");
MODULE_DESCRIPTION("A hello world module");
```

These three macros are defined in `linux/module.h`:

```c
#define MODULE_LICENSE(_license) MODULE_FILE MODULE_INFO(license, _license)

/*
 * Author(s), use "Name <email>" or just "Name", for multiple
 * authors use multiple MODULE_AUTHOR() statements/lines.
 */
#define MODULE_AUTHOR(_author) MODULE_INFO(author, _author)

/* What your module does. */
#define MODULE_DESCRIPTION(_description) MODULE_INFO(description, _description)
```

which all use `MODULE_INFO` macro:

```c
/* Generic info of form tag = "info" */
#define MODULE_INFO(tag, info) __MODULE_INFO(tag, tag, info)
```

`MODULE_INFO` is defined in `linux/moduleparam.h`:

```c
#define __MODULE_INFO(tag, name, info)					  \
	static const char __UNIQUE_ID(name)[]				  \
		__used __section(".modinfo") __aligned(1)		  \
		= __MODULE_INFO_PREFIX __stringify(tag) "=" info
```
where `__UNIQUE_ID` is defined in `linux/compiler.h`:

```c
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)
```

where  `__PASTE` is defined in `linux/compiler_types.h`:
```c
/* Indirect macros required for expanded argument pasting, eg. __LINE__. */
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

```

For more information on the `__PASTE, __COUNTER__` macros, see:

The key point is that: Each macro argument whose name appears in the macro body not preceeded by # or ## or followed by ## is "prescanned" for macros to expand -- any macros entirely within the argument will be recursively expanded before substituting into the macro body.

In other words, those macro arguments that is preceeded by # or ## or followed by ## **are not "prescanned"**!, so in case like

```c
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)
```

If we don't use the indirect macro, then the inner `__PASTE` will not be expanded, which is not the desired result.

- [Argument Prescan](https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html)
- [你所不知道的 C 語言：前置處理器應用篇](https://hackmd.io/@sysprog/c-preprocessor)
- [Linux 核心原始程式碼巨集: max, min](https://hackmd.io/@sysprog/linux-macro-minmax#%E9%81%BF%E5%85%8D%E5%91%BD%E5%90%8D%E8%A1%9D%E7%AA%81)




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
