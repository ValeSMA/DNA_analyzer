#include "client.h"
 
int sock = 0, valread; 
struct sockaddr_in serv_addr; 

 // Temporal socket test
// void main()
// {
//     char filename[255];

//     connection();
//     request(filename);
//     printf("New file %s", filename);
// }

/* Do handshake between client and server */
int open_connection()
{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return S_CONNECTION_ERROR; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return S_CONNECTION_ERROR; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return S_CONNECTION_ERROR; 
        exit(1);
    } 

    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    send(sock , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
}

// Petition to the server
int request(char *request) 
{ 
    char buffer[1024] = {0}; 
    valread = read( sock, buffer, 1024);
    strcpy(request,buffer);
    return 0;
} 
