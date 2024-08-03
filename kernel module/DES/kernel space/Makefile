PWD := $(shell pwd)
MODULE_NAME := des
ARCH := arm
CROSS_COMPILE := arm-xilinx-linux-gnueabi-
KSRC := /home/eos/lab3/pynqz2/build/tmp/work-shared/zynq-generic/kernel-build-artifacts

obj-m := $(MODULE_NAME).o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(PWD)  modules
clean:
	rm *.o *.ko *.mod.c Module* module* *.mod

