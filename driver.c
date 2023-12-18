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

MODULE_LICENSE("Dual BSD/GPL");
#ifndef DRIVER_NAME
#define DRIVER_NAME "psci-kick"	/* /proc/devices, /proc/modules */
#endif /* DRIVER_NAME */

#define PSCI_FN32_BASE	0x84000000
#define PSCI_64_OFF	0x40000000
#define PSCI_FN64_BASE	(PSCI_FN32_BASE + PSCI_64_OFF)
#if defined(PSCI32)
#error "NOT SUPPORT"
#else
#define PSCI_FN_BASE	PSCI_FN64_BASE
#endif
#define PSCI_FN(n)			(PSCI_FN_BASE + (n))
#define PSCI_FN_PSCI_VERSION		PSCI_FN(0)
#define PSCI_FN_CPU_SUSPEND		PSCI_FN(1)
#define PSCI_FN_CPU_OFF			PSCI_FN(2)
#define PSCI_FN_CPU_ON			PSCI_FN(3)
#define PSCI_FN_AFFINITY_INFO		PSCI_FN(4)
#define PSCI_FN_MIGRATE			PSCI_FN(5)
#define PSCI_FN_MIGRATE_INFO_TYPE	PSCI_FN(6)
#define PSCI_FN_MIGRATE_INFO_UP_CPU	PSCI_FN(7)
#define PSCI_FN_SYSTEM_OFF		PSCI_FN(8)
#define PSCI_FN_SYSTEM_RESET		PSCI_FN(9)


/* PSCI return values (copy from linux-kernel/include/uapi/linux/psci.h) */
#define PSCI_RET_SUCCESS			0
#define PSCI_RET_NOT_SUPPORTED			-1
#define PSCI_RET_INVALID_PARAMS			-2
#define PSCI_RET_DENIED				-3
#define PSCI_RET_ALREADY_ON			-4
#define PSCI_RET_ON_PENDING			-5
#define PSCI_RET_INTERNAL_FAILURE		-6
#define PSCI_RET_NOT_PRESENT			-7
#define PSCI_RET_DISABLED			-8
#define PSCI_RET_INVALID_ADDRESS		-9

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

static __always_inline unsigned long
invoke_psci_fn_hvc(unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2) {
	struct arm_smccc_res res;
	arm_smccc_hvc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	return res.a0;
}
static __always_inline unsigned long
invoke_psci_fn_smc(unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2) {
	struct arm_smccc_res res;
	arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	return res.a0;
}
/*
 * void smc_call(arg0, arg1...arg7)
 * issue the secure monitor call
 * x0~x7: input arguments
 * x0~x3: output arguments
 */
struct call_regs {
	unsigned long regs[7];
};
static void hvc_call(struct call_regs *args) {
	asm volatile(
			"ldr x0, %0\n" /* <--- Function ID */
			"ldr x1, %1\n" /* <--- Input parameters */
			"ldr x2, %2\n"
			"ldr x3, %3\n"
			"ldr x4, %4\n"
			"ldr x5, %5\n"
			"ldr x6, %6\n"
			"hvc     #0\n" /* <--- SMC call (or hvc for HVC) */
			"str x0, %0\n" /* <--- Results */
			"str x1, %1\n"
			"str x2, %2\n"
			"str x3, %3\n"
			: "+m" (args->regs[0]), "+m" (args->regs[1]), "+m" (args->regs[2]), "+m" (args->regs[3])
			: "m" (args->regs[4]), "m" (args->regs[5]), "m" (args->regs[6])
			: "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
			"x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
			"x16", "x17");
}
static void smc_call(struct call_regs *args) {
	asm volatile(
			"ldr x0, %0\n" /* <--- Function ID */
			"ldr x1, %1\n" /* <--- Input parameters */
			"ldr x2, %2\n"
			"ldr x3, %3\n"
			"ldr x4, %4\n"
			"ldr x5, %5\n"
			"ldr x6, %6\n"
			"smc     #0\n" /* <--- SMC call (or hvc for HVC) */
			"str x0, %0\n" /* <--- Results */
			"str x1, %1\n"
			"str x2, %2\n"
			"str x3, %3\n"
			: "+m" (args->regs[0]), "+m" (args->regs[1]), "+m" (args->regs[2]), "+m" (args->regs[3])
			: "m" (args->regs[4]), "m" (args->regs[5]), "m" (args->regs[6])
			: "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
			"x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
			"x16", "x17");
}
static unsigned long invoke_psci_fn(unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2) {
	if (!use_asm) {
		if (use_hvc) {
			return invoke_psci_fn_hvc(function_id, arg0, arg1, arg2);
		} else {
			return invoke_psci_fn_smc(function_id, arg0, arg1, arg2);
		}
	} else {
		struct call_regs args = { .regs = { function_id, arg0, arg1, arg2, 0, 0, 0 } };
		if (use_hvc) {
			hvc_call(&args);
		} else {
			smc_call(&args);
		}
		return args.regs[0];
	}
}
static void print_psci_retval(unsigned long retval) {
	const char * s;
	switch (retval) {
		case PSCI_RET_SUCCESS:          s = "PSCI_RET_SUCCESS";          break;
		case PSCI_RET_NOT_SUPPORTED:    s = "PSCI_RET_NOT_SUPPORTED";    break;
		case PSCI_RET_INVALID_PARAMS:   s = "PSCI_RET_INVALID_PARAMS";   break;
		case PSCI_RET_DENIED:           s = "PSCI_RET_DENIED";           break;
		case PSCI_RET_ALREADY_ON:       s = "PSCI_RET_ALREADY_ON";       break;
		case PSCI_RET_ON_PENDING:       s = "PSCI_RET_ON_PENDING";       break;
		case PSCI_RET_INTERNAL_FAILURE: s = "PSCI_RET_INTERNAL_FAILURE"; break;
		case PSCI_RET_NOT_PRESENT:      s = "PSCI_RET_NOT_PRESENT";      break;
		case PSCI_RET_DISABLED:         s = "PSCI_RET_DISABLED";         break;
		case PSCI_RET_INVALID_ADDRESS:  s = "PSCI_RET_INVALID_ADDRESS";  break;
		default: s = "UNKNOWN"; break;
	}
	printk(DRIVER_NAME ": retvalue %lu \"%s\"\n", retval, s);
}
static unsigned long psci_cpu_on(unsigned long fn, unsigned long cpuid, unsigned long entry_point) {
	unsigned long psci_ret;
	psci_ret = invoke_psci_fn(fn, cpuid, entry_point, 0);
	print_psci_retval(psci_ret);
	return psci_ret;
}


static void print_settings(void) {
	printk(DRIVER_NAME ": func_id=0x%lX arg0=%lu arg1=%lu arg2=%lu\n", function_id, arg0, arg1, arg2);
}


/* At load (insmod) */
static int __init mod_init(void) {
	unsigned long psci_ret;
	printk(DRIVER_NAME ": mod_init\n");

	print_settings();

	psci_ret = psci_cpu_on(function_id, arg0, arg1);

	printk(DRIVER_NAME ": insmod done.\n");
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
}

module_init(mod_init);
module_exit(mod_exit);
