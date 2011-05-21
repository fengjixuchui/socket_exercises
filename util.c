#include "util.h"

void die(const char *strerr)  {
  fprintf(stderr, "%s\n", strerr);
  exit(errno);
}

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

void setnonblock(int fd) {
  int sflags;
  sflags = fcntl(fd, F_GETFL);
  sflags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, sflags);
}

int getlistenfd(char *port) {

  int lfd;
  struct addrinfo hints, *res;
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int yes, err;

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((err = getaddrinfo(NULL, port, &hints, &res)))  {
    die(gai_strerror(err));
  }

  if ((lfd = socket(res->ai_family, res->ai_socktype,
          res->ai_protocol)) < 0) {
    die("Couldn't get listen socket.");
  }

  if ((err = bind(lfd, res->ai_addr, res->ai_addrlen))) {
    die(gai_strerror(err));
  }

  if ((err = listen(lfd, MAX_CONNECTIONS))) {
    die(gai_strerror(err));
  }

  freeaddrinfo(&res);
  setnonblock(lfd);

  return lfd;
}
