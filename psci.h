#ifndef ___PSCI_H___
#define ___PSCI_H___

#define PSCI_FN32_BASE	0x84000000
#define PSCI_64_OFF	0x40000000
#define PSCI_FN64_BASE	(PSCI_FN32_BASE + PSCI_64_OFF)
#define PSCI_CMD_PSCI_VERSION			(0x00)
#define PSCI_CMD_CPU_SUSPEND			(0x01)
#define PSCI_CMD_CPU_OFF			(0x02)
#define PSCI_CMD_CPU_ON				(0x03)
#define PSCI_CMD_AFFINITY_INFO			(0x04)
#define PSCI_CMD_MIGRATE			(0x05)
#define PSCI_CMD_MIGRATE_INFO_TYPE		(0x06)
#define PSCI_CMD_MIGRATE_INFO_UP_CPU		(0x07)
#define PSCI_CMD_SYSTEM_OFF			(0x08)
#define PSCI_CMD_SYSTEM_RESET			(0x09)
#define PSCI_CMD_FEATURES			(0x0A)
#define PSCI_CMD_CPU_FREEZE			(0x0B)
#define PSCI_CMD_CPU_DEFAULT_SUSPEND		(0x0C)
#define PSCI_CMD_NODE_HW_STATE			(0x0D)
#define PSCI_CMD_SYSTEM_SUSPEND			(0x0E)
#define PSCI_CMD_SET_SUSPEND_MODE		(0x0F)
#define PSCI_CMD_STAT_RESIDENCY			(0x10)
#define PSCI_CMD_STAT_COUNT			(0x11)
#define PSCI_CMD_SYSTEM_RESET2			(0x12)
#define PSCI_CMD_MEM_PROTECT			(0x13)
#define PSCI_CMD_MEM_PROTECT_CHECK_RANGE	(0x14)
#define PSCI_CMD_CLEAN_INV_MEMREGION		(0x15)
#define PSCI_CMD_CLEAN_INV_MEMREGION_ATTRIBUTES	(0x16)


#define PSCI_FN32(n)				(PSCI_FN32_BASE + (n))
#define PSCI_FN64(n)				(PSCI_FN64_BASE + (n))
#define PSCI_FN_PSCI_VERSION			PSCI_FN32(PSCI_CMD_PSCI_VERSION)
#define PSCI_FN_CPU_SUSPEND			PSCI_FN64(PSCI_CMD_CPU_SUSPEND)
#define PSCI_FN_CPU_OFF				PSCI_FN32(PSCI_CMD_CPU_OFF)
#define PSCI_FN_CPU_ON				PSCI_FN64(PSCI_CMD_CPU_ON)
#define PSCI_FN_AFFINITY_INFO			PSCI_FN64(PSCI_CMD_AFFINITY_INFO)
#define PSCI_FN_MIGRATE				PSCI_FN64(PSCI_CMD_MIGRATE)
#define PSCI_FN_MIGRATE_INFO_TYPE		PSCI_FN32(PSCI_CMD_MIGRATE_INFO_TYPE)
#define PSCI_FN_MIGRATE_INFO_UP_CPU		PSCI_FN64(PSCI_CMD_MIGRATE_INFO_UP_CPU)
#define PSCI_FN_SYSTEM_OFF			PSCI_FN32(PSCI_CMD_SYSTEM_OFF)
#define PSCI_FN_SYSTEM_RESET			PSCI_FN32(PSCI_CMD_SYSTEM_RESET)
#define PSCI_FN_FEATURES			PSCI_FN32(PSCI_CMD_FEATURES)
#define PSCI_FN_CPU_FREEZE			PSCI_FN32(PSCI_CMD_CPU_FREEZE)
#define PSCI_FN_CPU_DEFAULT_SUSPEND		PSCI_FN64(PSCI_CMD_CPU_DEFAULT_SUSPEND)
#define PSCI_FN_NODE_HW_STATE			PSCI_FN64(PSCI_CMD_NODE_HW_STATE)
#define PSCI_FN_SYSTEM_SUSPEND			PSCI_FN64(PSCI_CMD_SYSTEM_SUSPEND)
#define PSCI_FN_SET_SUSPEND_MODE		PSCI_FN32(PSCI_CMD_SET_SUSPEND_MODE)
#define PSCI_FN_STAT_RESIDENCY			PSCI_FN64(PSCI_CMD_STAT_RESIDENCY)
#define PSCI_FN_STAT_COUNT			PSCI_FN64(PSCI_CMD_STAT_COUNT)
#define PSCI_FN_SYSTEM_RESET2			PSCI_FN64(PSCI_CMD_SYSTEM_RESET2)
#define PSCI_FN_MEM_PROTECT			PSCI_FN32(PSCI_CMD_MEM_PROTECT)
#define PSCI_FN_MEM_PROTECT_CHECK_RANGE		PSCI_FN64(PSCI_CMD_MEM_PROTECT_CHECK_RANGE)
#define PSCI_FN_CLEAN_INV_MEMREGION		PSCI_FN64(PSCI_CMD_CLEAN_INV_MEMREGION)
#define PSCI_FN_CLEAN_INV_MEMREGION_ATTRIBUTES	PSCI_FN32(PSCI_CMD_CLEAN_INV_MEMREGION_ATTRIBUTES)

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
#define PSCI_RET_TIMEOUT			-10
#define PSCI_RET_RATE_LIMITED			-11
#define PSCI_RET_BUSY				-12

static const unsigned long psci_function_id[] = {
	PSCI_FN_PSCI_VERSION,
	PSCI_FN_CPU_SUSPEND,
	PSCI_FN_CPU_OFF,
	PSCI_FN_CPU_ON,
	PSCI_FN_AFFINITY_INFO,
	PSCI_FN_MIGRATE,
	PSCI_FN_MIGRATE_INFO_TYPE,
	PSCI_FN_MIGRATE_INFO_UP_CPU,
	PSCI_FN_SYSTEM_OFF,
	PSCI_FN_SYSTEM_RESET,
	PSCI_FN_FEATURES,
	PSCI_FN_CPU_FREEZE,
	PSCI_FN_CPU_DEFAULT_SUSPEND,
	PSCI_FN_NODE_HW_STATE,
	PSCI_FN_SYSTEM_SUSPEND,
	PSCI_FN_SET_SUSPEND_MODE,
	PSCI_FN_STAT_RESIDENCY,
	PSCI_FN_STAT_COUNT,
	PSCI_FN_SYSTEM_RESET2,
	PSCI_FN_MEM_PROTECT,
	PSCI_FN_MEM_PROTECT_CHECK_RANGE,
	PSCI_FN_CLEAN_INV_MEMREGION,
	PSCI_FN_CLEAN_INV_MEMREGION_ATTRIBUTES,
};

static __always_inline unsigned long
invoke_psci_fn_hvc(unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2,
		struct arm_smccc_res * resp
		) {
	struct arm_smccc_res res;
	arm_smccc_hvc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	if (resp) *resp = res;
	return res.a0;
}
static __always_inline unsigned long
invoke_psci_fn_smc(unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2,
		struct arm_smccc_res * resp
		) {
	struct arm_smccc_res res;
	arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	if (resp) *resp = res;
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
static inline void hvc_call(struct call_regs *args) {
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
static inline void smc_call(struct call_regs *args) {
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
static inline unsigned long invoke_psci_fn(bool use_hvc, bool use_asm,
		unsigned long function_id,
		unsigned long arg0, unsigned long arg1, unsigned long arg2,
		struct arm_smccc_res *resp
		) {
	if (!use_asm) {
		if (use_hvc) {
			return invoke_psci_fn_hvc(function_id, arg0, arg1, arg2, resp);
		} else {
			return invoke_psci_fn_smc(function_id, arg0, arg1, arg2, resp);
		}
	} else {
		struct call_regs args = { .regs = { function_id, arg0, arg1, arg2, 0, 0, 0 } };
		if (use_hvc) {
			hvc_call(&args);
			if (resp) {
				resp->a0 = args.regs[0];
				resp->a1 = args.regs[1];
				resp->a2 = args.regs[2];
				resp->a3 = args.regs[3];
			}
		} else {
			smc_call(&args);
			if (resp) {
				resp->a0 = args.regs[0];
				resp->a1 = args.regs[1];
				resp->a2 = args.regs[2];
				resp->a3 = args.regs[3];
			}
		}
		return args.regs[0];
	}
}
static inline void print_psci_retval(unsigned long retval) {
	const char * s;
	int32_t ecode = (int32_t)retval;
	switch (ecode) {
		case PSCI_RET_SUCCESS:          s = "SUCCESS";          break;
		case PSCI_RET_NOT_SUPPORTED:    s = "NOT_SUPPORTED";    break;
		case PSCI_RET_INVALID_PARAMS:   s = "INVALID_PARAMS";   break;
		case PSCI_RET_DENIED:           s = "DENIED";           break;
		case PSCI_RET_ALREADY_ON:       s = "ALREADY_ON";       break;
		case PSCI_RET_ON_PENDING:       s = "ON_PENDING";       break;
		case PSCI_RET_INTERNAL_FAILURE: s = "INTERNAL_FAILURE"; break;
		case PSCI_RET_NOT_PRESENT:      s = "NOT_PRESENT";      break;
		case PSCI_RET_DISABLED:         s = "DISABLED";         break;
		case PSCI_RET_INVALID_ADDRESS:  s = "INVALID_ADDRESS";  break;
		case PSCI_RET_TIMEOUT:		s = "TIMEOUT";          break;
		case PSCI_RET_RATE_LIMITED:	s = "RATE_LIMITED";     break;
		case PSCI_RET_BUSY:		s = "BUSY";             break;
		default: s = "UNKNOWN"; break;
	}
	printk(DRIVER_NAME ": retvalue 0x%lx \"%s\"\n", retval, s);
}
static inline void print_psci_res(struct arm_smccc_res *resp) {
	if (!resp) return;
	printk(DRIVER_NAME ": a0=0x%lX a1=0x%lX a2=0x%lX a3=0x%lX\n",
				resp->a0, resp->a1, resp->a2, resp->a3);
}

#endif /* ___PSCI_H___ */
