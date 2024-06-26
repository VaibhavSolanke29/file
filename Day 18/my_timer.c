#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
 
#define SYSFS_DIR "dynamic_timer"
#define SYSFS_FILE "timer_interval"

void my_work_function(struct work_struct *work);
 
static struct timer_list my_timer;
static struct tasklet_struct my_tasklet;
static struct workqueue_struct *my_workqueue;
static DECLARE_WORK(my_work, my_work_function);
static unsigned long timer_interval = 500; // Default interval in ms

void my_timer_callback(struct timer_list *timer)
{
    printk(KERN_INFO "Timer expired, rescheduling for another %lu ms.\n", timer_interval);
    tasklet_schedule(&my_tasklet);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));
}

void my_tasklet_function(unsigned long data)
{
    printk(KERN_INFO "Tasklet function executed with data: %lu\n", data);
    queue_work(my_workqueue, &my_work);
}

void my_work_function(struct work_struct *work)
{
    printk(KERN_INFO "Work function executed.\n");
}

static ssize_t timer_interval_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%lu\n", timer_interval);
}

static ssize_t timer_interval_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    unsigned long new_interval;
    if (kstrtoul(buf, 10, &new_interval))
        return -EINVAL;
    timer_interval = new_interval;
    return count;
}
 
static struct kobj_attribute timer_interval_attr = __ATTR(timer_interval, 0664, timer_interval_show, timer_interval_store);
 
static struct attribute *attrs[] = {
&timer_interval_attr.attr,
    NULL,
};
 
static struct attribute_group attr_group = {
    .name = SYSFS_DIR,
    .attrs = attrs,
};
 
static struct kobject *kobj;

static int __init my_module_init(void)
{
    int retval;
 
    printk(KERN_INFO "Initializing the comprehensive timer module.\n");

    timer_setup(&my_timer, my_timer_callback, 0);
    my_timer.expires = jiffies + msecs_to_jiffies(timer_interval);
    add_timer(&my_timer);

    tasklet_init(&my_tasklet, my_tasklet_function, 42);

    my_workqueue = create_workqueue("my_workqueue");
 
    if (!my_workqueue) {
        printk(KERN_ERR "Failed to create workqueue.\n");
        del_timer(&my_timer);
        return -ENOMEM;
    }

    kobj = kobject_create_and_add("dynamic_timer", kernel_kobj);
    if (!kobj) {
        printk(KERN_ERR "Failed to create sysfs kobject.\n");
        destroy_workqueue(my_workqueue);
        del_timer(&my_timer);
        return -ENOMEM;
    }
 
    retval = sysfs_create_group(kobj, &attr_group);
    if (retval) {
        kobject_put(kobj);
        destroy_workqueue(my_workqueue);
        del_timer(&my_timer);
        return retval;
    }
 
    printk(KERN_INFO "Timer added to expire in %lu ms.\n", timer_interval);
 
    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Exiting the comprehensive timer module.\n");

    del_timer(&my_timer);

    tasklet_kill(&my_tasklet);

    if (my_workqueue)
        destroy_workqueue(my_workqueue);

    kobject_put(kobj);
}

module_init(my_module_init);
module_exit(my_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A comprehensive timer, tasklet, and workqueue example module");
