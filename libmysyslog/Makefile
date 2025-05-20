CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I./include
LDFLAGS = -shared
TARGET = libmysyslog.so
SRC_DIR = src
OBJ_DIR = obj
INSTALL_DIR = /usr/local

SRCS = $(SRC_DIR)/mysyslog.c
OBJS = $(OBJ_DIR)/mysyslog.o

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    mkdir -p $(OBJ_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

install:
    install -d $(INSTALL_DIR)/include $(INSTALL_DIR)/lib
    install -m 644 include/mysyslog.h $(INSTALL_DIR)/include/
    install -m 755 $(TARGET) $(INSTALL_DIR)/lib/

uninstall:
    rm -f $(INSTALL_DIR)/include/mysyslog.h
    rm -f $(INSTALL_DIR)/lib/$(TARGET)

clean:
    rm -rf $(OBJ_DIR) $(TARGET)

deb:
    dh_make --createorig -s -y
    dpkg-buildpackage -us -uc

.PHONY: all install uninstall clean deb
