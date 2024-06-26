#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/atomic.h>
 
struct my_node {
    struct list_head list;
    atomic_t counter;
};
 
static LIST_HEAD(my_list);
 
static int __init list_atomic_init(void) {
    struct my_node *node;
    int i;
 
    for (i = 0; i < 5; i++) {
        node = kmalloc(sizeof(*node), GFP_KERNEL);
        if (!node) {
            printk(KERN_ERR "Failed to allocate memory for node\n");
            return -ENOMEM;
        }
        atomic_set(&node->counter, i);
        list_add(&node->list, &my_list);
    }
 
    list_for_each_entry(node, &my_list, list) {
        atomic_inc(&node->counter);
        printk(KERN_INFO "Counter value: %d\n", atomic_read(&node->counter));
    }
 
    return 0;
}
 
static void __exit list_atomic_exit(void) {
    struct my_node *node, *tmp;
 
    list_for_each_entry_safe(node, tmp, &my_list, list) {
        list_del(&node->list);
        kfree(node);
    }
 
    printk(KERN_INFO "List and atomic operations module exit\n");
}
 
module_init(list_atomic_init);
module_exit(list_atomic_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A module to demonstrate list and atomic operations");
