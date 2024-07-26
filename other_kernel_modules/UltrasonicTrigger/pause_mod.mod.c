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
	{ 0x2aee72c, "kthread_stop" },
	{ 0x4f7f67fa, "netlink_kernel_release" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0xea74d62b, "__netlink_kernel_create" },
	{ 0x527f1381, "init_net" },
	{ 0xf9a482f9, "msleep" },
	{ 0x2e4bbc99, "send_sig" },
	{ 0x2d5f69b3, "rcu_read_unlock_strict" },
	{ 0xade2f989, "pid_task" },
	{ 0xc678ad64, "find_vpid" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x481ead8c, "wake_up_process" },
	{ 0x683eadc2, "kthread_create_on_node" },
	{ 0xb248d598, "__alloc_skb" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D6D995FFE9A5DB508D29293");
