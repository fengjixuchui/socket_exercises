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

void hangup(int *sfd) {

  printf("Connection closed on socket %d\n", *sfd);

  /* Close the socket. */
  close(*sfd);

  /* "Remove" the socket from the poll array. */
  *sfd = -1;
}
