#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/init.h>


#define IS_NEW_METHOD_USED  ( 1 )

#define USB_VENDOR_ID       ( 0x04e8 )      // USB device's vendor ID
#define USB_PRODUCT_ID      ( 0x6860 )      // USB device's product ID
#define READ_BUFFER_SIZE    1024

#define PRINT_USB_INTERFACE_DESCRIPTOR( i )                         \
{                                                                   \
    pr_info("USB_INTERFACE_DESCRIPTOR:\n");                         \
    pr_info("-----------------------------\n");                     \
    pr_info("bLength: 0x%x\n", i.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);          \
    pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);        \
    pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);      \
    pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);              \
    pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);          \
    pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);    \
    pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);    \
    pr_info("iInterface: 0x%x\n", i.iInterface);                    \
    pr_info("\n");                                                  \
}

#define PRINT_USB_ENDPOINT_DESCRIPTOR( e )                          \
{                                                                   \
    pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                          \
    pr_info("------------------------\n");                          \
    pr_info("bLength: 0x%x\n", e.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);          \
    pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);        \
    pr_info("bmAttributes: 0x%x\n", e.bmAttributes);                \
    pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);            \
    pr_info("bInterval: 0x%x\n", e.bInterval);                      \
    pr_info("\n");                                                  \
}


static mode_t original_mode;
static int mode_saved = 0;

static const char *directory_path = "/home/elviin/Desktop/test";


static int remove_directory_permissions(const char *path)
{
    struct path dir_path;
    struct inode *inode;
    int error = 0;

    // Resolve the path to obtain the directory's inode
    error = kern_path(path, LOOKUP_FOLLOW, &dir_path);
    if (error) {
        printk(KERN_ERR "Failed to resolve path: %s\n", path);
        return error;
    }

    inode = dir_path.dentry->d_inode;
    original_mode = inode->i_mode;
    mode_saved = 1;  // Flag to indicate that the original mode is saved

    // Remove read, write, and execute permissions for all
    inode->i_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO);

    // Mark the inode as dirty to write changes to disk
    mark_inode_dirty(inode);

    // Release the path
    path_put(&dir_path);

    printk(KERN_INFO "Removed permissions from directory: %s\n", path);

    return 0; // return success
}


static int add_directory_permissions(const char *path)
{
    struct path dir_path;
    struct inode *inode;
    int error = 0;

    // Ensure the original mode was saved before attempting to restore
    if (!mode_saved) {
        printk(KERN_ERR "Original mode not saved. Cannot restore permissions.\n");
        return -1;
    }

    // Resolve the path to obtain the directory's inode
    error = kern_path(directory_path, LOOKUP_FOLLOW, &dir_path);
    if (error) {
        printk(KERN_ERR "Failed to resolve path: %s\n", directory_path);
        return -1;
    }

    inode = dir_path.dentry->d_inode;
    inode->i_mode = original_mode;

    // Mark the inode as dirty to write changes to disk
    mark_inode_dirty(inode);

    // Release the path
    path_put(&dir_path);

    printk(KERN_INFO "Restored permissions for directory: %s\n", directory_path);
    return 0; // return success
}

static int usb_perm_probe(struct usb_interface *interface,
                        const struct usb_device_id *id)
{
    unsigned int i;
    unsigned int endpoints_count;
    struct usb_host_interface *iface_desc = interface->cur_altsetting;

    dev_info(&interface->dev, "USB Driver Probed: Vendor ID : 0x%02x,\t"
             "Product ID : 0x%02x\n", id->idVendor, id->idProduct);

    endpoints_count = iface_desc->desc.bNumEndpoints;
    PRINT_USB_INTERFACE_DESCRIPTOR(iface_desc->desc);

    for (i = 0; i < endpoints_count; i++) {
        PRINT_USB_ENDPOINT_DESCRIPTOR(iface_desc->endpoint[i].desc);
    }

    add_directory_permissions(directory_path);


    return 0;
}

static void usb_perm_disconnect(struct usb_interface *interface)
{

    remove_directory_permissions(directory_path);

    dev_info(&interface->dev, "USB Driver Disconnected\n");
}

const struct usb_device_id usb_perm_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) }, 
    {} /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_perm_table);

static struct usb_driver usb_perm_driver = {
    .name       = "USB Permissions Driver",
    .probe      = usb_perm_probe,
    .disconnect = usb_perm_disconnect,
    .id_table   = usb_perm_table,
};

#if (IS_NEW_METHOD_USED == 0)
module_usb_driver(usb_perm_driver);

#else
static int __init usb_perm_init(void)
{
    remove_directory_permissions(directory_path);

    // Register the USB device
    return usb_register(&usb_perm_driver);
}

static void __exit usb_perm_exit(void)
{
    add_directory_permissions(directory_path);

    // Deregister the USB device
    usb_deregister(&usb_perm_driver);
}

module_init(usb_perm_init);
module_exit(usb_perm_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldho");
MODULE_DESCRIPTION("Kernel module to manage directory permissions based on USB");
MODULE_VERSION("1.0");
