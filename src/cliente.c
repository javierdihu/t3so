#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "cliente.h"

void error(char *msg)
{
    printf("ERROR: %s\n", msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[256];
    if (argc < 3) {
        error("uso: serverName port");
        exit(0);
    }
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0)
        error("no se pudo abrir el socket");
    
    server = gethostbyname(argv[1]);
    
    if (server == NULL) {
        error("servidor no existe");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("conexion fallo");
    
    /* aca empieza a tomar input */
    
    while(1){
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        printf("[+] CLIENTE ENVIA MSJ : %s[+]\n", buffer);
        
        
        /* escribir en el socket */
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("no se pudo escribir al socket");
        
        
        /* leer del socket despues que hayamos mandado END y hasta que 
            hayamos recibido END
         */
        if(check_end(buffer)){
            while(1){
                bzero(buffer,256);
                n = read(sockfd, buffer, 255);
                
                if (n < 0)
                    error("error leyendo del socket");
                
                printf("[+] CLIENTE RECIBIO: %s [+]\n",buffer);
                if(check_end(buffer))
                    break;
                
                
            }
            
            
            
            /*
            bzero(buffer,256);
            n = read(sockfd, buffer, 255);
            
            if (n < 0)
                error("error leyendo del socket");
            
            printf("[+] CLIENTE RECIBIO: %s [+]\n",buffer);
            */
            
            /* hasta aqui hemos recibido el primer mensaje que puede
                ser OK o FAIL 
             */
            
            /* seguimos recibiendo mensajes */
            
            /*
            while(!check_end(buffer)){
                bzero(buffer,256);
                n = read(sockfd, buffer, 255);
                
                if (n < 0)
                    error("error leyendo del socket");
            
                printf("[+] CLIENTE RECIBIO: %s [+]\n",buffer);
            }
             */
            printf("[+] SALIO DEL LOOP\n");
        }
    
    }
    
    return 0;
}

int check_end(char *buffer){
    int i;
    char end[4] = {'E','N','D','\n'};
    for(i = 0; i < 4; i++){
        if(buffer[i] != end[i])
            return 0;
    }
    return 1;
}


