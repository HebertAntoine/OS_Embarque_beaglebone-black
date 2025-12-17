#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0xf28efea0, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x97f3532c, "class_unregister" },
	{ 0x5223e490, "device_destroy" },
	{ 0x18ff5646, "class_destroy" },
	{ 0x28415ff3, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x54471b96, "__class_create" },
	{ 0xead162ef, "__register_chrdev" },
	{ 0xfcec0987, "enable_irq" },
	{ 0xdbdb0e8b, "request_any_context_irq" },
	{ 0xcdcc56c, "gpiod_to_irq" },
	{ 0xb8662a86, "gpio_to_desc" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0x822137e2, "arm_heavy_mb" },
	{ 0xe97c4103, "ioremap" },
	{ 0xc5850110, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "DE7A2BD488F8FFEE4A2EF76");
