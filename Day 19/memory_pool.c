#include <linux/slab.h>

struct my_struct {
    int data;
    char name[50];
};

struct kmem_cache *my_cache;

static int __init my_module_init(void) {
    my_cache = kmem_cache_create("my_cache", sizeof(struct my_struct), 0, SLAB_HWCACHE_ALIGN, NULL);
    if (!my_cache) {
        printk(KERN_ALERT "Failed to create memory cache\n");
        return -ENOMEM;
    }

    struct my_struct *obj = kmem_cache_alloc(my_cache, GFP_KERNEL);
    if (!obj) {
        printk(KERN_ALERT "Failed to allocate object from cache\n");
        kmem_cache_destroy(my_cache);
        return -ENOMEM;
    }

    obj->data = 123;
    snprintf(obj->name, 50, "Example");

    printk(KERN_INFO "Object created: data=%d, name=%s\n", obj->data, obj->name);

    kmem_cache_free(my_cache, obj);

    return 0;
}

static void __exit my_module_exit(void) {
    if (my_cache) {
        kmem_cache_destroy(my_cache);
        printk(KERN_INFO "Memory cache destroyed\n");
    }
}

module_init(my_module_init);
module_exit(my_module_exit);

