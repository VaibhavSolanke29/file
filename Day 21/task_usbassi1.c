#include <linux/module.h>
#include <linux/usb.h>
#include <linux/scatterlist.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#define USB_STORAGE_VENDOR_ID  0x1234  
#define USB_STORAGE_PRODUCT_ID 0x5678  
static void read_descriptors(struct usb_device *dev); 
static int usb_storage_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void usb_storage_disconnect(struct usb_interface *interface);
static struct usb_device_id usb_storage_table[] = {
    { USB_DEVICE(USB_STORAGE_VENDOR_ID, USB_STORAGE_PRODUCT_ID) },
    {}
};
MODULE_DEVICE_TABLE(usb, usb_storage_table);
static struct usb_driver usb_storage_driver = {
    .name = "usb_storage_driver",
    .id_table = usb_storage_table,
    .probe = usb_storage_probe,
    .disconnect = usb_storage_disconnect,
};
static int usb_storage_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_device *dev = interface_to_usbdev(interface);
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;
    iface_desc = interface->cur_altsetting;
    printk(KERN_INFO "USB storage device (0x%04X:0x%04X) plugged\n", id->idVendor, id->idProduct);
    printk(KERN_INFO "Number of endpoints: %d\n", iface_desc->desc.bNumEndpoints);
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;
        printk(KERN_INFO "Endpoint[%d] address: 0x%02X\n", i, endpoint->bEndpointAddress);
    }
    read_descriptors(dev);
    return 0;
}
static void usb_storage_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "USB storage device removed\n");
}
static void read_descriptors(struct usb_device *dev) {
    unsigned char buffer[64];
    int ret;
    ret = usb_get_descriptor(dev, USB_DT_DEVICE, 0, buffer, sizeof(buffer));
    if (ret < 0) {
        printk(KERN_ERR "Failed to read device descriptor\n");
        return;
    }
    struct usb_device_descriptor *desc = (struct usb_device_descriptor *)buffer;
    printk(KERN_INFO "Device Descriptor:\n");
    printk(KERN_INFO "  bLength: %u\n", desc->bLength);
    printk(KERN_INFO "  bDescriptorType: %u\n", desc->bDescriptorType);
    printk(KERN_INFO "  bcdUSB: %x\n", desc->bcdUSB);
    printk(KERN_INFO "  idVendor: %x\n", desc->idVendor);
    printk(KERN_INFO "  idProduct: %x\n", desc->idProduct);
}
static int usb_storage_read(struct usb_device *dev, unsigned char *buffer, int length) {
    int retval;
    int actual_length;
    retval = usb_bulk_msg(dev, usb_rcvbulkpipe(dev, 1), buffer, length, &actual_length, 5000);
    if (retval)
        printk(KERN_ERR "USB bulk read error: %d\n", retval);
    return retval;
}
static int usb_storage_write(struct usb_device *dev, unsigned char *buffer, int length) {
    int retval;
    int actual_length;
    retval = usb_bulk_msg(dev, usb_sndbulkpipe(dev, 1), buffer, length, &actual_length, 5000);
    if (retval)
        printk(KERN_ERR "USB bulk write error: %d\n", retval);
    return retval;
}
static int __init usb_storage_init(void) {
    int result;
    result = usb_register(&usb_storage_driver);
    if (result)
        printk(KERN_ERR "usb_register failed. Error number %d", result);
    return result;
}
static void __exit usb_storage_exit(void) {
    usb_deregister(&usb_storage_driver);
}
module_init(usb_storage_init);
module_exit(usb_storage_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("USB Storage Driver");
