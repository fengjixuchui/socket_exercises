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

/* Each thread has a list of pollable sockets. The first socket is polled for
 * newly accept'ed sockets from the main routine */
struct pollfd pollfds[NUM_THREADS][MAX_CONNECTIONS];

void *thread_main (void *);

int main(int argc, char **argv) {

  if (argc < 2) {
    errno = EXIT_FAILURE;
    die("Usage: echoserver-* <port>\n");
  }

  struct sockaddr_storage their_addr;
  socklen_t addr_size;

  int listenfd;
  int x, i;
  int tid = 0, lowest_conns = 0;

  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];
  int num_current_conns[NUM_THREADS];
  struct pollfd listenpollfd[1];

  listenfd = getlistenfd(argv[1]);
  listenpollfd[0].fd = listenfd;
  listenpollfd[0].events = POLLIN;
  listenpollfd[0].revents = 0;

  /* initialize poll fds */
  for (i = 0; i < NUM_THREADS; i++) {
    for (x = 0; x < MAX_CONNECTIONS; x++) {
      pollfds[i][x].fd = -1;
      pollfds[i][x].events = POLLIN | POLLHUP;
      pollfds[i][x].revents = 0;
    }
  }

  /* Start fixed amount of listening threads */
  for (i = 0; i < NUM_THREADS; i++) {
    thread_args[i] = i;
    pthread_create(&threads[i], NULL, thread_main, &thread_args[i]);
  }

  /* accept loop. Delegate accept'ed fds to a thread */
  for (;;)  {
    if (poll(listenpollfd, 0, -1)) {
      if (listenpollfd[0].revents & POLLIN) {
        int sfd = accept(listenfd, (struct sockaddr*)&their_addr,
            &addr_size);

        /* add fd to thread with least amount of connections */
        for(i = 0; i < NUM_THREADS; i++)  {
          if (num_current_conns[i] < lowest_conns) {
            tid = i;
            lowest_conns = num_current_conns[i];
          }
        }
        /* Find first open fd slot for our thread */
        for (i = 0; i < MAX_CONNECTIONS; i++) {
          if (i == -1)  { break;  }
        }
        setnonblock(sfd);
        pollfds[tid][i].fd = sfd;
      }
    }
  }

  /* Wait for threads to complete. */
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return EXIT_SUCCESS;
}

void *thread_main (void *ptid)  {

  int tid = *((int *) ptid);
  int i = 0;

  while(1)  {
    if (poll(pollfds[tid], MAX_CONNECTIONS, 100))  {
      for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (pollfds[tid][i].revents & POLLHUP) {
          hangup(&pollfds[tid][i].fd);
        } else if (pollfds[tid][i].revents & POLLIN) {
          /* echo data */

        }
      }
    }
  }

  return NULL;
}
