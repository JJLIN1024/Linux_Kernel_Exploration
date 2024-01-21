/* 
 * kthreads.c 
 */ 
#include <linux/completion.h> 
#include <linux/err.h> /* for IS_ERR() */ 
#include <linux/init.h> 
#include <linux/kthread.h> 
#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/version.h> 
 
static struct completion k1_comp; 
static struct completion k2_comp; 
 
static int k1_threadfn(void *arg) 
{ 
    pr_info("Hello from kthread 1!\n"); 
 
    complete_all(&k1_comp); 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0) 
    kthread_complete_and_exit(&k1_comp, 0); 
#else 
    complete_and_exit(&k1_comp, 0); 
#endif 
} 
 
static int k2_threadfn(void *arg) 
{ 
    wait_for_completion(&k1_comp); 
 
    pr_info("Hello from kthread 2!\n");  
 
    complete_all(&k2_comp); 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0) 
    kthread_complete_and_exit(&k2_comp, 0); 
#else 
    complete_and_exit(&k2_comp, 0); 
#endif 
} 
 
static int __init completions_init(void) 
{ 
    struct task_struct *kthread1; 
    struct task_struct *kthread2; 
 
    pr_info("kernel threads completions example\n"); 
 
    init_completion(&k1_comp); 
    init_completion(&k2_comp); 
 
    kthread1 = kthread_create(k1_threadfn, NULL, "KThread One"); 
    if (IS_ERR(kthread1)) 
        goto ERROR_THREAD_1; 
 
    kthread2 = kthread_create(k2_threadfn, NULL, "KThread Two"); 
    if (IS_ERR(kthread2)) 
        goto ERROR_THREAD_2; 
 
    wake_up_process(kthread1); 
    wake_up_process(kthread2); 
 
    return 0; 
 
ERROR_THREAD_2: 
    kthread_stop(kthread2); 
ERROR_THREAD_1: 
    return -1; 
} 
 
static void __exit completions_exit(void) 
{ 
    wait_for_completion(&k1_comp); 
    wait_for_completion(&k2_comp); 
 
    pr_info("kthreads completions exit\n"); 
} 
 
module_init(completions_init); 
module_exit(completions_exit); 
 
MODULE_DESCRIPTION("Kernel Threads Completions Example"); 
MODULE_LICENSE("GPL");