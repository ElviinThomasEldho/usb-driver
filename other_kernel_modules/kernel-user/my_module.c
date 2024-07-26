#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/device.h>  // For device creation and management
#include <linux/kdev_t.h>  // For MKDEV macro

#define DEVICE_NAME "my_device"
#define CLASS_NAME "my_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Author");
MODULE_DESCRIPTION("A simple Linux char driver");
MODULE_VERSION("0.1");

static int major_number;
static char message[256] = {0};
static short message_size;
static struct class* my_class = NULL;
static struct device* my_device = NULL;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release,
};

static int device_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Device successfully closed\n");
    return 0;
}

static ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    error_count = copy_to_user(buffer, message, message_size);

    if (error_count == 0) {
        printk(KERN_INFO "Sent %d characters to the user\n", message_size);
        return (message_size = 0);
    } else {
        printk(KERN_INFO "Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t device_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    sprintf(message, "%s(%zu letters)", buffer, len);
    message_size = strlen(message);
    printk(KERN_INFO "Received %zu characters from the user\n", len);
    return len;
}

static int __init my_module_init(void) {
    printk(KERN_INFO "Initializing the my_module\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered correctly with major number %d\n", major_number);

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(my_class);
    }
    printk(KERN_INFO "Device class registered correctly\n");

    my_device = device_create(my_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(my_device);
    }
    printk(KERN_INFO "Device class created correctly\n");

    return 0;
}

static void __exit my_module_exit(void) {
    device_destroy(my_class, MKDEV(major_number, 0));
    class_unregister(my_class);
    class_destroy(my_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Goodbye from the my_module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

