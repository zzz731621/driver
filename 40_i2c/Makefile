# -m表示編譯成module
obj-m +=driver.o

# linux kernel source code的路徑
KDIR:=/home/eric/work/100ask_imx6ull-qemu/linux-4.9.88

# 當前路徑
PWD?=$(shell pwd)

# -C代表指定變數KDIR為工作目錄，代表會切換目錄到KDIR底下
# M代表使用PWD目錄下的程式碼
# modules用來指示make編譯成kernel module
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	mv driver.c a.c
	rm d* .d* Mo* m*
	rm .t* -rf
	mv a.c driver.c

# 編譯驅動之前，linux kernel一定要先編譯過
# 編譯驅動module用的linux kernel必須要和開發版上運行的kernel img是一樣的