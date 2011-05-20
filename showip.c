#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void die(const char *s) {
    perror(s);
    exit(errno);
}

int main(int argc, char **argv) {
    int status;
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    bzero(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    p = res;
    while (p != NULL) {
        void *addr;
        char *ipver;

        if (p->ai_family == AF_INET)  {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr; 
            addr = &(ipv4->sin_addr);
            ipver = "ipv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "ipv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf(" %s: %s\n", ipver, ipstr);
        p = p->ai_next;
    }

    freeaddrinfo(res);
    return 0;
}
