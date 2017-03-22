obj-m += test.o

#CFLAGS_test.o := -DSCHEDULE_IN_INTR

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
