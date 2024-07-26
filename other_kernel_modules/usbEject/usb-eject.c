#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/kthread.h>
#include <linux/delay.h>

// Define the allowed vendor and product IDs
struct usb_id {
    uint16_t vendor;
    uint16_t product;
};

static const struct usb_id allowed_ids[] = {
    {0x04e8, 0x6860},  // Example allowed device
    //{0x0781, 0x5567}, 
    // Add more allowed IDs as needed
};

static struct notifier_block usb_notify;

static void usb_unbind_device(struct usb_device *dev) {
    // Unbind the device from the USB driver
    if (dev->dev.driver) {
        device_release_driver(&dev->dev);
        printk(KERN_INFO "USB device unbound: Vendor ID = %04x, Product ID = %04x\n",
               dev->descriptor.idVendor, dev->descriptor.idProduct);
    }
}

static int usb_notify_handler(struct notifier_block *nb, unsigned long action, void *data) {
    struct usb_device *dev = data;
    int allowed = 0;

    switch (action) {
        case USB_DEVICE_ADD:
            printk(KERN_INFO "USB device connected: Vendor ID = %04x, Product ID = %04x\n",
                   dev->descriptor.idVendor, dev->descriptor.idProduct);

            // Check if the connected device is in the allowed list
            for (int i = 0; i < ARRAY_SIZE(allowed_ids); ++i) {
                if (dev->descriptor.idVendor == allowed_ids[i].vendor &&
                    dev->descriptor.idProduct == allowed_ids[i].product) {
                    allowed = 1;
                    break;
                }
            }

            if (allowed) {
                printk(KERN_INFO "USB device access granted.\n");
            } else {
                printk(KERN_INFO "USB device access denied. Unauthorized device.\n");
                // Eject the unauthorized device
                //usb_reset_device(dev);
                //fsleep(5); // Sleep for a while to allow reset to complete
                usb_unbind_device(dev);
            }
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

