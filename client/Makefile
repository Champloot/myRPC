CC = gcc
CFLAGS = -Wall -Wextra -I../libmysyslog/include
LDFLAGS = -L../libmysyslog/lib -lmysyslog -ljansson

all: myrpc-client

myrpc-client: myrpc_client.o
    $(CC) -o $@ $^ $(LDFLAGS)

%.o: src/%.c
    $(CC) $(CFLAGS) -c $<

clean:
    rm -f *.o myrpc-client

deb:
    dh_make --createorig -s -y
    dpkg-buildpackage -us -uc
