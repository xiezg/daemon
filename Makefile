all:daemon

daemon:main.c
	gcc main.c -o daemon

