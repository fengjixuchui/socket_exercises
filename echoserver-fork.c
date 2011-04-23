#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BACKLOG 10

int main(int argc, char **argv) {

    struct addrinfo hints, *res;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    char *listenport;
    int listenfd, connfd;
    char buf[1024];
    size_t bytes;
    int yes;

    listenport = argv[1];

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, listenport, &hints, &res);
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(listenfd, res->ai_addr, res->ai_addrlen);
    listen(listenfd, BACKLOG);


    printf("Waiting for connections...\n");
    while (1) {
        addr_size = sizeof(their_addr);
        connfd = accept(listenfd, (struct sockaddr*)&their_addr, &addr_size);
        yes = 60;
        setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &yes, sizeof(int));
        printf("got connection.\n");

        if (!fork())  {
            close(listenfd);
            while ((bytes = recv(connfd, buf, sizeof(buf), 0)))  {
                send(connfd, buf, strlen(buf), 0);
            }
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    freeaddrinfo(res);
    close(listenfd);
    close(connfd);

    return 0;
}
