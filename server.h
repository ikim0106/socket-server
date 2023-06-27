#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define IP_ADDRESS "127.0.0.1"
#define PORT_NUMBER 8080
#define TRUE 1
#define FALSE 0
#define REQBUFF_SIZE 2048
#define FILEBUFF_SIZE 1
#define INDEX "/index.html"
