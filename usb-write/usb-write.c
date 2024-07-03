#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define ALLOWED_VENDOR_ID 0x1234
#define ALLOWED_PRODUCT_ID 0x5678

static struct usb_device_id usb_table[] = {
    { USB_DEVICE(ALLOWED_VENDOR_ID, ALLOWED_PRODUCT_ID) },
    { }
};
MODULE_DEVICE_TABLE(usb, usb_table);

struct usb_data {
    struct usb_device *udev;
};

static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_data *data;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    data = kmalloc(sizeof(struct usb_data), GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "Cannot allocate memory for usb_data\n");
        return -ENOMEM;
    }

    data->udev = interface_to_usbdev(interface);
    iface_desc = interface->cur_altsetting;

    printk(KERN_INFO "USB device (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    printk(KERN_INFO "Interface Number: %d\n", iface_desc->desc.bInterfaceNumber);
    printk(KERN_INFO "Number of Endpoints: %d\n", iface_desc->desc.bNumEndpoints);

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;
        printk(KERN_INFO "Endpoint Address: 0x%02X\n", endpoint->bEndpointAddress);
    }

    usb_set_intfdata(interface, data);
    return 0;
}

static void usb_disconnect(struct usb_interface *interface) {
    struct usb_data *data;

    data = usb_get_intfdata(interface);
    kfree(data);

    printk(KERN_INFO "USB device removed\n");
}

static struct usb_driver usb_driver = {
    .name = "EE50-047A",
    .id_table = usb_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};

static int __init usb_init(void) {
    int result;
    
    result = usb_register(&usb_driver);
    if (result < 0) {
        printk(KERN_ERR "usb_register failed. Error number %d\n", result);
        return result;
    }
    
    printk(KERN_INFO "USB driver registered\n");
    return 0;
}

static void __exit usb_exit(void) {
    usb_deregister(&usb_driver);
    printk(KERN_INFO "USB driver deregistered\n");
}

module_init(usb_init);
module_exit(usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("USB Mass Storage Device Driver");

