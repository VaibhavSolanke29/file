#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h> // For kmalloc() and kfree()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A simple kernel module to demonstrate memory allocation.");
MODULE_VERSION("0.1");

static char *buffer;

static int __init my_module_init(void) {
    printk(KERN_INFO "my_module: Initializing the module\n");

    buffer = kmalloc(1024, GFP_KERNEL);
    if (!buffer) {
        printk(KERN_ALERT "my_module: kmalloc failed!\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "my_module: kmalloc succeeded\n");
    return 0;
}

static void __exit my_module_exit(void) {
    printk(KERN_INFO "my_module: Exiting the module\n");

    if (buffer) {
        kfree(buffer);
        printk(KERN_INFO "my_module: kfree succeeded\n");
    }
}

module_init(my_module_init);
module_exit(my_module_exit);
