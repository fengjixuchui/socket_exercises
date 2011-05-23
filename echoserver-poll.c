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

#include "util.h"

#define MAX_CONNECTIONS 20
#define BUFLEN 1024

int main(int argc, char **argv) {

  struct addrinfo hints, *res;
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  char *listenport;
  int listenfd;
  int yes, err, sflags;
  struct pollfd pollfds[MAX_CONNECTIONS];
  int x, i;

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

  setnonblock(pollfds[0].fd);

  /* set the first poll socket to the listen socket. */
  pollfds[0].fd = listenfd;

  while(1) {
    if (poll(pollfds, MAX_CONNECTIONS, -1)) {
      for (i = 0; i < MAX_CONNECTIONS; i++)  {
        /* if an event happened */
        if (pollfds[i].revents)  {
          /* if it is our listen socket */
          if (i == 0) {
            int newfd;
            /* accept the new connection and add it to the first available
             * slot in our list of pollable sockets */
            newfd = accept(pollfds[0].fd, (struct sockaddr*)&their_addr,
                &addr_size);
            setnonblock(newfd);
            for (x = 1; x < MAX_CONNECTIONS; x++)  {
              if (pollfds[x].fd == -1) {
                pollfds[x].fd = newfd;
                printf("New connection on socket %d\n", newfd);
                break;
              }
            }
          /* there is data to be read on a socket other than our listen
           * socket */
          } else {
            /* close socket and remove if there was a explicit hangup */
            if (pollfds[i].revents & POLLHUP) {
              hangup(&pollfds[i].fd);
              pollfds[i].revents = 0;
            }
            /* some readable data arrived. echo it. */
            else if (pollfds[i].revents & POLLIN) {
              int numbytes = 0;
              char buf[BUFLEN];
              numbytes = recv(pollfds[i].fd, buf, sizeof(buf), 0);
              send(pollfds[i].fd, buf, numbytes, 0);
              /* zero bytes recv'd on a POLLIN event means that the remote
               * connection was probably closed. Hang up in this situation. */
              if (numbytes == 0)  {
                hangup(&pollfds[i].fd);
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
