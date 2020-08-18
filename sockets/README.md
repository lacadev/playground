# Linux Sockets

This is the code I wrote following [this tutorial](https://www.binarytides.com/socket-programming-c-linux-tutorial/), in an attempt to better understand how to use sockets in Linux.

To run client:
```bash
gcc client.c -o client.out && ./client.out
```

To run server:
```bash
gcc server.c -lpthread -o server.out && ./server.out
```
