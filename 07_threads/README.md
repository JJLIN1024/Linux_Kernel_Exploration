## Guide

## Code Tracing


## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-91-generic/build M=/home/jimmy/Linux_Driver_Practice/07_threads modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-91-generic'
  CC [M]  /home/jimmy/Linux_Driver_Practice/07_threads/threads.o
  MODPOST /home/jimmy/Linux_Driver_Practice/07_threads/Module.symvers
  CC [M]  /home/jimmy/Linux_Driver_Practice/07_threads/threads.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/07_threads/threads.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/07_threads/threads.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/07_threads/threads.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-91-generic'
```

2. check module info: `modinfo threads.ko`

```console
filename:       /home/jimmy/Linux_Driver_Practice/07_threads/threads.ko
license:        GPL
description:    Completions example
srcversion:     A86692770DFEFCF09EC56EA
depends:        
name:           threads
vermagic:       5.15.0-91-generic SMP mod_unload modversions aarch64
```
3. insert module into the kernel: `sudo insmod threads.ko`

4. check if the module is successfully inserted: `sudo lsmod | grep threads`

```console
threads                16384  0
```

5. check kernel ring buffer: `sudo dmesg -k | tail -5`

```console
[  231.204818] threads: loading out-of-tree module taints kernel.
[  231.204877] threads: module verification failed: signature and/or required key missing - tainting kernel
[  231.205297] completions example
[  231.206616] Turn the crank
[  231.206646] Flywheel spins up
```

6. remove the module: `sudo rmmod threads`
7. clean up: `make clean`
