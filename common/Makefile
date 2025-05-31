# Makefile for libmysyslog

CC        := gcc
CFLAGS    := -Wall -g -fPIC
SRC_DIR   := .
OBJ_DIR   := obj
LIB_DIR   := lib
TARGET    := $(LIB_DIR)/libmysyslog.so

SRC       := $(wildcard $(SRC_DIR)/*.c)
OBJ       := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean deb install

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) *.deb

deb: clean all
	# Create temporary directory structure
	mkdir -p deb_pkg/usr/lib
	mkdir -p deb_pkg/usr/include
	mkdir -p deb_pkg/DEBIAN
	# Copy files
	install -m 755 $(TARGET) deb_pkg/usr/lib/
	install -m 644 libmysyslog.h deb_pkg/usr/include/
	# Create control file
	echo "Package: libmysyslog" > deb_pkg/DEBIAN/control
	echo "Version: 1.0-1" >> deb_pkg/DEBIAN/control
	echo "Section: libs" >> deb_pkg/DEBIAN/control
	echo "Priority: optional" >> deb_pkg/DEBIAN/control
	echo "Architecture: amd64" >> deb_pkg/DEBIAN/control
	echo "Maintainer: Artem <artem.avert@gmail.com>" >> deb_pkg/DEBIAN/control
	echo "Description: System logging library" >> deb_pkg/DEBIAN/control
	# Build the package
	fakeroot dpkg-deb --build deb_pkg ../deb/libmysyslog_1.0-1_amd64.deb
	# Clean up
	rm -rf deb_pkg

install:
	install -D -m 755 $(TARGET) $(DESTDIR)/usr/lib/libmysyslog.so
	install -D -m 644 libmysyslog.h $(DESTDIR)/usr/include/libmysyslog.h
