obj-m += usbassi1.o
obj-m += usbassi2.o
obj-m += viassi1.o
obj-m += viassi2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
