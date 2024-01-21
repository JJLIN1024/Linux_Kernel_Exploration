## Guide

## Code Tracing

## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-91-generic/build M=/home/jimmy/Linux_Driver_Practice/08_locks modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-91-generic'
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.o
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:2:17: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘.’ token
    2 | * example_atomic.c
      |                 ^
In file included from ./include/uapi/asm-generic/types.h:7,
                 from ./arch/arm64/include/generated/uapi/asm/types.h:1,
                 from ./include/uapi/linux/types.h:5,
                 from ./include/linux/types.h:6,
                 from ./include/linux/atomic.h:5,
                 from /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:5:
./include/asm-generic/int-ll64.h:16:9: error: unknown type name ‘__s8’
   16 | typedef __s8  s8;
      |         ^~~~
In file included from ./include/linux/quota.h:42,
                 from ./include/linux/fs.h:244,
                 from ./arch/arm64/include/asm/elf.h:141,
                 from ./include/linux/elf.h:6,
                 from ./include/linux/module.h:19,
                 from /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:7:
./include/uapi/linux/dqblk_xfs.h:54:9: error: unknown type name ‘__s8’
   54 |         __s8            d_version;      /* version of this structure */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:55:9: error: unknown type name ‘__s8’
   55 |         __s8            d_flags;        /* FS_{USER,PROJ,GROUP}_QUOTA */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:70:9: error: unknown type name ‘__s8’
   70 |         __s8            d_itimer_hi;    /* upper 8 bits of timer values */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:71:9: error: unknown type name ‘__s8’
   71 |         __s8            d_btimer_hi;
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:72:9: error: unknown type name ‘__s8’
   72 |         __s8            d_rtbtimer_hi;
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:73:9: error: unknown type name ‘__s8’
   73 |         __s8            d_padding2;     /* padding2 - for future use */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:166:9: error: unknown type name ‘__s8’
  166 |         __s8            qs_version;     /* version number for future changes */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:168:9: error: unknown type name ‘__s8’
  168 |         __s8            qs_pad;         /* unused */
      |         ^~~~
./include/uapi/linux/dqblk_xfs.h:210:9: error: unknown type name ‘__s8’
  210 |         __s8                    qs_version;     /* version for future changes */
      |         ^~~~
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:11:80: warning: backslash and newline separated by space
   11 | #define BYTE_TO_BINARY(byte)                                                   \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:12:80: warning: backslash and newline separated by space
   12 |     ((byte & 0x80) ? '1' : '0'), ((byte & 0x40) ? '1' : '0'),                  \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:13:80: warning: backslash and newline separated by space
   13 |         ((byte & 0x20) ? '1' : '0'), ((byte & 0x10) ? '1' : '0'),              \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:14:80: warning: backslash and newline separated by space
   14 |         ((byte & 0x08) ? '1' : '0'), ((byte & 0x04) ? '1' : '0'),              \
      |                                                                                 
make[2]: *** [scripts/Makefile.build:297: /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.o] Error 1
make[1]: *** [Makefile:1909: /home/jimmy/Linux_Driver_Practice/08_locks] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-91-generic'
make: *** [Makefile:9: all] Error 2
jimmy@ubuntu:~/Linux_Driver_Practice/08_locks$ make
make -C /lib/modules/5.15.0-91-generic/build M=/home/jimmy/Linux_Driver_Practice/08_locks modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-91-generic'
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.o
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:10:80: warning: backslash and newline separated by space
   10 | #define BYTE_TO_BINARY(byte)                                                   \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:11:80: warning: backslash and newline separated by space
   11 |     ((byte & 0x80) ? '1' : '0'), ((byte & 0x40) ? '1' : '0'),                  \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:12:80: warning: backslash and newline separated by space
   12 |         ((byte & 0x20) ? '1' : '0'), ((byte & 0x10) ? '1' : '0'),              \
      |                                                                                 
/home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.c:13:80: warning: backslash and newline separated by space
   13 |         ((byte & 0x08) ? '1' : '0'), ((byte & 0x04) ? '1' : '0'),              \
      |                                                                                 
  MODPOST /home/jimmy/Linux_Driver_Practice/08_locks/Module.symvers
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/RWLock.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/08_locks/RWLock.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/08_locks/RWLock.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/08_locks/RWLock.ko due to unavailability of vmlinux
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/08_locks/atomicOp.ko due to unavailability of vmlinux
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/mutex.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/08_locks/mutex.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/08_locks/mutex.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/08_locks/mutex.ko due to unavailability of vmlinux
  CC [M]  /home/jimmy/Linux_Driver_Practice/08_locks/spinlock.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/08_locks/spinlock.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/08_locks/spinlock.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/08_locks/spinlock.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-91-generic'
```

2. check module info: `modinfo .ko`

```console

```
3. insert module into the kernel: `sudo insmod .ko`

4. check if the module is successfully inserted: `sudo lsmod | grep `

```console

```

5. check kernel ring buffer: `sudo dmesg -k | tail -3`

```console

```

6. remove the module: `sudo rmmod ` & `sudo dmesg -k | tail -4`

```console
```


7. clean up: `make clean`
