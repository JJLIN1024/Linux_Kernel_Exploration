# Introduction

## System Call

要讓自行 implement 的 function 變成 system call 的方式就是讓 `sys_call_table` 中的 pointer 指向我們自行 implement 的 function。

要如何更動 `sys_call_table`？ 有兩種方式，第一種是 manual symbol lookup，第二種是 `kallsyms_lookup_name`。

```console
jimmylin@pi:~/Linux_Driver_Practice$ sudo grep sys_call_table /proc/kallsyms
[sudo] password for jimmylin: 
ffffbbc8ce211778 D sys_call_table
ffffbbc8ce216ce8 D compat_sys_call_table
```

```console
jimmylin@pi:~/Linux_Driver_Practice$ sudo grep sys_call_table /boot/System.map-$(uname -r)
ffff800008e11778 D sys_call_table
ffff800008e16ce8 D compat_sys_call_table
```