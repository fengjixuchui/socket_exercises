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
#include <pthread.h>

#include "util.h"

#define NUM_THREADS 6
#define MAX_CONNECTIONS 20
#define BUFLEN 1024

/* Each thread has a list of pollable sockets. The first socket is polled for
 * newly accept'ed sockets from the main routine */
struct pollfd tpollfds[NUM_THREADS][MAX_CONNECTIONS];

void *listen_main (void *listenfd);

void die(const char *strerr)  {
  fprintf(stderr, "%s\n", strerr);
  exit(errno);
}

int setnonblock(int sfd) {
  int sflags;
  sflags = fcntl(sfd, F_GETFL);
  sflags |= O_NONBLOCK;
  fcntl(sfd, F_SETFL, sflags);
}

/* socket, bind, and listen routines */
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

  if ((err = getaddrinfo(NULL, port, &hints, &res)))
    die(gai_strerror(err));

  if ((lfd = socket(res->ai_family, res->ai_socktype,
          res->ai_protocol)) < 0)
    die("Couldn't get listen socket.");

  if ((err = bind(lfd, res->ai_addr, res->ai_addrlen)))
    die(gai_strerror(err));

  if ((err = listen(lfd, MAX_CONNECTIONS)))
    die(gai_strerror(err));

  freeaddrinfo(&res);
  setnonblock(lfd);

  return lfd;
}

int main(int argc, char **argv) {

  if (argc < 2) {
    errno = EXIT_FAILURE;
    die("Usage: echoserver-* <port>\n");
  }

  int listenfd;
  int x, i;

  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];
  int num_current_conns[NUM_THREADS];

  listenport = argv[1];
  listenfd = getlistenfd(listenfd);

  /* initialize poll fds */
  for (i = 0; i < NUM_THREADS; i++) {
    for (x = 0; x < MAX_CONNECTIONS; x++) {
      tpollfds[i][x] = -1;
    }
  }

  /* Start fixed amount of listening threads */
  for (i = 0; i < NUM_THREADS; i++) {
    thread_args[i] = i;
    pthread_create(&threads[i], NULL, listen_main, &thread_args[i]);
  }

  /* accept loop. Delegate accept'ed fds to a thread */
  for (;;)  {
    int sfd = accept(pollfds[0].fd, (struct sockaddr*)&their_addr,
        &addr_size);
    int tid = 0, lowest_conns = 0;

    /* add fd to thread with least amount of connections */
    lowest_conns = num_current_conns[0]
    for(i = 0; i < NUM_THREADS; i++)  {
      if (num_current_conns[i] < lowest_conns) {
        tid = i;
        lowest_conns = num_current_conns[i];
      }
    }

  }

  /* Wait for threads to complete. */
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return EXIT_SUCCESS;
}

void *listen_main (void *ptid)  {

  int tid = *((int *) ptid);
  struct pollfd pollfds[MAX_CONNECTIONS];

  while(1)  {
    if (poll(pollfds, 0, 0))  {
      
    }
  }

  return NULL;
}
