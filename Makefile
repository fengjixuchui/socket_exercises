CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) echoserver.c -o echoserver
	$(CC) $(CFLAGS) echoserver-poll.c -o echoserver-poll
	$(CC) $(CFLAGS) echoserver-fork.c -o echoserver-fork
	$(CC) $(CFLAGS) echoserver-time.c -o echoserver-time
