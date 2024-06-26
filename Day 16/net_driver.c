#include<linux/module.h>
#include<linux/netdevice.h>
#include<linux/etherdevice.h>

static struct net_device *net_dev;

static int my_open ( struct net_device *dev ){
	netif_start_queue(dev);
	pr_info ("Device opened ...\n");
	return 0;
	}

static int my_stop ( struct net_device *dev ){
	netif_stop_queue(dev);
	pr_info ("Device closed ...\n");
	return 0;
	}

static netdev_tx_t my_start_xmit(struct sk_buff *skb, struct net_device *dev){
	pr_info("Transmitting package ...\n");
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
	}
	
static struct net_device_ops netdev_ops={
	.ndo_open=my_open,
	.ndo_stop=my_stop,
	.ndo_start_xmit=my_start_xmit,
	};

static void my_setup(struct net_device *dev){
	ether_setup(dev);
	dev->netdev_ops=&netdev_ops;
	}
	
static int __init my_init(void){
	net_dev=alloc_etherdev(0);
	if (!net_dev){
		pr_err("Allocation failed...\n");
		return -ENOMEM;
		}
	strcpy(net_dev->name, "my_net%d");
	my_setup(net_dev);
	if (register_netdev(net_dev)){
		pr_err("Registration failed ...\n");
		free_netdev(net_dev);
		return -1;
		}
	pr_info("Network module laded successfully...\n");
	return 0;
	}
	
static void __exit my_exit(void){
	unregister_netdev (net_dev);
	free_netdev(net_dev);
	pr_info("Module unloaded ...\n");
	}
	
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("NETWORK DRIVER");

