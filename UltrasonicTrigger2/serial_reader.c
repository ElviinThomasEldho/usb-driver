#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/string.h>

#define SERIAL_PORT "/dev/ttyACM0"  // Example serial port
#define BUFFER_SIZE 256

static struct task_struct *serial_thread;
static struct file *filep = NULL;
static struct task_struct *task;
struct sock *nl_sk = NULL;
static int pid = 3029;

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

static int serial_reader_thread(void *data) {
    char buffer[BUFFER_SIZE];
    int ret;

    filep = filp_open(SERIAL_PORT, O_RDONLY, 0);
    if (IS_ERR(filep)) {
        printk(KERN_ERR "Failed to open serial port %s\n", SERIAL_PORT);
        return PTR_ERR(filep);
    }

    printk(KERN_INFO "Serial port %s opened\n", SERIAL_PORT);

    while (!kthread_should_stop()) {
        loff_t pos = 0;  // File position
        ret = kernel_read(filep, buffer, BUFFER_SIZE, &pos);
        if (ret < 0) {
            if (ret == -ENODEV) {
                printk(KERN_INFO "Serial device disconnected\n");
                break;
            }
            printk(KERN_ERR "Read error: %d\n", ret);
        } else {
            buffer[ret] = '\0'; // Null-terminate the string
            printk(KERN_INFO "Read %d bytes: %s\n", ret, buffer);
            
            if(strcmp(buffer, "Object Detected\n") == 0) {            
		    printk(KERN_INFO "PauseModule: Initializing module\n");
		    task = kthread_run(pause_thread, NULL, "pause_thread");
		    if (IS_ERR(task)) {
			printk(KERN_ALERT "PauseModule: Failed to create thread\n");
		    }               
            }                               
        }
        //msleep(100); // Sleep for 1 second
    }

    filp_close(filep, NULL);
    return 0;
}

static int __init serial_reader_init(void) {
    serial_thread = kthread_run(serial_reader_thread, NULL, "serial_reader_thread");
    if (IS_ERR(serial_thread)) {
        printk(KERN_ERR "Failed to create serial reader thread\n");
        return PTR_ERR(serial_thread);
    }
    return 0;
}

static void __exit serial_reader_exit(void) {
    if (serial_thread) {
        kthread_stop(serial_thread);
        printk(KERN_INFO "Serial reader thread stopped\n");
    }
}

module_init(serial_reader_init);
module_exit(serial_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module for reading from Arduino serial port until device is no longer available");

