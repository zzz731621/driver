# $(CONFIG_HELLO)會根據我們選擇的狀態改變
# 若選擇編譯進kernel就是-y
# 若選擇編譯成module就是-m
# CONFIG_HELLO就是我們在Kconfig下的HELLO，只是因為Kconfig會省略前面的CONFIG_
# 實際上也可以直接在drivers/char/Makefile加上obj-y+=helloworld.o這行，效果是一樣的，但這樣寫擴充性更高
obj-$(CONFIG_HELLO)+=helloworld.o