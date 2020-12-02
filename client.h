#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#define PORT                    8080
#define MAX_FILENAME_SIZE       255
#define REQUEST_CMD             '1'
#define S_CONNECTION_ERROR      -1

