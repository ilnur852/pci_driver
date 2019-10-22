#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xb57c9d32, "module_layout" },
	{ 0xe61d9d25, "kmalloc_caches" },
	{ 0xbeb90d2e, "cdev_init" },
	{ 0xf8147c92, "pci_release_region" },
	{ 0xe1a3d2bf, "pci_disable_device" },
	{ 0xad27f361, "__warn_printk" },
	{ 0x253fad60, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xe445e0e7, "printk" },
	{ 0x600c7b84, "class_unregister" },
	{ 0xa8f01b82, "device_create" },
	{ 0x3284cec8, "cdev_add" },
	{ 0xdd892d20, "pci_select_bars" },
	{ 0x93a219c, "ioremap_nocache" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xeaf82907, "pci_unregister_driver" },
	{ 0x9d05cfae, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0xb6f8eb53, "__pci_register_driver" },
	{ 0x1fa06916, "class_destroy" },
	{ 0x16a6c5e6, "pci_enable_device_mem" },
	{ 0x4a453f53, "iowrite32" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x9478a3f7, "__class_create" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xe484e35f, "ioread32" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00000104d00000001sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "C27E5C82BA5F40CA676EC26");
