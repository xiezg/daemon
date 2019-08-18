all:daemon

daemon:main.c
	gcc main.c -o daemon

.PHONY:install

install:
	cp daemon /root/go/src/dbbak/
	cp daemon /root/go/src/bin/
