#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static struct timer_list my_timer;
static unsigned int timer_interval = 200; // Default interval in ms
static struct kobject *timer_kobj;

void my_timer_callback(struct timer_list *timer)
{
    printk(KERN_INFO "Timer expired, rescheduling for %u ms\n", timer_interval);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));
}

static ssize_t timer_interval_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", timer_interval);
}

static ssize_t timer_interval_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%u", &timer_interval);
    return count;
}

static struct kobj_attribute timer_interval_attr = __ATTR(timer_interval, 0660, timer_interval_show, timer_interval_store);

static int __init my_module_init(void)
{
    int retval;

    printk(KERN_INFO "Initializing module\n");

    timer_kobj = kobject_create_and_add("timer", kernel_kobj);
    if (!timer_kobj)
        return -ENOMEM;

    retval = sysfs_create_file(timer_kobj, &timer_interval_attr.attr);
    if (retval)
        kobject_put(timer_kobj);

    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));

    return retval;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Exiting module\n");
    del_timer(&my_timer);
    kobject_put(timer_kobj);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A dynamic timer interval example");
