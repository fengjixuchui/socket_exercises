CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) echoserver.c sendall.c -o echoserver
	$(CC) $(CFLAGS) echoserver-poll.c sendall.c -o echoserver-poll
	$(CC) $(CFLAGS) echoserver-fork.c sendall.c -o echoserver-fork
	$(CC) $(CFLAGS) echoserver-time.c sendall.c -o echoserver-time
