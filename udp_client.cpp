// udp_client.cpp

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define SERVERPORT "4950"    // the port users will be connecting to
#define MYPORT "4951"
#define MAXBUFLEN 20000000
#define maxsize 50000

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, hints2, *servinfo, *p, *servinfo2, *p2;
    int rv, rv2;
    int numbytes;
    char *buf = new char[MAXBUFLEN];
    char s[INET_ADDRSTRLEN];
    socklen_t addr_len;
    struct sockaddr_storage their_addr;

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    memset(&hints2, 0, sizeof hints2);
    hints2.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints2.ai_socktype = SOCK_DGRAM;
    hints2.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    if ((rv2 = getaddrinfo(NULL, MYPORT, &hints2, &servinfo2)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv2));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    // loop through all the results and bind to the first we can
    for (p2 = servinfo2; p2 != NULL; p2 = p2->ai_next) {
        if (bind(sockfd, p2->ai_addr, p2->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);

    int total = 0, ttotal;
    if ((ttotal = recvfrom(sockfd, buf+total, maxsize, 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        total++;
        perror("recvfrom");
        exit(1);
    }
    total += ttotal;

    clock_t tstart, tend;
    tstart = tend = clock();

    cout << total << " has been received." << endl;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }

    while (true) {
        if ((ttotal = recvfrom(sockfd, buf+total, maxsize, 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            total++;
        }
        total += ttotal;
        tend = clock();
        if (tend > tstart + 100000) {
            break;
        }
    }
    printf("listener: got packet from %s\n",
                inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s));
    printf("listener: packet is %d bytes long\n", total);
    buf[total] = '\0';
    freeaddrinfo(servinfo);
    close(sockfd);
    delete[] buf;
    return 0;
}
