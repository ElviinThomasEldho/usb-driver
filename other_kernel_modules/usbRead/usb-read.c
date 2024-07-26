#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

//probe function
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "[*] SanDisk Corp. Cruzer Blade (%04X : %04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}

//disconnect
static void pen_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "[*] SanDisk Corp. Cruzer Blade removed");
}

//usb_device_id
static struct usb_device_id pen_table[] = {
    {USB_DEVICE(0x0781,0x5567)},
    {}
};
MODULE_DEVICE_TABLE(usb, pen_table);

//usb_driver
static struct usb_driver pen_driver = {
    .name = "SanDisk Corp. Cruzer Blade",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

static int __init pen_init(void) {
    int ret = -1;
    printk(KERN_INFO "\tRegistering Driver with kernel");
    printk(KERN_INFO "\tRegistering Driver with Kernel");
    ret = usb_register(&pen_driver);
    printk(KERN_INFO "\tRegistration is complete");
    return ret;
}

static void __exit pen_exit(void) {

}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldho");
MODULE_DESCRIPTION("USB Pen Regirstration Driver");