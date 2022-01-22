// a stream socket client(TCP)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 20000000 // max number of bytes we can get at once

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int recvall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've received
    int bytesleft = *len; // how many we have left to receive
    int n;

    while(total < *len) {
        if ((n = recv(s, buf, MAXDATASIZE-1, 0)) == -1)
            break;
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually received here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char *buf = new char[MAXDATASIZE];
    int *buff = new int;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if (send(sockfd, argv[2], 10, 0) == -1)
        perror("send");

    if (recv(sockfd, buff, sizeof(int), 0) == -1)
        {perror("recv"); exit(1);}
    int t = *buff;

    if (recvall(sockfd, buf, &t) == -1)
        {perror("recv"); exit(1);}

    buf[numbytes] = '\0';
    cout << "Received size is: " << t << endl;
    delete[] buf;
    close(sockfd);

    return 0;
}
