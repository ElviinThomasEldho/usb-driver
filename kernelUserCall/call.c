#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>

static struct kobject *my_kobj;

static int __init my_module_init(void) {
    int ret;

    my_kobj = kobject_create_and_add("call", kernel_kobj);
    if (!my_kobj)
        return -ENOMEM;

    // Send uevent
    ret = kobject_uevent(my_kobj, KOBJ_ADD);
    if (ret)
        kobject_put(my_kobj);

    return ret;
}

static void __exit my_module_exit(void) {
    kobject_put(my_kobj);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example module with uevent");

