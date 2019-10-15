#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

static struct pci_device_id my_driver_id_table[] = {
	{ PCI_DEVICE(0x0104, 0x0001) }, //VID & PID of device
	{0,}
};

MODULE_DEVICE_TABLE(pci, my_driver_id_table);

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);

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
}

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent){
//Do there what we want
	return 0;
}

void release_device(struct pci_dev *pdev)
{
	pci_disable_device(pdev);
}

static void my_driver_remove(struct pci_dev *pdev)
{
	release_device(pdev);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Test PCI driver");
MODULE_VERSION("0.1 alfa");

module_init(mypci_driver_init);
module_exit(mypci_driver_exit);

