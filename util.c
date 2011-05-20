#include "util.h"

int sendall(int sfd, char *buf, int len, int flags)  {

  int sent = 0, bytesleft = len, n = len;

  while (sent < n)  {
    bytesleft = send(sfd, buf + sent, len, flags);
    if (bytesleft == -1) {
      return -1;
    }
    sent += bytesleft;
    len -= bytesleft;
  }
  return 0;
}

void hangup(struct pollfd *pollfds, int fdnum) {

  printf("Connection on socket %d pollfd[%d] closed\n", pollfds[fdnum].fd,
      fdnum);

  /* Close the socket. */
  close(pollfds[fdnum].fd);

  /* "Remove" the socket from the poll array. */
  pollfds[fdnum].fd = -1;
}
