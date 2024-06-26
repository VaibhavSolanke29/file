#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
 
struct my_data {
    int id;
    char name[20];
};
 
static struct kmem_cache *my_cache = NULL;
static int kmalloc_size = 1024;
module_param(kmalloc_size, int, 0644);
 
void* allocate_kmalloc_memory(void) {
    void *ptr = NULL;
    printk(KERN_INFO "Allocating %d bytes using kmalloc...\n", kmalloc_size);
    ptr = kmalloc(kmalloc_size, GFP_KERNEL);
    if (!ptr)
        printk(KERN_ERR "kmalloc allocation failed!\n");
    else
        printk(KERN_INFO "kmalloc allocation successful. Pointer: %p\n", ptr);
    return ptr;
}
 
void free_kmalloc_memory(void *ptr) {
    if (ptr) {
        kfree(ptr);
        printk(KERN_INFO "kmalloc memory freed. Pointer: %p\n", ptr);
    }
}
 
void* allocate_vmalloc_memory(unsigned long size) {
    void *ptr = NULL;
    printk(KERN_INFO "Allocating %lu bytes using vmalloc...\n", size);
    ptr = vmalloc(size);
    if (!ptr)
        printk(KERN_ERR "vmalloc allocation failed!\n");
    else
        printk(KERN_INFO "vmalloc allocation successful. Pointer: %p\n", ptr);
    return ptr;
}
 
void free_vmalloc_memory(void *ptr) {
    if (ptr) {
        vfree(ptr);
        printk(KERN_INFO "vmalloc memory freed. Pointer: %p\n", ptr);
    }
}
 
int create_memory_cache(void) {
    printk(KERN_INFO "Creating a memory cache for struct my_data...\n");
    my_cache = kmem_cache_create("my_cache", sizeof(struct my_data), 0,
                                 SLAB_HWCACHE_ALIGN, NULL);
    if (!my_cache) {
        printk(KERN_ERR "Failed to create memory cache!\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Memory cache 'my_cache' created successfully.\n");
    return 0;
}
 
void destroy_memory_cache(void) {
    if (my_cache) {
        kmem_cache_destroy(my_cache);
        printk(KERN_INFO "Memory cache 'my_cache' destroyed.\n");
    }
}
 
static int __init memory_mgmt_module_init(void) {
    void *kmalloc_ptr = NULL;
    void *vmalloc_ptr = NULL;
    printk(KERN_INFO "Initializing memory management module...\n");
    kmalloc_ptr = allocate_kmalloc_memory();
    if (kmalloc_ptr)
        free_kmalloc_memory(kmalloc_ptr);
    vmalloc_ptr = allocate_vmalloc_memory(4096);
    if (vmalloc_ptr)
        free_vmalloc_memory(vmalloc_ptr);
    if (create_memory_cache() == 0)
        destroy_memory_cache();
    return 0;
}
 
static void __exit memory_mgmt_module_exit(void) {
    printk(KERN_INFO "Cleaning up memory management module...\n");
    printk(KERN_INFO "Memory management module unloaded.\n");
}
 
module_init(memory_mgmt_module_init);
module_exit(memory_mgmt_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("Linux kernel module for memory management");

