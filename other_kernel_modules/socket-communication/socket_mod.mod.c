#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

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
	{ 0x4454730e, "kmalloc_trace" },
	{ 0xbe45c6fe, "kernel_recvmsg" },
	{ 0x122c3a7e, "_printk" },
	{ 0xa7eedcc4, "call_usermodehelper" },
	{ 0x37a0cba, "kfree" },
	{ 0xdccea0da, "sock_release" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xcdb746d4, "sock_create" },
	{ 0x5a2a4dbf, "kernel_bind" },
	{ 0xbb5fc2f5, "kernel_listen" },
	{ 0x5d626f20, "kthread_create_on_node" },
	{ 0xbca77c8c, "wake_up_process" },
	{ 0x908dcd2e, "kthread_stop" },
	{ 0x6fadc295, "kernel_sock_shutdown" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7046cb67, "kernel_accept" },
	{ 0xb88db70c, "kmalloc_caches" },
	{ 0x2fa5cadd, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "90C51905A2B04DAC32490CA");
