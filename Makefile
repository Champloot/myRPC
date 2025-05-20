SUBDIRS = libmysyslog client server

all:
    for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir; \
    done

install:
    for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir install; \
    done

clean:
    for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir clean; \
    done

deb:
    for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir deb; \
    done

.PHONY: all install clean deb
