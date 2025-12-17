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
	{ 0x6adfab7b, "param_ops_int" },
	{ 0x97f3532c, "class_unregister" },
	{ 0x5223e490, "device_destroy" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0x18ff5646, "class_destroy" },
	{ 0x28415ff3, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x54471b96, "__class_create" },
	{ 0xead162ef, "__register_chrdev" },
	{ 0x4384eb42, "__release_region" },
	{ 0xae9849dd, "__request_region" },
	{ 0xc94d8e3b, "iomem_resource" },
	{ 0xedc03953, "iounmap" },
	{ 0x822137e2, "arm_heavy_mb" },
	{ 0xe97c4103, "ioremap" },
	{ 0xc5850110, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9990367B7AFC00DA145F0C7");
