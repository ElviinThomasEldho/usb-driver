obj-m += usb-read.o

PWD = $(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

