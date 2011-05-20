#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

/* Send all bytes to a socket */
int sendall(int sfd, char *buf, int len, int flags);

/* Close a socket in a poll array. */
void hangup(int *sfd);
