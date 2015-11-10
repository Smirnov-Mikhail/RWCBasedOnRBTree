CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
EXTRA_CFLAGS += -g -Wall -Werror

OBJS = workingWithFile.o

obj-m += example.o
example-objs := main.o workWithFile.o rbTree.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



