obj-m += mutex.o
obj-m += spinlock.o
obj-m += RWLock.o
obj-m += atomicOp.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

