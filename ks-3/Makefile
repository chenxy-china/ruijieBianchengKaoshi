#"-O2", "-fmax-errors=10", "-Wall","-Werror", "-lm", "-lpthread", "--static", "-std=gnu99", "-DONLINE_JUDGE" 

#编译器
CROSS_COMPILE :=
#CROSS_COMPILE := arm-linux-gnueabihf-

CC := $(CROSS_COMPILE)gcc

#当前路径
CUR_DIR := $(shell pwd)

#中间文件路径
OBJ_DIR := $(CUR_DIR)/obj

#输出路径
BIN_DIR := $(CUR_DIR)

# 头文件路径
INCLUDE :=
INCLUDE += -I$(CUR_DIR)/include/

#C编译器的选项
CFLAGS :=
CFLAGS += -O2
CFLAGS += -fmax-errors=10
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -static
CFLAGS += -std=gnu99
CFLAGS += -DONLINE_JUDGE
CFLAGS += $(INCLUDE)

 # If you want a static binary, you might uncomment these
LDFLAGS = -static
STRIP = -s

#库文件参数
LDLIBS :=
LDLIBS += -lm
LDLIBS += -lpthread


SRCS = $(wildcard *.c) 
OBJS = $(SRCS:%.c=%.o)
BINS = $(SRCS:%.c=%)


all:clean dir_create $(OBJS) $(BINS) 

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) $^ -o $(OBJ_DIR)/$@

$(BINS):%:%.o
	$(CC) -o $(BIN_DIR)/$@ $(OBJ_DIR)/$^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/$(BINS)


dir_create:  
	@$(call CRT_DIR,$(OBJ_DIR))
	@$(call CRT_DIR,$(BIN_DIR))

define CRT_DIR
	if [ ! -d $(1) ];\
	 	then\
    	mkdir -p $(1);\
    	echo " $(1) created success!";\
    else\
    	echo " $(1) has been created!";\
	fi	
endef