# -m表示編譯成module
obj-m +=misc.o

# linux kernel source code的路徑
KDIR:=/home/eric/work/100ask_imx6ull-qemu/linux-4.9.88

# 當前路徑
PWD?=$(shell pwd)

# -C代表指定變數KDIR為工作目錄，代表會切換目錄到KDIR底下
# M代表使用PWD目錄下的程式碼
# modules用來指示make編譯成kernel module
all:
	make -C $(KDIR) M=$(PWD) modules