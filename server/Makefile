CC = gcc
CFLAGS = -Wall -Wextra -I../libmysyslog/include -I/usr/include/json-c
LDFLAGS = -L../libmysyslog -lmysyslog -ljson-c
TARGET = myrpc-server
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/myrpc_server.c
OBJS = $(OBJ_DIR)/myrpc_server.o

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    mkdir -p $(OBJ_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

install:
    install -d /usr/local/bin /etc/myRPC
    install -m 755 $(TARGET) /usr/local/bin/
    install -m 644 ../configs/server.conf /etc/myRPC/myRPC.conf
    install -m 644 ../configs/users.conf /etc/myRPC/users.conf

clean:
    rm -rf $(OBJ_DIR) $(TARGET)

deb:
    dh_make --createorig -s -y
    dpkg-buildpackage -us -uc

.PHONY: all install clean deb
