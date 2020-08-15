/* Implementation of the tutorial: */
/* https://www.binarytides.com/socket-programming-c-linux-tutorial/ */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int socket_desc;
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1) {
    close(socket_desc);
    printf("Could not create socket");
    exit(EXIT_FAILURE);
  }

  printf("Socket created successfully!\n");
  close(socket_desc);

  return 0;
}
