obj-m += example.o
EXTRA_CFLAGS += -g -Wall -Werror

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
