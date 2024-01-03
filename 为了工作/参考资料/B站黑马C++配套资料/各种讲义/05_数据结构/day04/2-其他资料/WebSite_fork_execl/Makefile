LIBPATH = ./libs/
CC = gcc
#获得当前目录下的所有.c文件
SOURCES = $(wildcard *.c $(LIBPATH)*.c)
#将所有.c文件替换成.o文件
OBJS = $(patsubst %.c,%.o,$(SOURCES))
#$(warning $(OBJS)) 打印变量
CFLAGS = -Wall -O -g
TARGET = server

#输出变量  $(warning $(SOURCE))
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -L/alidata/server/mysql/lib -lpthread -lmysqlclient
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *.o
	rm -rf $(LIBPATH)*.o