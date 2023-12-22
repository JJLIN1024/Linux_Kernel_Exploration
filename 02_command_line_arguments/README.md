## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-1044-raspi/build M=/home/jimmylin/Linux_Driver_Practice/02_command_line_arguments modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-1044-raspi'
  CC [M]  /home/jimmylin/Linux_Driver_Practice/02_command_line_arguments/command_line_arguments.o
  MODPOST /home/jimmylin/Linux_Driver_Practice/02_command_line_arguments/Module.symvers
  CC [M]  /home/jimmylin/Linux_Driver_Practice/02_command_line_arguments/command_line_arguments.mod.o
  LD [M]  /home/jimmylin/Linux_Driver_Practice/02_command_line_arguments/command_line_arguments.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-1044-raspi'
```

2. check module info: `modinfo command_line_arguments.ko`

```console
filename:       /home/jimmylin/Linux_Driver_Practice/02_command_line_arguments/command_line_arguments.ko
description:    A sample driver that accepts command line arguments
author:         Jimmy Lin
license:        GPL
srcversion:     8EABCC322423F86876C18B2
depends:
name:           command_line_arguments
vermagic:       5.15.0-1044-raspi SMP preempt mod_unload modversions aarch64
parm:           myshort:short
parm:           myint:An integer (int)
parm:           mylong:A long integer (long)
parm:           mystring:A character string (charp)
parm:           myintarray:An array of integers (array of int)
```

3. insert the module with parameters: `sudo insmod command_line_arguments.ko myshort=13 myintarray=1,2`
4. `sudo dmesg -t | tail -7`

```console
myshort is a short integer: 13
myint is an integer: 420
mylong is a long integer: 9999
mystring is a string: blah
myintarray[0] = 1
myintarray[1] = 2
got 2 arguments for myintarray.
```

5. remove inserted module: `sudo rmmod command_line_arguments`
