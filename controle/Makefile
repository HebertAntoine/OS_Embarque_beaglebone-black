obj-m:=kgpio.o kirq.o kpwm.o

KDIR := /usr/src/linux-headers-4.19.94-ti-xenomai-r66

all: default 


default:
	$(MAKE)  ARCH=arm  -C ${KDIR} SUBDIRS=$(shell pwd) modules

clean:
	rm -f *.[oas] .*.flags *.ko .*.cmd .*.d .*.tmp *.mod.c ioctl_pwm

