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
    int last_cmd = 0;
    int arg_1_enviado = 0;
    int buff_size_put = 0;
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
        if(last_cmd == 2){
            printf("C: ");
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0)
                error("no se pudo escribir al socket");
            
            if(!arg_1_enviado){
                /* guardar valor ingresado */
                arg_1_enviado = 1;
            }
            else{
                buff_size_put = atoi(buffer);
                arg_1_enviado = 0;
                last_cmd = 0;
                
                /* enviar archivo con buffer tamaño nuevo*/
                put(sockfd, buff_size_put);
            }
            
            continue;
        }
            
        printf("C: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        //printf("[+] CLIENTE ENVIA MSJ : %s[+]\n", buffer);
        
        
        /* escribir en el socket */
        //printf("[+] ESCRIBIENDO %s\n", buffer);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("no se pudo escribir al socket");
        
        
        /* leer del socket despues que hayamos mandado END y hasta que 
            hayamos recibido END
         */
        if(check_end(buffer)){
            bzero(buffer,256);
            n = read(sockfd, buffer, 255);
                
            if (n < 0)
                error("error leyendo del socket");
                
            /* parsear el mensaje del server, aca se maneja que
                el server haya mandando END
             */
            parse_msj(buffer);
        }
        last_cmd = check_cmd(buffer);
    }
    
    return 0;
}

void put(int sock, int buff_size){
    int n;
    char buffer[buff_size];
    bzero(buffer, buff_size);
    fgets(buffer, buff_size - 1, stdin);
    
    n = write(sock, buffer, strlen(buffer));
    if (n < 0)
        error("no se pudo escribir al socket");

}

int parse_msj(char *buff){
    char buff2[256];
    int seguir = 1;
    int jj = 0;
    int ret = 0;
    
    
    while(seguir){
        int ii;
        memset(buff2, '\0', 256);
        
        for(ii = 0; ii < 256 - jj; ii++){
            buff2[ii] = buff[ii + jj];
            if(buff[ii + jj] == '#'){
                if(buff[ii + jj + 1] == '\0')
                    seguir = 0;
                jj = ii + jj + 1;
                buff2[ii] = '\n';
                if(check_end(buff2)){
                    ret = 1;
                }
                break;
            }
        }
        printf("S: %s\n", buff2);
    }
    
    return ret;
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

int check_cmd(char *cmd){
    if(!strcmp(cmd, "USER")){
        return 1;
    }
    else if(!strcmp(cmd, "PUT")){
        return 2;
    }
    else if(!strcmp(cmd, "GET")){
        return 3;
    }
    else if(!strcmp(cmd, "LS")){
        return 4;
    }
    else if(!strcmp(cmd, "RM")){
        return 5;
    }
    else if(!strcmp(cmd, "SHARE")){
        return 6;
    }
    else if(!strcmp(cmd, "CLOSE")){
        return 7;
    }
    else{
        return 0;
    }
    
    return 1;
}

int check_end_server(char *buffer){
    int i;
    char end[3] = {'E','N','D'};
    for(i = 0; i < 3; i++){
        if(buffer[i] != end[i])
            return 0;
    }
    return 1;
}

