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
int last_cmd;
/* liberar last_arg despues de usarlo */
char * last_arg;
char * arg_1;
char * arg_2;
char * user;


int main (int argc, char *argv[])
{
    last_cmd = 0;
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
    int success_cmd = 0;
    int success_arg = 0;
    char *args;
   
    char *input = get_input(sock);
    
    /* parsear comando, retorna 1 si el comando era valido */
    success_cmd = parse_comando(input);
    if(success_cmd)
        printf("comando valido\n");
    free(input);
    
    /* leer argumentos si el comando anterior fue valido */
    if(success_cmd){
        if(last_cmd == 1 ||
           last_cmd == 3 ||
           last_cmd == 5 ||
           last_cmd == 6){
            
            input = get_input(sock);
            success_arg = parse_argumento(input);
            free(input);
            if(success_arg){
                arg_1 = last_arg;
                /* esperar END si el argumento era valido */
                input = get_input(sock);
                if(!strcmp(input, "END")){
                    run_accion(sock);
                }
                free(input);
            }
        }
        else if(last_cmd == 2){
            input = get_input(sock);
            success_arg = parse_argumento(input);
            free(input);
            if(success_arg){
                arg_1 = last_arg;
                input = get_input(sock);
                success_arg = parse_argumento(input);
                free(input);
                if(success_arg){
                    arg_2 = last_arg;
                    /* tenemos los dos argumentos listos, esperar END */
                    input = get_input(sock);
                    if(!strcmp(input, "END")){
                        run_accion(sock);
                    }
                    free(input);
                }
            }
            
        }
        /* manejar caso de ls y close que no reciben argumentos*/
        else{
            input = get_input(sock);
            if(!strcmp(input, "END")){
                run_accion(sock);
            }
            free(input);
        }
        
    }
    
}

void run_accion(int sock){
    switch(last_cmd){
        case 1:
            set_user(sock);
            break;
        case 2:
            break;
            
    }
}

void set_user(int sock){
    int n;
    int j;
    user = arg_1;
    char out[300] = "User identified as ";
    char out2[3] = "OK\n";
    char out3[4] = "END\n";
    strcat(out, user);
    j = clean_out(out, 300);
    
    /* enviar mensaje diciendo que el usuario se puso */
    
    n = write(sock,out2,3);
    if (n < 0) error("ERROR writing to socket");
    printf("%d bytes enviados al cliente\n", n);
    
    n = write(sock, out,j);
    if (n < 0) error("ERROR writing to socket");
    printf("%d bytes enviados al cliente\n", n);
    
    n = write(sock, "\n",1);
    if (n < 0) error("ERROR writing to socket");
    printf("%d bytes enviados al cliente\n", n);
    
    n = write(sock,out3,4);
    if (n < 0) error("ERROR writing to socket");
    printf("%d bytes enviados al cliente\n", n);
    
    
}

int clean_out(char *out, int n){
    int i;
    for(i = 0; i < n; i++){
        if(out[i] == '\0')
            return i;
    }
    return n;
}

int parse_argumento(char *input){
    char delim[2] = {':', ' '};
    char arg_name[5] = {'0','0','0','0','\0'};
    char *arg = malloc(sizeof(char) * 256);
    char *temp;
    
    if(strlen(input) >= 4){
        memcpy(arg_name, input, 4);
        if(!strcmp(arg_name, "Name")){
            temp = strstr(input, delim);
            temp = temp + 2;
            memcpy(arg, temp, strlen(temp));
            last_arg = arg;
            return 1;
        }
        
    }
    return 0;
}

char *get_input(int sock){
    int n;
    int buffer_size = 256;
    char *buffer = malloc(sizeof(char) * buffer_size);
    
    
    bzero(buffer, buffer_size);
    n = read(sock, buffer, buffer_size - 1);
    
    if(n < 0)
        error("ERROR leyendo del socket");
    
    buffer[n - 1] = '\0';
    printf("[*] LEIDO : %s [*]\n", buffer);
    return buffer;
}

int parse_comando(char *cmd){
    if(!strcmp(cmd, "USER")){
        last_cmd = 1;
    }
    else if(!strcmp(cmd, "PUT")){
        last_cmd = 2;
    }
    else if(!strcmp(cmd, "GET")){
        last_cmd = 3;
    }
    else if(!strcmp(cmd, "LS")){
        last_cmd = 4;
    }
    else if(!strcmp(cmd, "RM")){
        last_cmd = 5;
    }
    else if(!strcmp(cmd, "SHARE")){
        last_cmd = 6;
    }
    else if(!strcmp(cmd, "CLOSE")){
        last_cmd = 7;
    }
    else{
        error("COMANDO INVALIDO");
        return 0;
    }
    
    return 1;
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