#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>

#define MAX_EVENTS 1024  // Maximum number of events to process
#define LEN_NAME 16  // Assuming that the length of the filename won't exceed 16 bytes
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) //size of one event
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME )) // buffer to store the data of events
#define MAX_SUB 20 // Maximum number of subdirectories 

int fd, wd[MAX_SUB];

static void daemonize()
{
    pid_t pid;

    pid = fork(); // Copiar y terminar proceso padre
    
    if (pid < 0) //Si el resultado del fork es menor a 0, entonces hay error
        exit(EXIT_FAILURE);
    
    if (pid > 0) /// Si el resultado es mayor a 0, dejar que el proceso padre termine
        exit(EXIT_SUCCESS);
    
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    
    //Señales
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    pid = fork(); //Se hace de nuevo el fork y se checa por error
    
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    umask(027); //Establecer los permisos del proceso
    
    chdir("."); //Cambiar el directorio (comúnmente por root pero esta vez en esta carpeta)
    
    //Cerrar los descriptores de archivo
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    openlog ("firstdaemon", LOG_PID, LOG_DAEMON); //Abrir el archivo log
}

int main()
{
    printf("Starting daemonize\n");
    daemonize();

    //SOCKET
	int listen_fd = 0, conn_fd = 0;
    struct sockaddr_in server, client;
    char sendBuffer[100];

    int dirr = 1;
    char *path_to_be_watched[MAX_SUB];
    path_to_be_watched[0] = "./Experimentos";

    FILE *fp= NULL;
    fp = fopen ("Log.txt", "w+");
         

    // Step 1. Initialize inotify
    fd = inotify_init();

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)  // error checking for fcntl
        exit(2);

    // Step 2. Add Watch
    wd[0] = inotify_add_watch(fd, path_to_be_watched[0], IN_CREATE | IN_MOVED_TO);
    if(wd[0] == -1){
            fprintf(fp, "Could not watch : %s\n", path_to_be_watched[0]);
    }
    else{
            fprintf(fp, "Watching : %s\n", path_to_be_watched[0]);
    }

    //Create a socket
	if((listen_fd = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
	{
		fprintf(fp, "Could not create socket\n");
        return -1;
	}

	fprintf(fp, "Socket created.\n");
    memset(&server, '0', sizeof(server));
    memset(sendBuffer, '0', sizeof(sendBuffer));
	
	
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

    bind(listen_fd, (struct sockaddr*)&server, sizeof(server));

	listen(listen_fd, 3); 
    
    if (fp != NULL){
        while (1)
        {
            
            //TODO: Insert daemon code here.
            syslog (LOG_NOTICE, "First daemon running.");
            conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
            fprintf(fp, "New socket connected");

            int i=0,length;
            char buffer[BUF_LEN];
 
            // Step 3. Read buffer
            length = read(fd,buffer,BUF_LEN);
 
            // Step 4. Process the events which has occurred 
            while(i<length){
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
                if(event->len){
                    if ( event->mask & (IN_CREATE | IN_MOVED_TO)) {
                        if ( event->mask & IN_ISDIR ) {
                            fprintf(fp, "The directory %s was created.\n", event->name );
                            
                        }
                        else {
                            const char *dot = strrchr(event->name, '.');
                            int comp = strcmp(dot + 1, "seq");
                            if(comp == 0) {
                                fprintf(fp, "The file %s was created.\n", event->name );
                                if(send(conn_fd , event->name , strlen(event->name), 0) < 0)
                                {
                                    fprintf(fp, "Send failed\n");
                                    return 1;
                                }
                                fprintf(fp, "Data Send\n");
                            }
                        }
                    }/*
                    else if ( event->mask & IN_DELETE ) {
                        if ( event->mask & IN_ISDIR ) {
                            fprintf(fp, "The directory %s was deleted.\n", event->name );
                        }
                        else {
                            fprintf(fp, "The file %s was deleted.\n", event->name );
                        }
                    }
                    else if ( event->mask & IN_MODIFY ) {
                        if ( event->mask & IN_ISDIR ) {
                            fprintf(fp, "The directory %s was modified.\n", event->name );
                        }   
                        else {
                            fprintf(fp, "The file %s was modified.\n", event->name );
                        }
                    }*/
                }
                i += EVENT_SIZE + event->len;
            }

            fflush(fp);
        }
        
        for(int i = 0; i < dirr; i++)
            inotify_rm_watch(fd, wd[i]);
        close( fd );
        exit( 0 );
        
    }
    fclose(fp);
    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();
    
    return EXIT_SUCCESS;
}
