#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *my_net_dev;

static int my_open(struct net_device *dev){
	netif_start_queue(dev);
	return 0;
}

static int my_stop(struct net_device *dev){
	netif_stop_queue(dev);
	return 0;
}

static netdev_tx_t my_start_xmit(struct sk_buff *skb, struct net_device *dev){
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}

static const struct net_device_ops my_netdev_ops={
	.ndo_open=my_open,
	.ndo_stop=my_stop,
	.ndo_start_xmit=my_start_xmit,
};

static void my_setup(struct net_device *dev){
	dev->netdev_ops=&my_netdev_ops;
	ether_setup(dev);
}

static int __init my_init(void){
	my_net_dev=alloc_netdev(0,"mynet%d",NET_NAME_UNKNOWN, my_setup);
	if(register_netdev(my_net_dev)){
		printk(KERN_ALERT "Failed to register net device\n");
		free_netdev(my_net_dev);
		return -1;
		}
		printk(KERN_INFO "NETWORK DRIVER LOADED SUCCESSFULLY\n");
		return 0;
}

static void __exit my_exit(void){
	unregister_netdev(my_net_dev);
	free_netdev(my_net_dev);
	printk(KERN_INFO "NETWORK DRIVER UNLOADED\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("SIMPLE NETWORK DEVICE");
	
