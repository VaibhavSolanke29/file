#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/usb.h>
#include <linux/device.h>
 
static struct kobject *usb_kobj;
static int usb_status = 0;
static char usb_vendor[20] = "VirtualVendor";
static char usb_product[20] = "VirtualProduct";
 
static ssize_t usb_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", usb_status);
}
 
static ssize_t usb_status_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%du", &usb_status);
    return count;
}
 
static ssize_t usb_vendor_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", usb_vendor);
}
 
static ssize_t usb_vendor_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    snprintf(usb_vendor, sizeof(usb_vendor), "%s", buf);
    return count;
}
 
static ssize_t usb_product_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", usb_product);
}
 
static ssize_t usb_product_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    snprintf(usb_product, sizeof(usb_product), "%s", buf);
    return count;
}
 
static struct kobj_attribute usb_status_attr = __ATTR(usb_status, 0664, usb_status_show, usb_status_store);
static struct kobj_attribute usb_vendor_attr = __ATTR(usb_vendor, 0664, usb_vendor_show, usb_vendor_store);
static struct kobj_attribute usb_product_attr = __ATTR(usb_product, 0664, usb_product_show, usb_product_store);
 
static struct attribute *usb_attrs[] = {
&usb_status_attr.attr,
&usb_vendor_attr.attr,
&usb_product_attr.attr,
    NULL,
};
 
static struct attribute_group usb_attr_group = {
    .attrs = usb_attrs,
};
 
static int __init usb_module_init(void) {
    int error;
 
    usb_kobj = kobject_create_and_add("virtual_usb_device", kernel_kobj);
    if (!usb_kobj)
        return -ENOMEM;
 
    error = sysfs_create_group(usb_kobj, &usb_attr_group);
    if (error)
        kobject_put(usb_kobj);
 
    return error;
}
 
static void __exit usb_module_exit(void) {
    kobject_put(usb_kobj);
}
 
module_init(usb_module_init);
module_exit(usb_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("An advanced kernel module for managing a virtual USB device.");
