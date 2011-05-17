#include <sys/socket.h>

#include "sendall.h"

int sendall(int sfd, char *buf, int len, int flags)  {

  int sent = 0, left = len, n = len;

  while (sent < n)  {
    left = send(sfd, buf + sent, len, flags);
    if (left == -1) {
      return -1;
    }
    sent += left;
    len -= left;
  }
  return 0;
}
