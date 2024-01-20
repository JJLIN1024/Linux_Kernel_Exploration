# Linux_Driver_Practice

## Overview

This repository contains code of Linux kernel modules, for the purpose of learning Linux kernel module programming while studying reference resources.

All modules were compiled and tested on VM Ubuntu server 22.04.3 LTS, using [UTM](https://mac.getutm.app/).

```console
jimmy@ubuntu:~$ uname -a
Linux ubuntu 5.15.0-91-generic #101-Ubuntu SMP Tue Nov 14 13:29:11 UTC 2023 aarch64 aarch64 aarch64 GNU/Linux
```

## Tracing Linux Kernel?

I use Visual Studio Code, and Microsoft's C/C++ Extension. 

Modified Microsoft's C/C++ Extension's setting, in `settings.json`:

```json
"C_Cpp.files.exclude": {
        "**/.vscode": true,
        "**/.vs": true,
        "**/.git": true,
        "**/.svn": true,
        "**/.DS_Store": true,
        "**/drivers": true,
        "**/sound": true,
        "**/tools": true,
        "**/arch/alpha": true,
        "**/arch/arc": true,
        "**/arch/c6x": true,
        "**/arch/h8300": true,
        "**/arch/hexagon": true,
        "**/arch/ia64": true,
        "**/arch/m32r": true,
        "**/arch/m68k": true,
        "**/arch/microblaze": true,
        "**/arch/mn10300": true,
        "**/arch/nds32": true,
        "**/arch/nios2": true,
        "**/arch/parisc": true,
        "**/arch/powerpc": true,
        "**/arch/s390": true,
        "**/arch/sparc": true,
        "**/arch/score": true,
        "**/arch/sh": true,
        "**/arch/um": true,
        "**/arch/unicore32": true,
        "**/arch/xtensa": true,
        "**/Documentation": true,
        "**/LICENSES": true,
        "**/crypto": true,
    }
```

This will speed up the search process, make the code tracing less cpu-intensive.

## Reference
- [The Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)
- [Linux_Driver_Tutorial](https://github.com/Johannes4Linux/Linux_Driver_Tutorial)
- [Operating Systems 2¶](https://linux-kernel-labs.github.io/refs/heads/master/so2/index.html)