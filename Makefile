CC = gcc
OBJS = proxy.c func.h func.c
TARGET = proxy_cache

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lcrypto
