# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
# KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/kernel-source
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/build
# KERNEL_HEADERS=/home/kmc/work/rz_v2l/linux/build.f1
KERNEL_HEADERS=/home/kmc/work/h3/linux/build

CFILES = driver.c
obj-m := mod-psci.o
$(obj-m:.o=-objs) := $(CFILES:.c=.o)
obj-mm := $(subst -,_,$(obj-m:.o=))
obj-ko := $(obj-m:.o=.ko)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement
ccflags-y += -DDRIVER_NAME=\"$(obj-m:.o=)\"

LDF		= ../../linux-load-file/ldf.elf
LDKO		= ../../linux-load-module/ldko.elf
LOOP64		= ../../arm64-loop/loop64.img 
BOOTFILE	= $(LOOP64)

# RZ/T2
#BOOTADDR	= 0x380000000

# R-CarH3 test1
#BOOTADDR	= 0x60000000
#TARGETCPU	= 3	# Clulster 0 Core 3

# R-CarH3 test2
BOOTADDR	= 0x730000000
CLUSTER		= 1
CORE		= 0
TARGETCPU	= 0x100	# Clulster 1 Core 0

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) modules
	-modinfo ./*.ko
clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) clean

.PHONY: rmmod insmod
lsmod:
	@echo $(obj-mm)
	@lsmod | grep $(obj-mm)
	@n=$(shell lsmod | grep $(obj-mm)|wc -l ); echo $$n

rmmod:
	@n=$(shell lsmod | grep $(obj-mm)|wc -l ); [ "$$n" -ne "0" ] && rmmod $(obj-mm)
	#-lsmod | grep $(obj-mm)

reinsmod: rmmod
	#-modprobe $(obj-mm)
	insmod $(obj-ko)
	-lsmod | grep $(obj-mm)
	-dmesg | tail
	-ls -l /proc/psciop
	#-grep "" -r /sys/module/$(obj-m:.o=)/parameters/

insmod:
	@n=$(shell lsmod | grep $(obj-mm)|wc -l ); \
		[ "$$n" -eq "0" ] && insmod $(obj-ko) || lsmod | grep $(obj-mm)

on: insmod
	-echo on $(CLUSTER) $(CORE) $(BOOTADDR) > /proc/psciop

off: insmod
	-echo off $(CLUSTER) $(CORE) > /proc/psciop

load:
	@echo $(LDF) $(BOOTADDR) $(BOOTFILE)

