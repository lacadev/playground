/* Implementation of the tutorial: */
/* https://www.binarytides.com/socket-programming-c-linux-tutorial/ */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/* Structs being used internally

//IPv4 AF_INET sockets
struct sockaddr_in {
  // Protocol (e.g. AF_INET, AF_INET6)
  short sin_family;
  // Port in network byte order (e.g htons(80))
  // See https://stackoverflow.com/questions/19207745/htons-function-in-socket-programing
  unsigned short sin_port;
  // See struct below
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct in_addr {
  // IP address in long format
  // load with inet_pton(), which  converts an IP address to a long format
  unsigned long s_addr;
};

struct sockaddr {
  // Address family, AF_XXX
  unsigned short sa_family;
  // 14 bytes of protocol
  char sa_data[14];
};

*/

int main(int argc, char *argv[]) {
  int socket_descriptor;
  struct sockaddr_in server;

  // Create socket
  // AF_INET means we're using IPv4
  // SOCK_STREAM means we want to use the TCP protocol
  socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor == -1) {
    close(socket_descriptor);
    printf("Could not create socket");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully!\n");

  server.sin_addr.s_addr = inet_addr("1.1.1.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(80);

  // Connect to server
  if (connect(socket_descriptor, (struct sockaddr *)& server, sizeof(server)) < 0) {
    close(socket_descriptor);
    printf("Failed to connect to server\n");
    exit(EXIT_FAILURE);
  }
  printf("Connect to server successfully!\n");


  close(socket_descriptor);

  return 0;
}
