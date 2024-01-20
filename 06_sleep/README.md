


## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-91-generic/build M=/home/jimmy/Linux_Driver_Practice/06_sleep modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-91-generic'
  CC [M]  /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.o
  MODPOST /home/jimmy/Linux_Driver_Practice/06_sleep/Module.symvers
  CC [M]  /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-91-generic'
gcc test.c -o cat_nonblock
```

2. check module info: `modinfo sleep.ko`

```console
filename:       /home/jimmy/Linux_Driver_Practice/06_sleep/sleep.ko
license:        GPL
srcversion:     54ECDB7965AE96459D8F160
depends:        
name:           sleep
vermagic:       5.15.0-91-generic SMP mod_unload modversions aarch64
```
3. insert module into the kernel: `sudo insmod sleep.ko`

4. check if the module is successfully inserted: `sudo lsmod | grep sleep`

```console
jimmy@ubuntu:~/Linux_Driver_Practice/06_sleep$ sudo lsmod | grep sleep
sleep                  20480  0
```

5. `test.c`

First we use `./cat_nonblock` to read `/proc/sleep`, which successfully return `Last input:` to the stdout.

Next, use `tail -f` command to read `/proc/sleep`, so that when `./cat_nonblock` tries to read `/proc/sleep`, it will fail to read and recieve `EAGAIN` since the I/O mode is non-blocking.

Then we kill `tail -f` with `kill %1`, and then we try `./cat_nonblock` again, this time the atomic exchange of if will succeed, so we see the output of `Last input:` in stdout.

```console
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Last input:
jimmy@ubuntu ~/L/06_sleep (main)> tail -f /proc/sleep &
Last input:
Last input:
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Open would block
jimmy@ubuntu ~/L/06_sleep (main) [255]> kill %1
jimmy@ubuntu ~/L/06_sleep (main)> fish: Job 1, 'tail -f /proc/sleep &' terminated by signal SIGTERM (Polite quit request)
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Last input:
```
> use Ctrl-Z to suspend a running process, and then use `bg %n` to send the n-th job to the background and continue executing. Or one can use the `&` to directly send a job to the background to execute.
>
> Down below is another way to use the `test.c` to perform the test.

```console
jimmy@ubuntu ~/L/06_sleep (main) [1]> ./cat_nonblock /proc/sleep
Last input:
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Last input:
jimmy@ubuntu ~/L/06_sleep (main)> tail -f /proc/sleep > /dev/null 2>&1
^Zfish: Job 1, 'tail -f /proc/sleep > /dev/null…' has stopped
jimmy@ubuntu ~/L/06_sleep (main)> jobs
Job	Group	CPU	State	Command
1	3452	0%	stopped	tail -f /proc/sleep > /dev/null 2>&1
jimmy@ubuntu ~/L/06_sleep (main)> bg %1
Send job 1 'tail -f /proc/sleep > /dev/null 2>&1' to background
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Open would block
jimmy@ubuntu ~/L/06_sleep (main) [255]> kill %1
jimmy@ubuntu ~/L/06_sleep (main)> fish: Job 1, 'tail -f /proc/sleep > /dev/null…' terminated by signal SIGTERM (Polite quit request)
jimmy@ubuntu ~/L/06_sleep (main)> ./cat_nonblock /proc/sleep
Last input:
```

6. remove the module: `sudo rmmod helloWorld`


