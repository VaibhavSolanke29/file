#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>

#define DMA_BUFFER_SIZE 1024
#define SG_ENTRIES 2

static void *dma_buffer;
static dma_addr_t dma_handle;
static struct scatterlist sg[SG_ENTRIES];

static int __init dma_example_init(void)
{
    struct device *dev = NULL;
    dma_addr_t sg_dma_addr;
    int i;
    printk(KERN_INFO "DMA Example: Initializing\n");

    dma_buffer = dma_alloc_coherent(dev, DMA_BUFFER_SIZE, &dma_handle, GFP_KERNEL);
    if (!dma_buffer) {
        printk(KERN_ERR "DMA Example: Failed to allocate DMA buffer\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "DMA Example: DMA buffer allocated at %p, DMA address: %pad\n", dma_buffer, &dma_handle);

    dma_handle = dma_map_single(dev, dma_buffer, DMA_BUFFER_SIZE, DMA_TO_DEVICE);
    if (dma_mapping_error(dev, dma_handle)) {
        printk(KERN_ERR "DMA Example: Failed to map DMA buffer\n");
        dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_handle);
        return -ENOMEM;
    }
    printk(KERN_INFO "DMA Example: DMA buffer mapped\n");

    memset(dma_buffer, 0xBB, DMA_BUFFER_SIZE);
    dma_sync_single_for_device(dev, dma_handle, DMA_BUFFER_SIZE, DMA_TO_DEVICE);
    printk(KERN_INFO "DMA Example: DMA operation simulated\n");

    dma_unmap_single(dev, dma_handle, DMA_BUFFER_SIZE, DMA_TO_DEVICE);
    printk(KERN_INFO "DMA Example: DMA buffer unmapped\n");

    for (i = 0; i < SG_ENTRIES; i++) {
        sg_set_buf(&sg[i], dma_buffer + i * (DMA_BUFFER_SIZE / SG_ENTRIES), DMA_BUFFER_SIZE / SG_ENTRIES);
    }
    sg_dma_addr = dma_map_sg(dev, sg, SG_ENTRIES, DMA_TO_DEVICE);
    if (dma_mapping_error(dev, sg_dma_addr)) {
        printk(KERN_ERR "DMA Example: Failed to map scatter-gather list\n");
        dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_handle);
        return -ENOMEM;
    }
    printk(KERN_INFO "DMA Example: Scatter-gather list mapped\n");

    for (i = 0; i < SG_ENTRIES; i++) {
        dma_sync_single_for_device(dev, sg_dma_address(&sg[i]), sg_dma_len(&sg[i]), DMA_TO_DEVICE);
    }
    printk(KERN_INFO "DMA Example: DMA operation on scatter-gather list simulated\n");

    dma_unmap_sg(dev, sg, SG_ENTRIES, DMA_TO_DEVICE);
    printk(KERN_INFO "DMA Example: Scatter-gather list unmapped\n");

    dma_sync_single_for_cpu(dev, dma_handle, DMA_BUFFER_SIZE, DMA_FROM_DEVICE);
    printk(KERN_INFO "DMA Example: First byte of DMA buffer after sync: 0x%02X\n", ((unsigned char *)dma_buffer)[0]);
    return 0;
}

static void __exit dma_example_exit(void)
{
    struct device *dev = NULL;

    printk(KERN_INFO "DMA Example: Exiting\n");

    if (dma_buffer) {
        dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_handle);
    }
    printk(KERN_INFO "DMA Example: DMA buffer freed\n");
}

module_init(dma_example_init);
module_exit(dma_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("Advanced DMA Example Module");
