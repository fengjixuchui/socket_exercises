#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>

#include "server.h"

/* Print error message and exit */
void die(const char *strmsg);

/* Send all bytes to a socket */
int sendall(int sfd, char *buf, int len, int flags);

/* Close a socket in a poll array. */
void hangup(int *sfd);

/* Set a socket to non-blocking mode for asynchronous I/O. */
void setnonblock(int fd);

/* Socket, bind, and listen routine */
int getlistenfd(char *port);
