## Guide

This program is about "Completions - 'wait for completion' barrier APIs".

Read [kenrnel.org - completion.txt](https://www.kernel.org/doc/Documentation/scheduler/completion.txt) for better understanding.

In short, `threads.c` do the following:

1. use `init_completion` to init completion (`struct completion`) for the following two threads
1. use `kthread_create` to create two kernel threads (which are `struct task_struct`)
2. use `wake_up_process` to let these two threads start execution

In each threads's threadfn, use the following synchronization mechanism:
1. use `wait_for_completion` to instruct one thread to wait for another thread to complete
2. use `complete_all` to specify one thread has complete its execution
3. use `complete_and_exit` to exit

Expected output:

Since one thread will wait for another thread to complete its work, so the expected output (`pr_info`) should be in specific order.

### Introduction

If you have one or more threads that must wait for some kernel activity
to have reached a point or a specific state, completions can provide a
**race-free** solution to this problem. Semantically they are somewhat like a
`pthread_barrier()` and have similar use-cases.

Completions are a code synchronization mechanism which is preferable to any
misuse of locks/semaphores and busy-loops. Any time you think of using
`yield()` or some quirky `msleep(1)` loop to allow something else to proceed,
you probably want to look into using one of the `wait_for_completion*()`
calls and `complete()` instead.

The advantage of using completions is that they have a well defined, focused
purpose which makes it very easy to see the intent of the code, but they
also result in more efficient code as all threads can continue execution
until the result is actually needed, and both the waiting and the signalling
is highly efficient using low level scheduler sleep/wakeup facilities.

Completions are built on top of the waitqueue and wakeup infrastructure of
the Linux scheduler. The event the threads on the waitqueue are waiting for
is reduced to a simple flag in 'struct completion', appropriately called "done".

As completions are scheduling related, the code can be found in
`kernel/sched/completion.c`.

### Things to Keep In Mind when using `completion`

From [kenrnel.org - completion.txt](https://www.kernel.org/doc/Documentation/scheduler/completion.txt):

```console
To emphasise this again: in particular when using some of the waiting API variants
with more complex outcomes, such as the timeout or signalling (_timeout(),
_killable() and _interruptible()) variants, the wait might complete
prematurely while the object might still be in use by another thread - and a return
from the wait_on_completion*() caller function will deallocate the function
stack and cause subtle data corruption if a complete() is done in some
other thread. Simple testing might not trigger these kinds of races.

If unsure, use dynamically allocated completion objects, preferably embedded
in some other long lived object that has a boringly long life time which
exceeds the life time of any helper threads using the completion object,
or has a lock or other synchronization mechanism to make sure complete()
is not called on a freed object.
```

## Code Tracing

### Kernel Threads

`struct task_struct` is defined in `linux/sched.h`:

```c
struct task_struct {
#ifdef CONFIG_THREAD_INFO_IN_TASK
	/*
	 * For reasons of header soup (see current_thread_info()), this
	 * must be the first element of task_struct.
	 */
	struct thread_info		thread_info;
#endif
```

`kthread_create` is defined in `linux/kthread.h`:

```c
/**
 * kthread_create - create a kthread on the current node
 * @threadfn: the function to run in the thread
 * @data: data pointer for @threadfn()
 * @namefmt: printf-style format string for the thread name
 * @arg: arguments for @namefmt.
 *
 * This macro will create a kthread on the current node, leaving it in
 * the stopped state.  This is just a helper for kthread_create_on_node();
 * see the documentation there for more details.
 */
#define kthread_create(threadfn, data, namefmt, arg...) \
	kthread_create_on_node(threadfn, data, NUMA_NO_NODE, namefmt, ##arg)
```

`wake_up_process` is defined in `sched/core.c`:

```c
/**
 * try_to_wake_up - wake up a thread
 * @p: the thread to be awakened
 * @state: the mask of task states that can be woken
 * @wake_flags: wake modifier flags (WF_*)
 *
 * Conceptually does:
 *
 *   If (@state & @p->state) @p->state = TASK_RUNNING.
 *
 ...
 ...
/**
 * wake_up_process - Wake up a specific process
 * @p: The process to be woken up.
 *
 * Attempt to wake up the nominated process and move it to the set of runnable
 * processes.
 *
 * Return: 1 if the process was woken up, 0 if it was already running.
 *
 * This function executes a full memory barrier before accessing the task state.
 */
int wake_up_process(struct task_struct *p)
{
	return try_to_wake_up(p, TASK_NORMAL, 0);
}
```

### completion

`struct completion` is defined in `linux/completion.h`:

```c
/*
 * struct completion - structure used to maintain state for a "completion"
 *
 * This is the opaque structure used to maintain the state for a "completion".
 * Completions currently use a FIFO to queue threads that have to wait for
 * the "completion" event.
 *
 * See also:  complete(), wait_for_completion() (and friends _timeout,
 * _interruptible, _interruptible_timeout, and _killable), init_completion(),
 * reinit_completion(), and macros DECLARE_COMPLETION(),
 * DECLARE_COMPLETION_ONSTACK().
 */
struct completion {
	unsigned int done;
	struct swait_queue_head wait;
};
```

`swait_queue_head` is defined in `linux/swait.h`:

```c
struct swait_queue_head {
	raw_spinlock_t		lock;
	struct list_head	task_list;
};
```

### Error

`IS_ERR` is defined in `linux/err.h`:

```c
/*
 * Kernel pointers have redundant information, so we can use a
 * scheme where we can return either an error code or a normal
 * pointer with the same return value.
 *
 * This should be a per-architecture thing, to allow different
 * error and pointer decisions.
 */
#define MAX_ERRNO	4095
...
...
/**
/**
 * IS_ERR_VALUE - Detect an error pointer.
 * @x: The pointer to check.
 *
 * Like IS_ERR(), but does not generate a compiler warning if result is unused.
 */
#define IS_ERR_VALUE(x) unlikely((unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO)
...
...
 * IS_ERR - Detect an error pointer.
 * @ptr: The pointer to check.
 * Return: true if @ptr is an error pointer, false otherwise.
 */
static inline bool __must_check IS_ERR(__force const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}
```
 
`MAX_ERRNO` is hex is `0x00000FFF`, `-MAX_ERRNO` is `0xFFFFF001`, and `(unsigned long)-MAX_ERRNO` is equals to `0xFFFFF001`. So `(unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO` is checking whether a pointer is pointing to address bigger than `0xFFFFF001`.

For a 32-bit system，the end of kernel space address is `0xFFFF FFFF`，the last page of it is from `0xFFFFF000` to `0xFFFFFFFF`（a 4K page），and this page is reserved, so any pointer that points to the address within this page is invalid.


### The likely and unlikely macro

the `likely` and `unlikely` macro is defined in `linux/compiler.h`, which helps compiler do its optimization by utilizing cache's spacial locality.

```c
# define likely(x)	__builtin_expect(!!(x), 1)            
# define unlikely(x)	__builtin_expect(!!(x), 0)
```
> The `!!(x)` ensures `x` is converted to either `0` or `1`, since the original `x` passed in can be greater than zero.

The `likely()` and `unlikely()` macros serve as hints to the CPU to help it load the appropriate memory into the cache, optimizing memory access and improving overall performance.

A toy example:

```c
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

void foo();
void bar();

int main(int argc, char *argv[])
{   
    int a = 0;
    if(likely(argc == 1)) {
        foo();
    } else {
        bar();
    }
    return 0;
}
```

The corresponding assembly code when compiled with optimization(-O1 flag):

Obeserve that `main` calls `foo` (`call  foo`).
```assembly
main:
        sub     rsp, 8
        cmp     edi, 1
        jne     .L2
        mov     eax, 0
        call    foo
.L3:
        mov     eax, 0
        add     rsp, 8
        ret
.L2:
        mov     eax, 0
        call    bar
        jmp     .L3
```
When we changed `likely` to `unlikely`, here's what happens:

Obeserve that `main` calls `bar` (`call  bar`).
```assembly
main:
        sub     rsp, 8
        cmp     edi, 1
        je      .L5
        mov     eax, 0
        call    bar
.L3:
        mov     eax, 0
        add     rsp, 8
        ret
.L5:
        mov     eax, 0
        call    foo
        jmp     .L3
```


## Usage

1. `make`

```console
make -C /lib/modules/5.15.0-91-generic/build M=/home/jimmy/Linux_Driver_Practice/07_kthread modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-91-generic'
  CC [M]  /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.o
  MODPOST /home/jimmy/Linux_Driver_Practice/07_kthread/Module.symvers
  CC [M]  /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.mod.o
  LD [M]  /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.ko
  BTF [M] /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.ko
Skipping BTF generation for /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-91-generic'
```

2. check module info: `modinfo kthreads.ko`

```console
filename:       /home/jimmy/Linux_Driver_Practice/07_kthread/kthreads.ko
license:        GPL
description:    Kernel Threads Completions Example
srcversion:     AADB635B6B3F4E857151864
depends:        
name:           kthreads
vermagic:       5.15.0-91-generic SMP mod_unload modversions aarch64
```
3. insert module into the kernel: `sudo insmod threads.ko`

4. check if the module is successfully inserted: `sudo lsmod | grep threads`

```console
kthreads               16384  0
```

5. check kernel ring buffer: `sudo dmesg -k | tail -3`

```console
[10870.960797] kernel threads completions example
[10870.962362] Hello from kthread 1!
[10870.962468] Hello from kthread 2!
```

6. remove the module: `sudo rmmod kthreads` & `sudo dmesg -k | tail -4`

```console
[10870.960797] kernel threads completions example
[10870.962362] Hello from kthread 1!
[10870.962468] Hello from kthread 2!
[10924.760696] kthreads completions exit```

7. clean up: `make clean`
