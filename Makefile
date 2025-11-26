# #obj-m += gpio_pwm_combined.o

# all:
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		-C /kernel M=$(PWD) modules

# clean:
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		-C /kernel M=$(PWD) clean


# Makefile pour compiler deux modules noyau :
#  - gpio_blink.c   -> gpio_blink.ko
#  - pwm_kernel.c   -> pwm_kernel.ko

obj-m += gpio_blink.o
obj-m += pwm_kernel.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
        -C /kernel M=$(PWD) modules

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
        -C /kernel M=$(PWD) clean