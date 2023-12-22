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
