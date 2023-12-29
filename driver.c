#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/arm-smccc.h>
// #include <linux/uapi/linux/psci.h>
#include <linux/psci.h>
#include <asm/uaccess.h>
#include "../psci.h"

MODULE_LICENSE("Dual BSD/GPL");
#ifndef DRIVER_NAME
#define DRIVER_NAME "psci-kick"	/* /proc/devices, /proc/modules */
#endif /* DRIVER_NAME */

/* default value */

static bool use_hvc = false;
module_param(use_hvc, bool, S_IRUGO);
MODULE_PARM_DESC(use_hvc, "Select SMC/HVC N:SMC(default) Y:HVC");

static bool use_asm = false;
module_param(use_asm, bool, S_IRUGO);
MODULE_PARM_DESC(use_asm, "Select API/ASM N:Linux-API(default) Y:ASM");


static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

static unsigned long function_id = PSCI_FN_CPU_ON;
module_param(function_id, ulong, S_IRUGO);
MODULE_PARM_DESC(function_id, "arm smccc a0");
static unsigned long arg0 = 3;
module_param(arg0, ulong, S_IRUGO);
MODULE_PARM_DESC(arg0, "arm smccc a1");
static unsigned long arg1 = 0x380000000;
module_param(arg1, ulong, S_IRUGO);
MODULE_PARM_DESC(arg1, "arm smccc a2");
static unsigned long arg2 = 0;
module_param(arg2, ulong, S_IRUGO);
MODULE_PARM_DESC(arg2, "arm smccc a3");

static int __init mod_init(void);
static void __exit mod_exit(void);


static void print_settings(void) {
	printk(DRIVER_NAME ": func_id=0x%lX arg0=%lu arg1=%lu arg2=%lu\n", function_id, arg0, arg1, arg2);
}


/* At load (insmod) */
static int __init mod_init(void) {
	unsigned long psci_ret;
	printk(DRIVER_NAME ": mod_init\n");

	print_settings();

	psci_ret = psci_cpu_on(use_hvc, use_asm,
			function_id, arg0, arg1);

	printk(DRIVER_NAME ": insmod done.\n");
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
}

module_init(mod_init);
module_exit(mod_exit);
