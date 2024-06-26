#include<linux/wait.h>
#include<linux/sched.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>


MODULE_LICENSE("GPL");
#define BUFFER_SIZE 1024

static char device_data[BUFFER_SIZE];

DECLARE_WAIT_QUEUE_HEAD(my_queue);
int data_available=0;
static int data_size=0;

ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
pr_info("waiting for data to be available\n");
wait_event_interruptible(my_queue, data_available=0);
pr_info("my read is available proceding to read \n");
if(count>data_size)
{
count=data_size;
}

if(copy_to_user(buf, device_data, count))
{
pr_err("failed to code to user");
return -EFAULT;
}
data_available=0;
pr_info("data read ... %zu bytes \n",count);
return count;
}
ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
pr_info("waiting data\n");
if(count>BUFFER_SIZE)
{
count=BUFFER_SIZE;
}
if(copy_from_user(device_data, buf, count))
{
pr_err("failed to write data from user");
return -EFAULT;
}
data_available=1;
wake_up_interruptible(&my_queue);
pr_info("data written %zu bytes\n",count);
return count;
}

struct file_operations my_fops = {
.read=my_read,
.write=my_write,
};

static int __init my_module_init(void)
{
int result=register_chrdev(0, "my_device", &my_fops);
if(result<0)
{
pr_alert("Could not register device");
return result;
}
pr_info ("module loaded with device with major number %d \n",result);
return 0;
}

static void __exit my_module_exit(void)
{
unregister_chrdev(0, "my_device");
pr_info("Module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
