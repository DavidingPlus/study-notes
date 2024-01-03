LIBPATH = /home/applications/server/libs/
OBJS = $(LIBPATH)cmysql.o $(LIBPATH)clinklist.o $(LIBPATH)cstring.o $(LIBPATH)chttp.o $(LIBPATH)csocket.o
CC = gcc
all:index news message
CFLAGS = -Wall -O -g
index:index.o $(OBJS)
	$(CC) $(CFLAGS) index.o $(OBJS) -o index -L/alidata/server/mysql/lib -lmysqlclient
index.o:index.c
	$(CC) -o index.o -c index.c
	
news:news.o
	$(CC) $(CFLAGS)  news.o $(OBJS)  -o news -L/alidata/server/mysql/lib -lmysqlclient
news.o:news.c
	$(CC) -o news.o -c news.c
	
message:message.o
	$(CC) $(CFLAGS)  message.o $(OBJS)  -o message -L/alidata/server/mysql/lib -lmysqlclient
massage.o:message.c
	$(CC) -o message.o -c message.c
	
clean:
	rm -rf *.o