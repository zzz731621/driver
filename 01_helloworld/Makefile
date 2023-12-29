# -m表示編譯成module
obj-m +=helloworld.o

# linux kernel source code的路徑
KDIR:=/home/eric/work/100ask_imx6ull-qemu/linux-4.9.88

# 當前路徑
PWD?=$(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules


# 編譯驅動之前，linux kernel一定要先編譯過
# 編譯驅動module用的linux kernel必須要和開發版上運行的kernel img是一樣的