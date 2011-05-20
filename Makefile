CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) echoserver.c util.c -o echoserver
	$(CC) $(CFLAGS) echoserver-poll.c util.c -o echoserver-poll
	$(CC) $(CFLAGS) echoserver-fork.c util.c -o echoserver-fork
	$(CC) $(CFLAGS) echoserver-time.c util.c -o echoserver-time
