#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>

static int __init fixed_width_init(void) {
    u8 var8 = 255;
    u16 var16 = 65535;
    u32 var32 = 4294967295U;

    printk(KERN_INFO "u8 variable: %u\n", var8);
    printk(KERN_INFO "u16 variable: %u\n", var16);
    printk(KERN_INFO "u32 variable: %u\n", var32);

    return 0;
}

static void __exit fixed_width_exit(void) {
    printk(KERN_INFO "Fixed width integer module exit\n");
}

module_init(fixed_width_init);
module_exit(fixed_width_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A simple module to demonstrate fixed-width integer types");
