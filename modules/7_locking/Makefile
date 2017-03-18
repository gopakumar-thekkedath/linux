obj-m += lock_mod.o
CFLAGS_lock_mod.o := -DUSE_TIMER
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
