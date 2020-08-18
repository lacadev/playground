#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

// Missing explanations might be found in the client.c file

void *connection_handler(void *);

int main(int argc, char *argv[]) {
  int socket_descriptor, new_socket, c, *new_sock;
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
  while ( (new_socket = accept(socket_descriptor, (struct sockaddr *)& client, (socklen_t*)& c)) ) {
    char *client_ip = inet_ntoa(client.sin_addr);
    int client_port = ntohs(client.sin_port);
    printf("Accepted incoming connection from %s:%d\n", client_ip, client_port);

    // Reply to connected client
    message = "Hello client! I have received your connection, let me assign you a handle\n";
    write(new_socket, message, strlen(message));

    pthread_t sniffer_thread;
    new_sock = malloc(1);
    *new_sock = new_socket;

    if (pthread_create( &sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
      printf("Failed to create thread to handle connection\n");
    }
    printf("Handler has been successfully assigned\n");
  }
  if (new_socket < 0) {
    close(socket_descriptor);
    printf("Failed to accept incoming connection\n");
    exit(EXIT_FAILURE);
  }

  close(socket_descriptor);

  return 0;
}

// Handle connection for each client socket
void *connection_handler(void *socket_descriptor) {
  // Get the socket descriptor
  int sock = *(int*) socket_descriptor;
  int read_size;
  char *message, client_message[2000];

  // Send messages to client
  message = "Greetings! You have been assigned a connection handler\n";
  write(sock, message, strlen(message));
  message = "I'll repeat whatever you say from now on\n";
  write(sock, message, strlen(message));

  // Wait for replies and repeat them back to client
  while ( (read_size = recv(sock, client_message, 2000, 0)) > 0) {
    write(sock, client_message, strlen(client_message));
  }
  if (read_size == 0) {
    printf("Client disconnected\n");
    fflush(stdout);
  }
  else if (read_size == -1) {
    printf("Failed to read from client\n");
  }

  // Free socket pointer
  free(socket_descriptor);

  return 0;
}
