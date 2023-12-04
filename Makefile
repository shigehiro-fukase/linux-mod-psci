# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)

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
	sudo insmod $(obj-m:.o=.ko) gpo=42 nsec=200000000 verbose=false
	grep "" -r /sys/module/gpio_blinker/parameters/

test:
	raspi-gpio set 42 op pn
	raspi-gpio get 42
