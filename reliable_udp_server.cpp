// reliable_udp_server.cpp

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
#include <time.h>
#include <cstring>

#define MYPORT "4950"    // the port users will be connecting to
#define MAXBUFLEN 100
#define INF 20000000
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

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    struct sockaddr_storage their_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n",
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    // dealing with png
    string FileName = buf;
    ifstream StreamPng;
    StreamPng.open(FileName.c_str(), ios_base::binary);
    StreamPng.seekg(0, ios::end);
    int datasize = StreamPng.tellg(); // 设置文件指针到末尾，读出文件内容大小
    char *imgbuff = new char[INF];
    StreamPng.seekg(0, ios::beg);
    StreamPng.read(imgbuff+1, datasize);
    cout << "Intended size is: " << datasize << endl;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }

    clock_t tstart, tend;
    tstart = clock();
    int tmp;
    int i = 1;
    int serial_num = 0;
    int recv_info;
    char recv_buf[10];
    while (i <= datasize) {
        *(imgbuff+i-1) = '0' + serial_num%10;
        if (i + maxsize - 1 <= datasize) {
            if ((numbytes = sendto(sockfd, imgbuff+i-1, maxsize, 0,
                     (const struct sockaddr *)&their_addr, addr_len)) == -1) {
                perror("talker: sendto");
                exit(1);
            }
            
            if ((recv_info = recvfrom(sockfd, recv_buf, sizeof(char), 0,
                (struct sockaddr *)&their_addr, &addr_len)) > 0) {
                if (recv_buf[0] - '0' == serial_num%10) {
                    i += numbytes-1;
                    serial_num++;
                }
            }
        }
        else {
            if ((numbytes = sendto(sockfd, imgbuff+i-1, datasize-i+2, 0,
                     (const struct sockaddr *)&their_addr, addr_len)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            if ((recv_info = recvfrom(sockfd, recv_buf, sizeof(char), 0,
                (struct sockaddr *)&their_addr, &addr_len)) > 0) {
                if (recv_buf[0] - '0' == serial_num%10) {
                    i += numbytes-1;
                    serial_num++;
                }
            }
        }
    }
    tend = clock();

    delete[] imgbuff;
    cout << "Sent size is: " << i-1 << "(serial number and duplicate packets not included)" << endl;
    cout << "The UDP transmission time of " << buf << " is " << tend - tstart << "ms." << endl;
    close(sockfd);

    return 0;
}
