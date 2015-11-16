/*
    Tarea 3
    Daniel Fleiderman
    Javier Diaz
 
    Paginas usadas
        https://www.pacificsimplicity.ca/blog/simple-read-configuration-file-struct-example
        http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"

#define FILENAME "fileserver.conf"
#define MAXBUF 1024
#define DELIM "="

struct config configurasound;
char *last_cmd;


int main (int argc, char *argv[])
{
    /* leer configuracion */
    configurasound = get_config(FILENAME);
    
    
    int sockfd, newsockfd, portno, clilen, n, pid;
    //sockfd y newsockfd son arreglos bla bla
    //portno guarda el numero del puerto
    //clilen guarda el tamaño de la direccion del cliente
    //n es el valor de retorno para las llamadas de read y write. Contiene el numero de caracteres read o write
    char buffer[256];
    
    struct sockaddr_in serv_addr, cli_addr;
    //sockaddr_in es una estrictura que contiene la direccion de internet. Esta definida en <netinet/in.h>
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//AF_UNIX
    if(sockfd < 0)
        error("ERROR abiendo socket");
    //socket() el primer argumento es la direccion de dominio del socket.
    //ojo con los dos tipos de direccion de dominio
    //El segundo argumento es el tipo de socket. Puede ser stream o datagram SOCK_DGRAM
    //El tercer argumento es el protocolo
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //setea todos los valores en el buffer a cero.
    
    portno = atoi(configurasound.port);
    //el numero del puerto donde el servidor va a escuchar
    
    serv_addr.sin_family = AF_INET;
    
    serv_addr.sin_port = htons(portno);
    
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    //La llamada bind() une al socket con la direccion
    
    listen(sockfd,5);
    printf("Escuchando en el puerto %d\n", atoi(configurasound.port));
    // listen permite al proceso escuchar al socket. el primero argumento es el descriptor del archivo
    //el segundo argumento es el tamaño del backlog queue
    
    clilen = sizeof(cli_addr);
    while (1){
        
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error ("Error al aceptar");
        pid = fork();
        if(pid < 0)
            error("Error de fork");
        if(pid == 0){
            close(sockfd);
            
            while(1){
                /* recibir comandos */
                leer_comandos(newsockfd);
            }
            exit(0);
        }
        else close(newsockfd);
    }
    return 0;
    
}

struct config get_config(char *filename)
{
    struct config configstruct;
    FILE *file = fopen (filename, "r");
    
    if (file != NULL)
    {
        char line[MAXBUF];
        int i = 0;
        
        while(fgets(line, sizeof(line), file) != NULL)
        {
            char *cfline;
            cfline = strstr((char *)line,DELIM);
            cfline = cfline + strlen(DELIM);
            
            switch (i){
                case 0:
                    memcpy(configstruct.port,cfline,strlen(cfline));
                    break;
                case 1:
                    memcpy(configstruct.dir,cfline,strlen(cfline));
                    break;
            }
            
            i++;
        }
        fclose(file);
    }
    return configstruct;
}


void error (char *msg){
    perror(msg);
    exit(1);
}

void leer_comandos(int sock){
    int n;
    int buffer_size = 256;
    char buffer[buffer_size];
    bzero(buffer, buffer_size);
    
    n = read(sock, buffer, buffer_size - 1);
    if(n < 0)
        error("ERROR leyendo del socket");
    printf("se leyeron %d bytes\n", n);
    buffer[n - 1] = '\0';
    
    /* parsear comando */
    parse_comando(buffer);
    
    
    
    
    
    n = write(sock,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
}

void parse_comando(char *cmd){
    printf("comparando %s con USER", cmd);
    if(!strcmp(cmd, "USER")){
        
    }
    else if(!strcmp(cmd, "PUT")){
        
    }
    else if(!strcmp(cmd, "GET")){
        
    }
    else if(!strcmp(cmd, "LS")){
        
    }
    else if(!strcmp(cmd, "RM")){
        
    }
    else if(!strcmp(cmd, "SHARE")){
        
    }
    else if(!strcmp(cmd, "CLOSE")){
        
    }
    else
        error("COMANDO INVALIDO");
    
}


void dostuff (int sock)
{
    int n;
    char buffer[256];
    
    bzero(buffer,256);
    n = read(sock,buffer,255);
    
    if (n < 0) error("ERROR reading from socket");
    
    printf("Here is the message: %s\n",buffer);
    /* respuesta */
    n = write(sock,"I got your message",18);
    
    if (n < 0) error("ERROR writing to socket");
    
    
    /* ejemplo ls */
    FILE *fp;
    char path[1035];
    char ans[4000];
    fp = popen("/bin/ls", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }
    while (fgets(path, sizeof(path)-1, fp) != NULL)
    {
        printf("%s", path);
        strcat(ans, path);
    }
    
    pclose(fp);
}