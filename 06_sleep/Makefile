obj-m += sleep.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc test.c -o cat_nonblock

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f cat_nonblock

