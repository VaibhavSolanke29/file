#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/dma-mapping.h>
#include<linux/slab.h>

#define DMA_BUFFER_SIZE 1024

static void *dma_buffer;
static dma_addr_t dma_handle;

static int __init dma_example_init(void){

struct device *dev=NULL;
printk(KERN_INFO "example init");

dma_buffer=dma_alloc_coherent(dev, DMA_BUFFER_SIZE, &dma_handle, GFP_KERNEL);
if (!dma_buffer){
	printk(KERN_ERR "Failed to allocate dma buffer\n");
	return -ENOMEM;
	}
	
printk(KERN_INFO "dma buffer allocated at %p, dma address %pad\n",dma_buffer, &dma_handle);

memset(dma_buffer, 0xAA, DMA_BUFFER_SIZE);
printk(KERN_INFO "Buffer initialized\n");

dma_sync_single_for_device(dev, dma_handle, DMA_BUFFER_SIZE, DMA_TO_DEVICE);

dma_sync_single_for_cpu(dev, dma_handle, DMA_BUFFER_SIZE, DMA_FROM_DEVICE);

printk(KERN_INFO "dma example first byte dma buffer 0x%02X\n", ((unsigned char *)dma_buffer)[0]);
return 0;
}

static void __exit dma_example_exit(void){
	struct device *dev=NULL;
	printk(KERN_INFO "dma existing");
	
	if (dma_buffer){
		dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_handle);
	}
	
	printk(KERN_INFO "dma buffer freed\n");
}

module_init(dma_example_init);
module_exit(dma_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("DMA EXAMPLE MODULE");

