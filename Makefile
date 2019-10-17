BINARY		:= mydev
KERNEL		:= /lib/modules/4.19.0-6-686-pae/build
ARCH		:= x86
C_FLAGS		:= -Wall
KMOD_DIR	:= $(shell pwd)
TARGET_PATH := /lib/modules/4.19.0-6-686-pae/kernel/drivers/char

OBJECTS	:= \
		mydev_main.o\
		chardev.o\

ccflags-y += $(C_FLAGS)

obj-m += $(BINARY).o

$(BINARY)-y := $(OBJECTS)

$(BINARY).ko:
	make -C $(KERNEL) M=$(KMOD_DIR) modules

install:
	cp $(BINARY).ko $(TARGET_PATH)
	depmod -a

clean: 
	rm -rf *.o *.ko *.mod *.symvers *.order
