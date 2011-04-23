#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BACKLOG 10

int main(int argc, char **argv) {

    struct addrinfo hints, *res;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    char *listenport;
    int listenfd, connfd, yes;
    char buf[1024];
    size_t bytes;

    listenport = argv[1];

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, listenport, &hints, &res);
    printf("%d %s\n", err, gai_strerror(errno));
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(listenfd, res->ai_addr, res->ai_addrlen);
    listen(listenfd, BACKLOG);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    addr_size = sizeof(their_addr);
    connfd = accept(listenfd, (struct sockaddr*)&their_addr, &addr_size);
    while ((bytes = recv(connfd, buf, sizeof(buf), 0)))  {
        send(connfd, buf, strlen(buf), 0);
    }

    freeaddrinfo(res);
    close(listenfd);
    close(connfd);

    return 0;
}
