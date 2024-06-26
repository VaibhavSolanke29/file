#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/uaccess.h>
 
static struct kobject *net_kobj;
static struct net_device *vnet_dev;
static int rx_packets = 0;
static int tx_packets = 0;
static char mac_address[18] = "00:11:22:33:44:55";
 
static ssize_t rx_packets_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", rx_packets);
}
 
static ssize_t tx_packets_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", tx_packets);
}
 
static ssize_t mac_address_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", mac_address);
}
 
static ssize_t mac_address_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    strncpy(mac_address, buf, sizeof(mac_address) - 1);
    return count;
}
 
static struct kobj_attribute rx_packets_attr = __ATTR(rx_packets, 0444, rx_packets_show, NULL);
static struct kobj_attribute tx_packets_attr = __ATTR(tx_packets, 0444, tx_packets_show, NULL);
static struct kobj_attribute mac_address_attr = __ATTR(mac_address, 0664, mac_address_show, mac_address_store);
 
static struct attribute *net_attrs[] = {
&rx_packets_attr.attr,
&tx_packets_attr.attr,
&mac_address_attr.attr,
    NULL,
};
 
static struct attribute_group net_attr_group = {
    .attrs = net_attrs,
};
 
static int vnet_open(struct net_device *dev) {
    netif_start_queue(dev);
    return 0;
}
 
static int vnet_stop(struct net_device *dev) {
    netif_stop_queue(dev);
    return 0;
}
 
static netdev_tx_t vnet_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    rx_packets++;
    tx_packets++;
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}
 
static struct net_device_ops vnet_ops = {
    .ndo_open = vnet_open,
    .ndo_stop = vnet_stop,
    .ndo_start_xmit = vnet_start_xmit,
};
 
static void vnet_setup(struct net_device *dev) {
    ether_setup(dev);
    dev->netdev_ops = &vnet_ops;
    memcpy(dev->dev_addr, mac_address, sizeof(mac_address) - 1);
}
 
static int __init net_module_init(void) {
    int error;
 
    net_kobj = kobject_create_and_add("virtual_net_device", kernel_kobj);
    if (!net_kobj)
        return -ENOMEM;
 
    error = sysfs_create_group(net_kobj, &net_attr_group);
    if (error) {
        kobject_put(net_kobj);
        return error;
    }
 
    vnet_dev = alloc_netdev(0, "vnet%d", NET_NAME_UNKNOWN, vnet_setup);
    if (register_netdev(vnet_dev)) {
        free_netdev(vnet_dev);
        kobject_put(net_kobj);
        return -ENOMEM;
    }
 
    return 0;
}
 
static void __exit net_module_exit(void) {
    unregister_netdev(vnet_dev);
    free_netdev(vnet_dev);
    kobject_put(net_kobj);
}
 
module_init(net_module_init);
module_exit(net_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("A kernel module for managing a virtual network interface device.");
