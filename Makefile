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
		arg0=3 \
		arg1=0x60000000 \
		arg2=0 \
		verbose=Y
	sudo dmesg | tail
	grep "" -r /sys/module/$(obj-m:.o=)/parameters/

test:
	@echo insmod $(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=3 \
		arg1=0x60000000 \
		arg2=0 \
		verbose=Y

test_h3_1:
	@echo ../linux-load-file/ldf.elf 0x60000000 ../../arm64-loop/loop64.img 
	@echo ../linux-load-module/ldko.elf \
		--init_module \
		$(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=3 \
		arg1=0x60000000 \
		arg2=0 \
		verbose=Y

test_h3_2:
	@echo ../linux-load-file/ldf.elf 0x730000000 ../../arm64-loop/loop64.img 
	@echo ../linux-load-module/ldko.elf \
		--init_module \
		$(obj-m:.o=.ko) \
		use_hvc=N \
		use_asm=N \
		arg0=0x100 \
		arg1=0x730000000 \
		arg2=0 \
		verbose=Y

