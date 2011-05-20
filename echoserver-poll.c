#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>

#include "sendall.h"

#define MAX_CONNECTIONS 20
#define BUFLEN 1024

void die(const char *strerr)  {
  fprintf(stderr, "%s\n", strerr);
  exit(errno);
}

int main(int argc, char **argv) {

  struct addrinfo hints, *res;
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  char *listenport;
  int listenfd;
  int yes, err, sflags;
  char buf[BUFLEN];
  struct pollfd pollfds[MAX_CONNECTIONS];
  int x, i; /* counters etc */
  int bytes; /* number of bytes sent */

  listenport = argv[1];

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((err = getaddrinfo(NULL, listenport, &hints, &res)))
    die(gai_strerror(err));

  /* initialize poll array. */
  for (i = 0; i < MAX_CONNECTIONS; i++)  {
      pollfds[i].fd = -1;
      pollfds[i].events = POLLIN | POLLHUP;
      pollfds[i].revents = 0;
  }

  if ((listenfd = socket(res->ai_family, res->ai_socktype,
          res->ai_protocol)) < 0)
    die("Couldn't get listen socket.");

  if ((err = bind(listenfd, res->ai_addr, res->ai_addrlen)))
    die(gai_strerror(err));

  if ((err = listen(listenfd, MAX_CONNECTIONS)))
    die(gai_strerror(err));

  if ((err = (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes,
      sizeof(int)))))
    die(gai_strerror(err));

  /* set listen socket to be non blocking */
  sflags = fcntl(listenfd, F_GETFL);
  sflags |= O_NONBLOCK;
  fcntl(listenfd, F_SETFL, sflags);

  /* set the first poll socket to the listen socket. */
  pollfds[0].fd = listenfd;

  while(1) {
    if (poll(pollfds, MAX_CONNECTIONS, -1)) {
      for (i = 0; i < MAX_CONNECTIONS; i++)  {
        if (pollfds[i].revents)  { /* if an event happened */
          if (i == 0) { /* If it is our listener socket */
            int newfd;
            newfd = accept(pollfds[0].fd, (struct sockaddr*)&their_addr,
                &addr_size);
            /* add the new fd to the list of pollable fds */
            for (x = 1; x < MAX_CONNECTIONS; x++)  {
              if (pollfds[x].fd == -1) { /* the first available fd */
                pollfds[x].fd = newfd;
                printf("new connection on socket %d pollfd[%d]\n", newfd, x);
                break;
              }
            }
          } else  { /* there is data to be read */
            if (pollfds[i].revents & POLLHUP) { /* lost connection */
              /* reset the fd, remote it from array */
              printf("hangup on socket %d pollfd[%d]\n",
                  pollfds[i].fd, i);
              close(pollfds[i].fd);
              pollfds[i].fd = -1;
              pollfds[i].revents = 0;
            }
            else if (pollfds[i].revents & POLLIN) { /* some data arrived, echo it */
              while ((bytes = recv(pollfds[i].fd, buf, sizeof(buf), 0)) > 0)  {
                buf[bytes] = 0;
                sendall(pollfds[i].fd, buf, bytes, 0);
              }
            }
          }
        }
      }
    }
  }

  freeaddrinfo(res);
  return 0;
}
