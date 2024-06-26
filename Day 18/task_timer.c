#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define SYSFS_DIR "periodic_timer"
#define SYSFS_FILE "control"

void my_work_function(struct work_struct *work);

static struct timer_list my_timer;
static struct tasklet_struct my_tasklet;
static struct workqueue_struct *my_workqueue;
static DECLARE_WORK(my_work, my_work_function);
static unsigned long timer_interval = 500; // Default interval in ms
static bool timer_running = false;

// Forward declaration of functions
void my_work_function(struct work_struct *work);
void my_timer_callback(struct timer_list *timer);
void my_tasklet_function(unsigned long data);

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

static ssize_t control_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", timer_running ? "running" : "stopped");
}

static ssize_t control_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    if (strncmp(buf, "start", count - 1) == 0) {
        if (!timer_running) {
            printk(KERN_INFO "Starting timer.\n");
            mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));
            timer_running = true;
        }
    } else if (strncmp(buf, "stop", count - 1) == 0) {
        if (timer_running) {
            printk(KERN_INFO "Stopping timer.\n");
            del_timer(&my_timer);
            timer_running = false;
        }
    } else {
        return -EINVAL;
    }
    return count;
}

static struct kobj_attribute control_attr = __ATTR(control, 0664, control_show, control_store);

static struct attribute *attrs[] = {
    &control_attr.attr,
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

    printk(KERN_INFO "Initializing the periodic timer module.\n");

    timer_setup(&my_timer, my_timer_callback, 0);
    tasklet_init(&my_tasklet, my_tasklet_function, 42);

    my_workqueue = create_workqueue("my_workqueue");
    if (!my_workqueue) {
        printk(KERN_ERR "Failed to create workqueue.\n");
        return -ENOMEM;
    }

    kobj = kobject_create_and_add("periodic_timer", kernel_kobj);
    if (!kobj) {
        printk(KERN_ERR "Failed to create sysfs kobject.\n");
        destroy_workqueue(my_workqueue);
        return -ENOMEM;
    }

    retval = sysfs_create_group(kobj, &attr_group);
    if (retval) {
        kobject_put(kobj);
        destroy_workqueue(my_workqueue);
        return retval;
    }

    printk(KERN_INFO "Periodic timer module initialized.\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Exiting the periodic timer module.\n");

    if (timer_running)
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
MODULE_DESCRIPTION("A module with a periodic timer, tasklet, and workqueue example with sysfs control");
