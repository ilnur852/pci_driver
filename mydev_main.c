#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include "chardev.h"
#include "mydev_main.h"

static struct pci_device_id my_driver_id_table[] = {
	{ PCI_DEVICE(0x0104, 0x0001) }, //VID & PID of device
	//{ PCI_DEVICE(0x0fee, 0x0002) }, //VID & PID of device

	{0,}
};

MODULE_DEVICE_TABLE(pci, my_driver_id_table);


static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);
void release_device(struct pci_dev *pdev);

//device struct
static struct pci_driver my_driver = {
	.name = "my_pci_driver",
	.id_table = my_driver_id_table,
	.probe = my_driver_probe,
	.remove = my_driver_remove
};


//init function for kernel - entry point
static int __init mypci_driver_init(void)
{
	return pci_register_driver(&my_driver);
}

//exit func for kernel
static void __exit mypci_driver_exit(void)
{
	pci_unregister_driver(&my_driver);
	destroy_char_dev();
}

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent){
//Do there what we want
	int bar; int err;
	struct my_driver_priv *drv;
	unsigned int mmio_start, mmio_len;
	bar = pci_select_bars(pdev, IORESOURCE_IO);
	err = pci_enable_device_mem(pdev);
	
	if (err) {
		return err; //i.e 1 
	}

	mmio_start  = pci_resource_start(pdev, 0);
	mmio_len = pci_resource_len(pdev, 0);
	printk("Device BAR starts @ Ox%X, and its length is 0x%X\n", mmio_start, mmio_len);
	
	drv = kzalloc(sizeof(struct my_driver_priv), GFP_KERNEL);
	

	if (!drv) {
			printk("kzalloc error");
			release_device(pdev);
			return -ENOMEM;
		}
	

	drv->hwmem = ioremap(mmio_start , mmio_len);
	
	if (!drv->hwmem) {
		printk("error during ioremap");
		release_device(pdev);
		return -EIO;
	}
	else {printk("memory successfully allocated to 0x%p!!!", drv->hwmem);
	}

	create_char_dev(drv);
	pci_set_drvdata(pdev, drv);
	return 0;
}

void release_device(struct pci_dev *pdev)
{
	pci_release_region(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
	pci_disable_device(pdev);
}

static void my_driver_remove(struct pci_dev *pdev)
{
	struct my_driver_priv *drv_priv = pci_get_drvdata(pdev);
	if (drv_priv)
	{
		kfree(drv_priv);
	}
	release_device(pdev);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Test PCI driver");
MODULE_VERSION("0.1 alfa");

module_init(mypci_driver_init);
module_exit(mypci_driver_exit);
