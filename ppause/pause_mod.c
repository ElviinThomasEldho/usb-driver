#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("A kernel module to pause a process for 5 seconds");
MODULE_VERSION("1.0");

static struct task_struct *task;
static int pid = -1;
module_param(pid, int, 0644);
MODULE_PARM_DESC(pid, "The PID of the process to pause");

static int pause_thread(void *data) {
    struct task_struct *task_to_pause;
    struct pid *pid_struct;

    if (pid == -1) {
        printk(KERN_INFO "PauseModule: No PID provided\n");
        return -EINVAL;
    }

    rcu_read_lock();
    pid_struct = find_vpid(pid);
    task_to_pause = pid_task(pid_struct, PIDTYPE_PID);
    rcu_read_unlock();

    if (!task_to_pause) {
        printk(KERN_INFO "PauseModule: Could not find task with PID %d\n", pid);
        return -ESRCH;
    }

    printk(KERN_INFO "PauseModule: Pausing process with PID %d\n", pid);

    // Suspend the task
    send_sig(SIGSTOP, task_to_pause, 0);

    // Sleep for 5 seconds
    ssleep(5);

    // Resume the task
    send_sig(SIGCONT, task_to_pause, 0);

    printk(KERN_INFO "PauseModule: Resuming process with PID %d\n", pid);

    return 0;
}

static int __init pause_module_init(void) {
    printk(KERN_INFO "PauseModule: Initializing module\n");

    task = kthread_run(pause_thread, NULL, "pause_thread");
    if (IS_ERR(task)) {
        printk(KERN_ALERT "PauseModule: Failed to create thread\n");
        return PTR_ERR(task);
    }

    return 0;
}

static void __exit pause_module_exit(void) {
    printk(KERN_INFO "PauseModule: Exiting module\n");
    if (task) {
        int ret = kthread_stop(task);
        if (ret == -EINTR) {
            printk(KERN_ALERT "PauseModule: Thread stop interrupted\n");
        } else if (ret == -EINVAL) {
            printk(KERN_ALERT "PauseModule: Invalid thread stop request\n");
        } else if (ret == -EAGAIN) {
            printk(KERN_ALERT "PauseModule: Thread was not stopped\n");
        } else {
            printk(KERN_INFO "PauseModule: Thread stopped successfully\n");
        }
    }
}

module_init(pause_module_init);
module_exit(pause_module_exit);

