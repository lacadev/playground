#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Missing explanations might be found in the client.c file

int main(int argc, char *argv[]) {
  int socket_descriptor, new_socket, c;
  struct sockaddr_in server, client;
  char *message;

  // Create socket
  socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor == -1) {
    close(socket_descriptor);
    printf("Could not create socket");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully!\n");

  server.sin_family = AF_INET;
  // INADDR_ANY will bind the socket to all the interfaces
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  // Bind socket to local address(es). Now other sockets can connect to it
  if (bind(socket_descriptor, (struct sockaddr *)& server, sizeof(server)) < 0) {
    close(socket_descriptor);
    printf("Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }
  printf("Socket binded successfully to port!\n");

  // Listen to incoming connections on the socket
  // 2nd argument defines the maximum length to which the queue of
  // pending connections may grow. See man pages for "listen".
  listen(socket_descriptor, 3);
  printf("Waiting for incoming connections...\n");

  // Accept incoming connections
  c = sizeof(struct sockaddr_in);
  new_socket = accept(socket_descriptor, (struct sockaddr *)& client, (socklen_t*)& c);
  if (new_socket < 0) {
    close(socket_descriptor);
    printf("Failed to accept incoming connection\n");
    exit(EXIT_FAILURE);
  }
  char *client_ip = inet_ntoa(client.sin_addr);
  int client_port = ntohs(client.sin_port);
  printf("Accepted incoming connection from %s:%d\n", client_ip, client_port);

  // Reply to connected client
  message = "Hello client! I have received your connection but I have to go... bye!\n";
  write(new_socket, message, strlen(message));



  close(socket_descriptor);

  return 0;
}
