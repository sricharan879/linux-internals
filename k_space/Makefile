K_DIR = /lib/modules/$(shell uname -r)/build
obj-m += sample.o

all:
	make -C $(K_DIR)  M=$(shell pwd) modules

clean:
	make -C $(K_DIR)  M=$(shell pwd) clean



