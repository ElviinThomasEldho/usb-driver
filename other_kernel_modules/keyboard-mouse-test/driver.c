#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

#define USB_KEYBOARD_INTERFACE_CLASS 0x03
#define USB_KEYBOARD_INTERFACE_SUBCLASS 0x01
#define USB_KEYBOARD_INTERFACE_PROTOCOL 0x01

#define USB_MOUSE_INTERFACE_CLASS 0x03
#define USB_MOUSE_INTERFACE_SUBCLASS 0x01
#define USB_MOUSE_INTERFACE_PROTOCOL 0x02

static int usb_device_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(interface);
    struct usb_host_interface *iface_desc = interface->cur_altsetting;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    printk(KERN_INFO "USB device connected: Vendor ID : 0x%02x, Product ID : 0x%02x\n", id->idVendor, id->idProduct);

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;
        printk(KERN_INFO "Endpoint %d: bEndpointAddress 0x%02x, bmAttributes 0x%02x, wMaxPacketSize 0x%04x, bInterval %d\n",
               i, endpoint->bEndpointAddress, endpoint->bmAttributes, endpoint->wMaxPacketSize, endpoint->bInterval);
    }

    return 0;
}

static void usb_device_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "USB device disconnected\n");
}

static struct usb_device_id usb_device_id_table[] = {
    { USB_INTERFACE_INFO(USB_KEYBOARD_INTERFACE_CLASS, USB_KEYBOARD_INTERFACE_SUBCLASS, USB_KEYBOARD_INTERFACE_PROTOCOL) },
    { USB_INTERFACE_INFO(USB_MOUSE_INTERFACE_CLASS, USB_MOUSE_INTERFACE_SUBCLASS, USB_MOUSE_INTERFACE_PROTOCOL) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usb_device_id_table);

static struct usb_driver usb_device_driver = {
    .name = "usb_device_driver",
    .id_table = usb_device_id_table,
    .probe = usb_device_probe,
    .disconnect = usb_device_disconnect,
};

static int __init usb_device_init(void)
{
    int result;

    result = usb_register(&usb_device_driver);
    if (result)
        printk(KERN_ERR "usb_register failed. Error number %d\n", result);

    return result;
}

static void __exit usb_device_exit(void)
{
    usb_deregister(&usb_device_driver);
}

module_init(usb_device_init);
module_exit(usb_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldho");
MODULE_DESCRIPTION("Kernel module to detect USB keyboards and mice");
MODULE_VERSION("1.0");
