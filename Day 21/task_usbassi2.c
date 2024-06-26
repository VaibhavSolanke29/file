#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/device.h>
 
static struct kobject *example_kobj;
static int my_dev_id = 0;
 
static ssize_t my_dev_id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", my_dev_id);
}
 
static ssize_t my_dev_id_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%du", &my_dev_id);
    return count;
}
 
static struct kobj_attribute my_dev_id_attr = __ATTR(my_dev_id, 0664, my_dev_id_show, my_dev_id_store);
 
static int __init my_module_init(void) {
    int error = 0;
 
    example_kobj = kobject_create_and_add("my_device", kernel_kobj);
    if (!example_kobj)
        return -ENOMEM;
 
    error = sysfs_create_file(example_kobj, &my_dev_id_attr.attr);
    if (error) {
        kobject_put(example_kobj);
    }
 
    return error;
}
 
static void __exit my_module_exit(void) {
    kobject_put(example_kobj);
}
 
module_init(my_module_init);
module_exit(my_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A simple Linux kernel module for a virtual device.");
