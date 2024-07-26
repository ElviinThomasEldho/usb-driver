#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;
int usbConnected = 0;

#define USB_VENDOR_ID       (0x0781)      // USB device's vendor ID
#define USB_PRODUCT_ID      (0x5567)      // USB device's product ID

#define PRINT_USB_INTERFACE_DESCRIPTOR(i)                         \
    {                                                              \
        pr_info("USB_INTERFACE_DESCRIPTOR:\n");                   \
        pr_info("-----------------------------\n");               \
        pr_info("bLength: 0x%x\n", i.bLength);                    \
        pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);    \
        pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);  \
        pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);\
        pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);        \
        pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);    \
        pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);\
        pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);\
        pr_info("iInterface: 0x%x\n", i.iInterface);              \
        pr_info("\n");                                            \
    }

#define PRINT_USB_ENDPOINT_DESCRIPTOR(e)                          \
    {                                                              \
        pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                    \
        pr_info("------------------------\n");                    \
        pr_info("bLength: 0x%x\n", e.bLength);                    \
        pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);    \
        pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);  \
        pr_info("bmAttributes: 0x%x\n", e.bmAttributes);          \
        pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);      \
        pr_info("bInterval: 0x%x\n", e.bInterval);                \
        pr_info("\n");                                            \
    }

static void hello_nl_recv_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg;
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    
    
	nlh = (struct nlmsghdr *)skb->data;
	printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));
	pid = nlh->nlmsg_pid; /*pid of sending process */

	if(usbConnected == 0) {
    	msg = "USB not connected";
    	msg_size = strlen(msg);
	} else {
		msg = "USB connected";
		msg_size = strlen(msg);
	
	}
	
	skb_out = nlmsg_new(msg_size, 0);
	if (!skb_out) {
		printk(KERN_ERR "Failed to allocate new skb\n");
		return;
	}

	
	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	strncpy(nlmsg_data(nlh), msg, msg_size);

	res = nlmsg_unicast(nl_sk, skb_out, pid);
	if (res < 0)
		printk(KERN_INFO "Error while sending bak to user\n");
}

static int etx_usb_probe(struct usb_interface *interface,
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
    
    usbConnected = 1;

    return 0;
}

static void etx_usb_disconnect(struct usb_interface *interface)
{
    dev_info(&interface->dev, "USB Driver Disconnected\n");
    usbConnected = 0;
}

const struct usb_device_id etx_usb_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
    {}
};

MODULE_DEVICE_TABLE(usb, etx_usb_table);

static struct usb_driver etx_usb_driver = {
    .name       = "Genrobotics USB Driver",
    .probe      = etx_usb_probe,
    .disconnect = etx_usb_disconnect,
    .id_table   = etx_usb_table,
};

static int __init etx_usb_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        pr_err("Failed to create Netlink socket\n");
        return -ENOMEM;
    }

    return usb_register(&etx_usb_driver);
}

static void __exit etx_usb_exit(void)
{
    if (nl_sk) {
        netlink_kernel_release(nl_sk);
        nl_sk = NULL;
    }

    usb_deregister(&etx_usb_driver);
}

module_init(etx_usb_init);
module_exit(etx_usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldhho");
MODULE_DESCRIPTION("USB Driver to trigger an application");
MODULE_VERSION("1.30");

