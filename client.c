#include "client.h"
 
 // Temporal socket test
void main()
{
    char *filename;
    request(filename);
    printf("New file %s", filename);
    free(filename);
}


int request(char *request) 
{ 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
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
    } 

    char *filename = (char*)malloc(sizeof(char)*MAX_FILENAME_SIZE);
    filename[0] = '0';

    //>>>>>>
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    send(sock , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
    //<<<<<<<

    while(filename[0] == '0')
    {
        // Request
        send(sock,(char*)REQUEST_CMD,sizeof(char),0);
        printf("Request sent\n");

        sleep(3);

        // Recept
        int filename_size = 0;
        int rcount = recv(sock, &filename, MAX_FILENAME_SIZE, 0);

        filename_size = strlen(filename);
    }

    return 0;
} 
