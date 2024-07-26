#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sysfs.h>

static int my_value = 0;

static ssize_t my_value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", my_value);
}

static ssize_t my_value_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%du", &my_value);
    return count;
}

static struct kobj_attribute my_attr = __ATTR(my_value, 0660, my_value_show, my_value_store);

static struct attribute *attrs[] = {
    &my_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *my_kobj;

static int __init my_module_init(void) {
    int retval;

    my_kobj = kobject_create_and_add("my_module", kernel_kobj);
    if (!my_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(my_kobj, &attr_group);
    if (retval)
        kobject_put(my_kobj);

    return retval;
}

static void __exit my_module_exit(void) {
    kobject_put(my_kobj);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example module with sysfs interface");

