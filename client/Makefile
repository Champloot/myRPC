# Makefile for myRPC-client

CC        := gcc
CFLAGS    := -Wall -g -I../common
LDFLAGS   := -L../common/lib -Wl,-rpath=../common/lib -lmysyslog
SRC_DIR   := .
OBJ_DIR   := obj
BIN_DIR   := bin
TARGET    := $(BIN_DIR)/myRPC-client

SRC       := $(wildcard $(SRC_DIR)/*.c)
OBJ       := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean deb install

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.deb

deb: clean all
	# Create temporary directory structure
	mkdir -p deb_pkg/usr/bin
	mkdir -p deb_pkg/DEBIAN
	# Copy the executable
	install -m 755 $(TARGET) deb_pkg/usr/bin/
	# Create control file
	echo "Package: myrpc-client" > deb_pkg/DEBIAN/control
	echo "Version: 1.0-1" >> deb_pkg/DEBIAN/control
	echo "Section: net" >> deb_pkg/DEBIAN/control
	echo "Priority: optional" >> deb_pkg/DEBIAN/control
	echo "Architecture: amd64" >> deb_pkg/DEBIAN/control
	echo "Maintainer: Artem <artem.avert@gmail.com>" >> deb_pkg/DEBIAN/control
	echo "Description: myRPC Client Application" >> deb_pkg/DEBIAN/control
	# Build the package
	fakeroot dpkg-deb --build deb_pkg ../deb/myrpc-client_1.0-1_amd64.deb
	# Clean up
	rm -rf deb_pkg

install:
	install -D -m 755 $(TARGET) $(DESTDIR)/usr/bin/myRPC-client
