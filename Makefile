SUBDIRS := common client server
DEB_DIR := deb

.PHONY: all clean deb install

all:
	@for dir in $(SUBDIRS); do \
		echo "Building in $$dir"; \
		$(MAKE) -C $$dir all || exit 1; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		echo "Cleaning in $$dir"; \
		$(MAKE) -C $$dir clean || exit 1; \
	done
	rm -rf $(DEB_DIR)

deb: clean
	mkdir -p $(DEB_DIR)
	@for dir in $(SUBDIRS); do \
		echo "Building deb package in $$dir"; \
		$(MAKE) -C $$dir deb || exit 1; \
	done

install:
	@for dir in $(SUBDIRS); do \
		echo "Installing from $$dir"; \
		$(MAKE) -C $$dir install || exit 1; \
	done
