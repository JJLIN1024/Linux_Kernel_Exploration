## Guide

What `sleep.c` does is create a `/proc/sleep` file for read and write, and use atomic operations and wait queue to ensure there can only be one process to read it at a time.

`test.c` attemps to read `/proc/sleep` in a non-blocking way, and `tail -f` behaves as another process that attemps to read `/proc/sleep`, too. For more I/O model behaviors, see [事件驅動伺服器：原理和實例](https://hackmd.io/@sysprog/linux-io-model/https%3A%2F%2Fhackmd.io%2F%40sysprog%2Fevent-driven-server).

## Code Tracing

### Transfer data between user space and kernel space

For `copy_to_user()` / `copy_from_user()` / `get_user()` / `put_user()`, see [Unreliable Guide To Hacking The Linux Kernel](https://www.kernel.org/doc/html/v4.13/kernel-hacking/hacking.html)


### Wait Queues and Atomic Operations

Read [Unreliable Guide To Hacking The Linux Kernel](https://www.kernel.org/doc/html/v4.13/kernel-hacking/hacking.html) first.

`ATOMIC_INIT` is defined in `include/asm/atomic.h`:

```c
#define ATOMIC_INIT(i)	  { (i) }
```

`DECLARE_WAIT_QUEUE_HEAD` is defined in `linux/wait.h`:

```c
#define DECLARE_WAIT_QUEUE_HEAD(name) \
	struct wait_queue_head name = __WAIT_QUEUE_HEAD_INITIALIZER(name)
```

```c
/*
 * A single wait-queue entry structure:
 */
struct wait_queue_entry {
	unsigned int		flags;
	void			*private;
	wait_queue_func_t	func;
	struct list_head	entry;
};

struct wait_queue_head {
	spinlock_t		lock;
	struct list_head	head;
};
```

`include/asm/atomic.h`

```c
static __always_inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return cmpxchg(&v->counter, old, new);
}
```

`include/asm/cmpxchg.h`

Here assembly language is used, and here is the general format:

```assembly
asm ( assembler template
    : output operands                   (optional)
    : input operands                    (optional)
    : clobbered registers list          (optional)
    );
```

It is observed that `__raw_cmpxchg` use assembly langauge to do the swap part of old and new, the `lock` keyword ensures atomic operations.

```c
#define __raw_cmpxchg(ptr, old, new, size, lock)			\
({									\
	__typeof__(*(ptr)) __ret;					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	switch (size) {							\
	case __X86_CASE_B:						\
	{								\
		volatile u8 *__ptr = (volatile u8 *)(ptr);		\
		asm volatile(lock "cmpxchgb %2,%1"			\
			     : "=a" (__ret), "+m" (*__ptr)		\
			     : "q" (__new), "0" (__old)			\
			     : "memory");				\
		break;							\
	}	
  ...
  ...
})

#define __cmpxchg(ptr, old, new, size)					\
	__raw_cmpxchg((ptr), (old), (new), (size), LOCK_PREFIX)

#define cmpxchg(ptr, old, new)						\
	__cmpxchg(ptr, old, new, sizeof(*(ptr)))
```

### Process & Task Struck & Signal

`wait_event_interruptible` is defined in `linux/wait.h`, `if (!(condition))`, put the process in the waiting queue, in this case, the condition is `!atomic_read(&already_open)`, which means that once the process is not able to read the file (`atomic_read(&already_open)` returns 1), it will be put into the wait queue. 

```c
#define ___wait_is_interruptible(state)						\
	(!__builtin_constant_p(state) ||					\
	 (state & (TASK_INTERRUPTIBLE | TASK_WAKEKILL)))
...
...
#define ___wait_event(wq_head, condition, state, exclusive, ret, cmd)		\
({										\
	__label__ __out;							\
	struct wait_queue_entry __wq_entry;					\
	long __ret = ret;	/* explicit shadow */				\
										\
	init_wait_entry(&__wq_entry, exclusive ? WQ_FLAG_EXCLUSIVE : 0);	\
	for (;;) {								\
		long __int = prepare_to_wait_event(&wq_head, &__wq_entry, state);\
										\
		if (condition)							\
			break;							\
										\
		if (___wait_is_interruptible(state) && __int) {			\
			__ret = __int;						\
			goto __out;						\
		}								\
										\
		cmd;								\
	}									\
	finish_wait(&wq_head, &__wq_entry);					\
__out:	__ret;									\
})
...
...
#define __wait_event_interruptible(wq_head, condition)				\
	___wait_event(wq_head, condition, TASK_INTERRUPTIBLE, 0, 0,		\
		      schedule())
...
...
#define wait_event_interruptible(wq_head, condition)				\
({										\
	int __ret = 0;								\
	might_sleep();								\
	if (!(condition))							\
		__ret = __wait_event_interruptible(wq_head, condition);		\
	__ret;									\
})
```


For the following:

```c
/* If we woke up because we got a signal we're not blocking,
* return -EINTR (fail the system call).  This allows processes
* to be killed or stopped.
*/
is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];
```

`current` is defined in `include/asm/current.h`:

```c
static __always_inline __attribute_const__ struct task_struct *get_current(void)
{
	struct task_struct *cur;
...
...
}
...
#define current get_current()
```

`task_struct` is defined in `linux/sched.h`:

```c
struct task_struct {
#ifdef CONFIG_THREAD_INFO_IN_TASK
	/*
	 * For reasons of header soup (see current_thread_info()), this
	 * must be the first element of task_struct.
	 */
	struct thread_info		thread_info;
#endif
	unsigned int			__state;

	/* saved state for "spinlock sleepers" */
	unsigned int			saved_state;

	/*
	 * This begins the randomizable portion of task_struct. Only
	 * scheduling-critical items should be added above here.
	 */
	randomized_struct_fields_start

	void				*stack;
	refcount_t			usage;
	/* Per task flags (PF_*), defined further below: */
	unsigned int			flags;
...
...
  struct sigpending		pending;
...
...
  /* Signal handlers: */
	struct signal_struct		*signal;
	struct sighand_struct __rcu		*sighand;
	sigset_t			blocked;
	sigset_t			real_blocked;
	/* Restored if set_restore_sigmask() was used: */
	sigset_t			saved_sigmask;
...
}
```

`sigpending` is defined in `linux/signal_types.h`:

```c
struct sigpending {
	struct list_head list;
	sigset_t signal;
};
```

`sigset_t` is defined in `linux/signal.h`:

```c
typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;
```




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
7. clean up: `make clean`
