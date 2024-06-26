#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define IRQ_NUMBER 19   // This is an example; we can use a different IRQ number
#define GPIO_PIN 17    // Example GPIO pin for simulation

static irqreturn_t my_interrupt_handler(int irq, void *dev_id) {
    printk(KERN_INFO "Interrupt handled: IRQ %d\n", irq);
    return IRQ_HANDLED;
}

static int __init my_module_init(void) {
    int result;

    result = request_irq(IRQ_NUMBER, my_interrupt_handler, IRQF_SHARED, "my_interrupt", (void *)(my_interrupt_handler));
    if (result) {
        printk(KERN_ERR "Failed to request IRQ %d\n", IRQ_NUMBER);
        return result;
    }

    if (!gpio_is_valid(GPIO_PIN)) {
        printk(KERN_ERR "Invalid GPIO pin %d\n", GPIO_PIN);
        return -ENODEV;
    }

    gpio_request(GPIO_PIN, "sysfs");
    gpio_direction_input(GPIO_PIN);
    gpiod_export(GPIO_PIN, false);

    printk(KERN_INFO "Module loaded and interrupt handler registered.\n");
    return 0;
}

static void __exit my_module_exit(void) {
    free_irq(IRQ_NUMBER, (void *)(my_interrupt_handler));

    gpiod_unexport(GPIO_PIN);
    gpio_free(GPIO_PIN);

    printk(KERN_INFO "Module unloaded and interrupt handler unregistered.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A simple Linux kernel module to handle interrupts.");
MODULE_VERSION("1.0");

module_init(my_module_init);
module_exit(my_module_exit);

