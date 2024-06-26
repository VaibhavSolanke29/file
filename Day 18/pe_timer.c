#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;

void my_timer_callback(struct timer_list *timer)
{
    printk(KERN_INFO "Timer expired, rescheduling for another 500ms.\n");

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
}

static int __init my_module_init(void)
{
    printk(KERN_INFO "Initializing the periodic timer module.\n");

    timer_setup(&my_timer, my_timer_callback, 0);

    my_timer.expires = jiffies + msecs_to_jiffies(500);
    add_timer(&my_timer);

    printk(KERN_INFO "Periodic timer added to expire in 500ms.\n");

    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Exiting the periodic timer module.\n");

    del_timer(&my_timer);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A periodic timer example module");
