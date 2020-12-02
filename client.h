#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>

#define PORT                    8080
#define MAX_FILENAME_SIZE       255
#define S_CONNECTION_ERROR      -1

// Petition to the server (waits until server responds with a new seq file)
int request(char *request);

/* Do handshake between client and server */
int open_connection();

#endif