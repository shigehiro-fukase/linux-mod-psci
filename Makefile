# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
# KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/kernel-source
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/build
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/build.f1
KERNEL_HEADERS=/home/kmc/work/h3/linux/build

CFILES = driver.c
obj-m := psci-kick.o
$(obj-m:.o=-objs) := $(CFILES:.c=.o)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement
ccflags-y += -DDRIVER_NAME=\"$(obj-m:.o=)\"

LDF		= ../../linux-load-file/ldf.elf
LDKO		= ../../linux-load-module/ldko.elf
LOOP64		= ../../arm64-loop/loop64.img 
#BOOTADDR	= 0x380000000	# RZ/T2

# R-CarH3 test1
#BOOTADDR	= 0x60000000
#BOOTCPU	= 3	# Clulster 0 Core 3

# R-CarH3 test2
BOOTADDR	= 0x730000000
BOOTCPU		= 0x100	# Clulster 1 Core 0

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) modules
	-modinfo ./*.ko
clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) clean

.PHONY: rmmod insmod
rmmod: all
	-sudo rmmod $(obj-m:.o=)
	-lsmod | grep $(obj-m:.o=)

insmod: rmmod
	-modprobe $(obj-m:.o=)
	sudo insmod $(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=$(BOOTCPU) \
		arg1=$(BOOTADDR) \
		arg2=0 \
		verbose=Y
	sudo dmesg | tail
	grep "" -r /sys/module/$(obj-m:.o=)/parameters/

test:
	@echo insmod $(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=$(BOOTCPU) \
		arg1=$(BOOTADDR) \
		arg2=0 \
		verbose=Y

h3:
	@echo $(LDF) $(BOOTADDR) $(LOOP64)
	@echo $(LDKO) \
		--init_module \
		$(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=$(BOOTCPU) \
		arg1=$(BOOTADDR) \
		arg2=0 \
		verbose=Y

