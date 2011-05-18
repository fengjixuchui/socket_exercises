#include <sys/socket.h>

#include "sendall.h"

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
