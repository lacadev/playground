#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  char *message, server_reply[2000];

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

  server.sin_addr.s_addr = inet_addr("138.201.81.199");
  server.sin_family = AF_INET;
  server.sin_port = htons(80);

  // Connect to server
  if (connect(socket_descriptor, (struct sockaddr *)& server, sizeof(server)) < 0) {
    close(socket_descriptor);
    printf("Failed to connect to server\n");
    exit(EXIT_FAILURE);
  }
  printf("Connect to server successfully!\n");

  // Send data to the server
  // HTTP command to fetch the mainpage of a website
  message = "GET / HTTP/1.1\r\nHost: archlinux.org\r\n\r\n";
  // We could also use write() instead of send(), as with any file.
  // With sockets, write() is equivalent to send() with no flags (set to 0)
  if (send(socket_descriptor, message, strlen(message), 0) < 0) {
    close(socket_descriptor);
    printf("Failed to send data to server\n");
    exit(EXIT_FAILURE);
  }
  printf("Data sent successfully!\n");

  // Receive reply from the server
  // Again, we could use read() instead of recv(), just like with files
  if (recv(socket_descriptor, server_reply, 2000, 0) < 0) {
    close(socket_descriptor);
    printf("Failed to read data from server\n");
    exit(EXIT_FAILURE);
  }
  printf("Reply received!\n");
  printf("%s", server_reply);

  close(socket_descriptor);

  return 0;
}
