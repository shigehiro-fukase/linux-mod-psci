#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/arm-smccc.h>
// #include <linux/uapi/linux/psci.h>
#include <linux/psci.h>
#include <uapi/linux/psci.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include "./psci.h"

MODULE_LICENSE("Dual BSD/GPL");
#ifndef DRIVER_NAME
#define DRIVER_NAME	"mod-psci"	/* /proc/devices, /proc/modules */
#endif /* DRIVER_NAME */
#ifndef PROC_NAME
#define PROC_NAME	"psci"		/* procfs name of /proc/ */
#endif /* PROC_NAME */

#define LINUX_VERSION_CODE_RELEASE	(((LINUX_VERSION_CODE)>>16)&0xff)
#define LINUX_VERSION_CODE_MAJOR	(((LINUX_VERSION_CODE)>>8)&0xff)
#define LINUX_VERSION_CODE_MINOR	(((LINUX_VERSION_CODE)>>0)&0xff)

/* default value */

static bool use_hvc = false;
module_param(use_hvc, bool, S_IRUGO);
MODULE_PARM_DESC(use_hvc, "Select SMC/HVC N:SMC(default) Y:HVC");

static bool use_asm = false;
module_param(use_asm, bool, S_IRUGO);
MODULE_PARM_DESC(use_asm, "Select API/ASM N:Linux-API(default) Y:ASM");

static bool init_act = false;
module_param(init_act, bool, S_IRUGO);
MODULE_PARM_DESC(init_act, "(optional) use initial action");

static bool procfs_io = true;
module_param(procfs_io, bool, S_IRUGO);
MODULE_PARM_DESC(procfs_io, "(optional) use procfs i/o");

static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

static unsigned long psci_cmd = PSCI_CMD_CPU_ON;
module_param(psci_cmd, ulong, S_IRUGO);
MODULE_PARM_DESC(psci_cmd, "arm smccc func");

static unsigned long cpu = 0;
module_param(cpu, ulong, S_IRUGO);
MODULE_PARM_DESC(cpu, "cpu cluster+core");
static unsigned long cpu_cluster = 0;
module_param(cpu_cluster, ulong, S_IRUGO);
MODULE_PARM_DESC(cpu_cluster, "cpu cluster");
static unsigned long cpu_core = 0;
module_param(cpu_core, ulong, S_IRUGO);
MODULE_PARM_DESC(cpu_core, "cpu core");
static unsigned long entrypoint = 0;
module_param(entrypoint, ulong, S_IRUGO);
MODULE_PARM_DESC(entrypoint, "entrypoint");

static unsigned long arg0 = 0;
module_param(arg0, ulong, S_IRUGO);
MODULE_PARM_DESC(arg0, "arm smccc arg0");
static unsigned long arg1 = 0;
module_param(arg1, ulong, S_IRUGO);
MODULE_PARM_DESC(arg1, "arm smccc arg1");
static unsigned long arg2 = 0;
module_param(arg2, ulong, S_IRUGO);
MODULE_PARM_DESC(arg2, "arm smccc arg2");

static int __init mod_init(void);
static void __exit mod_exit(void);


unsigned long psci_cmd2id(unsigned long cmd) {
	unsigned long function_id;

	if (psci_cmd < (sizeof(psci_function_id)/sizeof(psci_function_id[0]))) {
		function_id = psci_function_id[psci_cmd];
	} else {
		function_id = psci_cmd;
	}
	return function_id;
}
static void print_settings(void) {
	unsigned long function_id = psci_cmd2id(psci_cmd);
	printk(DRIVER_NAME ": func(%lu) id=0x%lX arg0=0x%lX arg1=0x%lX arg2=0x%lX\n", psci_cmd, function_id, arg0, arg1, arg2);
}
static int psci_invoke(void) {
	unsigned long psci_ret;
	unsigned long function_id = psci_cmd2id(psci_cmd);

	struct arm_smccc_res res;
	psci_ret = invoke_psci_fn(use_hvc, use_asm,
			function_id, arg0, arg1, arg2,
			&res);

	print_psci_retval(psci_ret);
	print_psci_res(&res);
	return psci_ret;
}

static int strsplit(char * s, char* av[], int avsz) {
	int i;
	if (!s || !av || !avsz) return 0;
	for (i=0; i<(avsz-1); ) {
		for (; s && *s; s++) if (!isspace(*s)) break;
		if (!s || !*s) break;
		av[i++] = s;
		for (; s && *s; s++) if (!isgraph(*s)) break;
		if (!s || !*s) break;
		*s++ = '\0';
	}
	av[i] = NULL;
	return i;
}
#define is0_9(c) (((int)'0' <= (int)(c)) && ((int)(c) <= (int)'9'))
static int parse_line(char * buf, char* av[], int avsz) {
	int ret, ac, i;
	unsigned long cmd = psci_cmd;
	unsigned long cluster = 0;
	unsigned long core = 0;
	int has_arg0 = 0;
	int has_arg1 = 0;
	int has_arg2 = 0;
	// printk(DRIVER_NAME ": %s line='%s'\n", __func__, buf);
	ac = strsplit(buf, av, avsz);
	for (i=0; i<ac; i++) {
		char* s = av[i];
		if (!s) break;
		// printk(DRIVER_NAME ": %s av[%d]='%s'\n", __func__, i, s);
		if (i==0) { // function
			if (is0_9(*s)) {
				ret = kstrtoul(s, 0, &cmd);
				// printk(DRIVER_NAME ": %s cmd=%ld ret=%d\n", __func__, cmd, ret);
			} else if ((strcasecmp(s, "version") == 0) || (strcasecmp(s, "ver") == 0)) {
				cmd = PSCI_CMD_PSCI_VERSION;
			} else if ((strcasecmp(s, "cpu_suspend") == 0) || (strcasecmp(s, "suspend") == 0) || (strcasecmp(s, "sus") == 0)) {
				cmd = PSCI_CMD_CPU_SUSPEND;
			} else if ((strcasecmp(s, "cpu_off") == 0) || (strcasecmp(s, "off") == 0)) {
				cmd = PSCI_CMD_CPU_OFF;
			} else if ((strcasecmp(s, "cpu_on") == 0) || (strcasecmp(s, "on") == 0)) {
				cmd = PSCI_CMD_CPU_ON;
			} else if ((strcasecmp(s, "affinity_info") == 0) || (strcasecmp(s, "affinity") == 0)) {
				cmd = PSCI_CMD_AFFINITY_INFO;
			} else if (strcasecmp(s, "migrate") == 0) {
				cmd = PSCI_CMD_MIGRATE;
			} else if (strcasecmp(s, "migrate_info_type") == 0) {
				cmd = PSCI_CMD_MIGRATE_INFO_TYPE;
			} else if (strcasecmp(s, "migrate_info_up_cpu") == 0) {
				cmd = PSCI_CMD_MIGRATE_INFO_UP_CPU;
			} else if ((strcasecmp(s, "system_off") == 0) || (strcasecmp(s, "sys_off") == 0) || (strcasecmp(s, "halt") == 0)) {
				cmd = PSCI_CMD_SYSTEM_OFF;
			} else if ((strcasecmp(s, "system_reset") == 0) || (strcasecmp(s, "sys_reset") == 0) || (strcasecmp(s, "reset") == 0)) {
				cmd = PSCI_CMD_SYSTEM_RESET;
			} else if (strcasecmp(s, "features") == 0) {
				cmd = PSCI_CMD_FEATURES;
			} else if (strcasecmp(s, "cpu_freeze") == 0) {
				cmd = PSCI_CMD_CPU_FREEZE;
			} else if (strcasecmp(s, "cpu_default_suspend") == 0) {
				cmd = PSCI_CMD_CPU_DEFAULT_SUSPEND;
			} else if (strcasecmp(s, "node_hw_state") == 0) {
				cmd = PSCI_CMD_NODE_HW_STATE;
			} else if (strcasecmp(s, "system_suspend") == 0) {
				cmd = PSCI_CMD_SYSTEM_SUSPEND;
			} else if (strcasecmp(s, "set_suspend_mode") == 0) {
				cmd = PSCI_CMD_SET_SUSPEND_MODE;
			} else if (strcasecmp(s, "stat_residency") == 0) {
				cmd = PSCI_CMD_STAT_RESIDENCY;
			} else if (strcasecmp(s, "stat_count") == 0) {
				cmd = PSCI_CMD_STAT_COUNT;
			} else if (strcasecmp(s, "system_reset2") == 0) {
				cmd = PSCI_CMD_SYSTEM_RESET2;
			} else if (strcasecmp(s, "mem_protect") == 0) {
				cmd = PSCI_CMD_MEM_PROTECT;
			} else if (strcasecmp(s, "mem_protect_check_range") == 0) {
				cmd = PSCI_CMD_MEM_PROTECT_CHECK_RANGE;
			} else if (strcasecmp(s, "clean_inv_memregion") == 0) {
				cmd = PSCI_CMD_CLEAN_INV_MEMREGION;
			} else if (strcasecmp(s, "clean_inv_memregion_attributes") == 0) {
				cmd = PSCI_CMD_CLEAN_INV_MEMREGION_ATTRIBUTES;
			} else {
				printk(DRIVER_NAME ": %s BAD argument[%d] '%s'\n", __func__, i, s);
				return -1;
			}
		} else if (strncmp(s, "cluster=", strlen("cluster=")) == 0) {
			char *p = s + strlen("cluster=");
			ret = kstrtoul(p, 0, &cluster);
			arg0 = ((cluster & 0xFF) << 8) | ((core & 0xFF) << 0);
			// printk(DRIVER_NAME ": %s cluster=%ld ret=%d arg0=0x%lX\n", __func__, cluster, ret, arg0);
			has_arg0 = 1;
		} else if (strncmp(s, "core=", strlen("core=")) == 0) {
			char *p = s + strlen("core=");
			ret = kstrtoul(p, 0, &core);
			arg0 = ((cluster & 0xFF) << 8) | ((core & 0xFF) << 0);
			// printk(DRIVER_NAME ": %s core=%ld ret=%d arg0=0x%lX\n", __func__, core, ret, arg0);
			has_arg0 = 1;
		} else if (strncmp(s, "address=", strlen("address=")) == 0) {
			char *p = s + strlen("address=");
			ret = kstrtoul(p, 0, &arg1);
			// printk(DRIVER_NAME ": %s address=0x%lX ret=%d\n", __func__, arg1, ret);
			has_arg1 = 1;
		} else if (has_arg0 == 0) {
			ret = kstrtoul(s, 0, &arg0);
			// printk(DRIVER_NAME ": %s arg0=%ld ret=%d\n", __func__, arg0, ret);
			has_arg0 = 1;
		} else if (has_arg1 == 0) {
			ret = kstrtoul(s, 0, &arg1);
			// printk(DRIVER_NAME ": %s arg1=%ld ret=%d\n", __func__, arg1, ret);
			has_arg1 = 1;
		} else if (has_arg2 == 0) {
			ret = kstrtoul(s, 0, &arg2);
			// printk(DRIVER_NAME ": %s arg2=%ld ret=%d\n", __func__, arg2, ret);
			has_arg2 = 1;
		} else {
			printk(DRIVER_NAME ": %s BAD argument[%d], index overflow.\n", __func__, i);
			return -1;
		}
	}
	psci_cmd = cmd;
	print_settings();
	return ac;
}

static char proc_linebuf[0x400];
static char av[32];

static int mod_proc_open(struct inode *inode, struct file *file) {
	printk(DRIVER_NAME ": %s\n", __func__);
	return 0;
}
static int mod_proc_close(struct inode *inode, struct file *file) {
	printk(DRIVER_NAME ": %s\n", __func__);
	return 0;
}
static ssize_t mod_proc_read(struct file *file, char __user *buf, size_t count, loff_t *fpos) {
	printk(DRIVER_NAME ": %s\n", __func__);
	return 0;
}
static ssize_t mod_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *fpos) {
	printk(DRIVER_NAME ": %s\n", __func__);
	if (count > sizeof(proc_linebuf)) count = sizeof(proc_linebuf) -1;
	if (__copy_from_user(proc_linebuf, buf, count)) {
		return -EFAULT;
	}
	proc_linebuf[count] = '\0';
	int ret;
	ret = parse_line(proc_linebuf, (char**)&av, sizeof(av));
	if (ret != -1) {
		psci_invoke();
	}
	// printk(DRIVER_NAME ": %s '%s'\n", __func__, proc_linebuf);
	return count;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
static struct file_operations mod_proc_fops = {
	.owner = THIS_MODULE,
	.open = mod_proc_open,
	.release = mod_proc_close,
	.read = mod_proc_read,
	.write = mod_proc_write,
};
#else
static struct proc_ops mod_proc_fops = {
	.proc_open = mod_proc_open,
	.proc_release = mod_proc_close,
	.proc_read = mod_proc_read,
	.proc_write = mod_proc_write,
};
#endif

/* At load (insmod) */
static int __init mod_init(void) {
	printk(DRIVER_NAME ": %s\n", __func__);

	if (init_act) {
		unsigned long psci_ret;
		unsigned long function_id = psci_cmd2id(psci_cmd);

		if (cpu) {
			arg0 = cpu;
		} else if (cpu_cluster || cpu_core) {
			arg0 = ((cpu_cluster & 0xFF) << 8) | ((cpu_core & 0xFF) << 0);
		}
		if (entrypoint) {
			arg1 = entrypoint;
		}

		print_settings();

		struct arm_smccc_res res;
		psci_ret = invoke_psci_fn(use_hvc, use_asm,
				function_id, arg0, arg1, arg2,
				&res);
		print_psci_retval(psci_ret);
		print_psci_res(&res);
	}

	if (procfs_io) {
		struct proc_dir_entry *entry;
		entry = proc_create(PROC_NAME, S_IRUGO | S_IWUGO, NULL, &mod_proc_fops);
		if (entry == NULL) {
			printk(KERN_ERR "proc_create\n");
			return -ENOMEM;
		}
	}

	printk(DRIVER_NAME ": %s done.\n", __func__);
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": %s\n", __func__);
	if (procfs_io) {
		remove_proc_entry(PROC_NAME, NULL);
	}
}

module_init(mod_init);
module_exit(mod_exit);
