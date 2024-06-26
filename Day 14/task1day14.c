#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>

#define RX_RING_SIZE 16
#define TX_RING_SIZE 16

#define MMIO_BASE 0xDEADBBEEF
#define TX_DESC_BASE 0x000
#define DESC_SIZE 16

struct priv {
    struct net_device *dev;
    struct sk_buff *tx_skb[TX_RING_SIZE];
    dma_addr_t tx_dma[TX_RING_SIZE];
    int tx_next;
    void __iomem *mmio_base;
};

static int my_open(struct net_device *dev);
static int my_stop(struct net_device *dev);
static netdev_tx_t my_start_xmit(struct sk_buff *skb, struct net_device *dev);

static const struct net_device_ops netdev_ops = {
    .ndo_open = my_open,
    .ndo_stop = my_stop,
    .ndo_start_xmit = my_start_xmit,
};
 static int my_open(struct net_device *dev) {
    netif_start_queue(dev);
    return 0;
}

static int my_stop(struct net_device *dev) {
    netif_stop_queue(dev);
    return 0;
}

static netdev_tx_t my_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    struct priv *priv = netdev_priv(dev);
    int tx_next = priv->tx_next;

    dma_addr_t dma_addr = dma_map_single(&dev->dev, skb->data, skb->len, DMA_TO_DEVICE);
    if (dma_mapping_error(&dev->dev, dma_addr)) {
        dev_kfree_skb(skb);
        return NETDEV_TX_BUSY;
    }

    priv->tx_skb[tx_next] = skb;
    priv->tx_dma[tx_next] = dma_addr;
    
    writel(dma_addr, priv->mmio_base + TX_DESC_BASE + tx_next *DESC_SIZE);
    
    mdelay(10);
    dma_unmap_single(&dev->dev, dma_addr, skb->len, DMA_TO_DEVICE);
    priv->tx_skb[tx_next] = NULL;
    

    priv->tx_next = (tx_next + 1) % TX_RING_SIZE;

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static void my_setup(struct net_device *dev) {
    ether_setup(dev);
    dev->netdev_ops = &netdev_ops;
}

static int __init my_init(void) {
    struct net_device *ndev;
    struct priv *priv;

    ndev = alloc_etherdev(sizeof(struct priv));
    if (!ndev)
        return -ENOMEM;

    my_setup(ndev);
    priv = netdev_priv(ndev);
    priv->dev = ndev;
    
    priv->mmio_base = ioremap(MMIO_BASE, 0x100);

     if (!priv->mmio_base) {
        free_netdev(ndev);
        return -ENOMEM;
    }
    
    if (register_netdev (ndev)){
	iounmap(priv->mmio_base);
	free_netdev(ndev);
	return -ENODEV;
	}
    return 0;
}

static void __exit my_exit(void) {
    struct priv *priv;
    struct net_device *ndev;
    
    ndev=dev_get_by_name(&init_net, "eth0");

    if(ndev){
    priv = netdev_priv(ndev);
    unregister_netdev(ndev);
    free_netdev(ndev);
    iounmap(priv->mmio_base);
    }
    
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV");
MODULE_DESCRIPTION("Simple Network Driver Example");
