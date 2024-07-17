#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

static struct task_struct *task;
struct sock *nl_sk = NULL;
static int pid = -1;

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

    // Delay without sleep
    msleep(5000); // 5 seconds delay

    // Resume the task
    send_sig(SIGCONT, task_to_pause, 0);

    printk(KERN_INFO "PauseModule: Resuming process with PID %d\n", pid);

    return 0;
}

static void hello_nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Continue";
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    msg_size = strlen(msg);

    nlh = nlmsg_hdr(skb);
    if (!nlh) {
        printk(KERN_ERR "Netlink message header is NULL\n");
        return;
    }

    printk(KERN_INFO "Netlink received msg payload: %s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /* pid of sending process */

    skb_out = nlmsg_new(NLMSG_DEFAULT_SIZE, 0);
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    printk(KERN_INFO "PauseModule: Initializing module\n");
    task = kthread_run(pause_thread, NULL, "pause_thread");
    if (IS_ERR(task)) {
        printk(KERN_ALERT "PauseModule: Failed to create thread\n");
    }

    /* No need to sleep here 
    
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; 
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "Error while sending back to user\n");*/
}

static int __init hello_init(void) {
    printk("Entering: %s\n", __FUNCTION__);

    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -ENOMEM;
    }

    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "Exiting hello module\n");
    netlink_kernel_release(nl_sk);
    
    
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

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

