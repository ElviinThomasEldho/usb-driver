#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0x602c1205, "module_layout" },
	{ 0xde4db734, "usb_deregister" },
	{ 0x4f7f67fa, "netlink_kernel_release" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x6a1fcc3e, "usb_register_driver" },
	{ 0xea74d62b, "__netlink_kernel_create" },
	{ 0x527f1381, "init_net" },
	{ 0x61b37490, "netlink_unicast" },
	{ 0x9166fada, "strncpy" },
	{ 0x893adddd, "__nlmsg_put" },
	{ 0xb248d598, "__alloc_skb" },
	{ 0x92997ed8, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x50f4a523, "_dev_info" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v0781p5567d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "0083B6DD47492929F39156C");
