#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

static struct notifier_block usb_notify;

static int usb_notify_handler(struct notifier_block *nb, unsigned long action, void *data) {
    struct usb_device *dev = data;
    switch (action) {
        case USB_DEVICE_ADD:
            printk(KERN_INFO "USB device connected: Vendor ID = %04x, Product ID = %04x\n",
                   dev->descriptor.idVendor, dev->descriptor.idProduct);
            break;
        case USB_DEVICE_REMOVE:
            printk(KERN_INFO "USB device disconnected: Vendor ID = %04x, Product ID = %04x\n",
                   dev->descriptor.idVendor, dev->descriptor.idProduct);
            break;
    }
    return NOTIFY_OK;
}

static int __init usb_detect_init(void) {
    printk(KERN_INFO "USB Detect Module: Initialized\n");

    usb_notify.notifier_call = usb_notify_handler;

    /* Register a callback for USB device insertion and removal */
    usb_register_notify(&usb_notify);

    return 0;
}

static void __exit usb_detect_exit(void) {
    printk(KERN_INFO "USB Detect Module: Exiting\n");
    
    /* Unregister the USB device callbacks */
    usb_unregister_notify(&usb_notify);
}

module_init(usb_detect_init);
module_exit(usb_detect_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldho");
MODULE_DESCRIPTION("USB Device Detection Module");

