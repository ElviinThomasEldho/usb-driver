#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/usb.h>
#include <linux/usbdevice_fs.h> // For USB device operations

#define NETLINK_USER 31
#define USB_VENDOR_ID 0x1234 // Replace with your USB vendor ID
#define USB_PRODUCT_ID 0x5678 // Replace with your USB product ID

struct sock *nl_sk = NULL;

static bool usb_device_present(void)
{
    struct usb_device *dev;
    struct usb_device_id id_table[] = {
        { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
        { } /* Terminating entry */
    };

    dev = usb_find_device(id_table);
    if (!dev)
        return false;
    usb_put_dev(dev);
    return true;
}

static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    msg_size = strlen(msg);

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */

    // Check if USB device is present
    if (!usb_device_present()) {
        printk(KERN_INFO "USB device not present, skipping message send\n");
        return;
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
        printk(KERN_INFO "Error while sending back to user\n");
}

static int __init hello_init(void)
{
    printk("Entering: %s\n", __FUNCTION__);
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

