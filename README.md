# Socket-Programming
This project is used for practice socket programming. It is a commonly used tool for messaging and file transmission on the Internet. In this lab, I built a file transmission system that contains a server and a client using TCP and UDP protocols respectively. I programmed in C++ language. You can refer to https://beej.us/guide/bgnet/ for more useful information.

I set a rather long period of time to close down UDP client sockets(10 seconds), please wait a bit. 

The interesting part is to improve UDP to ensure reliable data transmission. I borrowed the idea of TCP protocol. I assigned each UDP packet a serial number. The receiver collects the packets by the order of serial number. If the receiver successfully receives the packet, it will send back an ACK packet, carrying the serial number of that packet. After receiving the ACK packet, the sender can transmit the next packet with serial number adding one.
