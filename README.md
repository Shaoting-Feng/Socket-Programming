# Socket-Programming

This project is used for socket programming. It is a commonly used tool for messaging and file transmission on the Internet. In this project, I built a file transmission system that contains a server and a client using TCP and UDP protocols respectively. I programmed in C++ language.

## Features

- **TCP and UDP Protocols**: Implemented both TCP and UDP protocols for file transmission.
- **Reliable UDP**: Enhanced UDP to ensure reliable data transmission by assigning serial numbers to packets and using ACK packets for confirmation.
- **File Transmission**: Supports file transmission between server and client.

## Getting Started

### Prerequisites

- C++ compiler
- Basic understanding of socket programming

### Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/Shaoting-Feng/Socket-Programming.git
   ```
2. Navigate to the project directory:
   ```sh
   cd Socket-Programming
   ```

### Usage

1. Compile the server and client programs:
   ```sh
   g++ tcp_server.cpp -o tcp_server
   g++ tcp_client.cpp -o tcp_client
   g++ udp_server.cpp -o udp_server
   g++ udp_client.cpp -o udp_client
   g++ reliable_udp_server.cpp -o reliable_udp_server
   g++ reliable_udp_client.cpp -o reliable_udp_client
   ```
2. Run the server:
   ```sh
   ./tcp_server
   ./udp_server
   ./reliable_udp_server
   ```
3. Run the client:
   ```sh
   ./tcp_client
   ./udp_client
   ./reliable_udp_client
   ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
