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
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "server.h"

#define FILENAME "fileserver.conf"
#define MAXBUF 1024
#define DELIM "="

struct config configurasound;
archivo *archivos;
int last_cmd;
/* liberar last_arg despues de usarlo */
char * last_arg;
char * arg_1;
char * arg_2;
char * user;
int file_cnt;
char *new_file;


int main (int argc, char *argv[])
{
    file_cnt = 32;
    last_cmd = 0;
    user = malloc(sizeof(char)*256);
    archivos = malloc(sizeof(archivo)*32);
    int iii;
    for(iii = 0; iii < file_cnt; iii++){
        archivos[iii].name = NULL;
        archivos[iii].owner = NULL;
        archivos[iii].shared = -1;
    }
    
    
    memset(user, '\0', 256);
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
    
    printf("[*] ESPERANDO COMANDO\n");
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
            
            printf("[*] ESPERANDO ARGUMENTO\n");
            input = get_input(sock);
            success_arg = parse_argumento(input);
            free(input);
            printf("---viendo que wea el argumento\n");
            if(success_arg){
                arg_1 = last_arg;
                /* esperar END si el argumento era valido */
                printf("[*] ESPERANDO END\n");
                input = get_input(sock);
                if(!strcmp(input, "END")){
                    run_accion(sock);
                }
                free(input);
            }
            else{
                printf("[*]FALLO LECTURA ARGUMENTO\n");
            }
        }
        else if(last_cmd == 2){
            /* manejar PUT */
            input = get_input(sock);
            success_arg = parse_argumento(input);
            free(input);
            if(success_arg){
                arg_1 = last_arg;
                printf("ARG 1 LISTO: %s\n", arg_1);
                input = get_input(sock);
                success_arg = parse_argumento(input);
                free(input);
                if(success_arg){
                    arg_2 = last_arg;
                    printf("ARG 2 LISTO: %s\n", arg_2);
                    /* con el argumento 2 sabemos el tamaño del buffer
                        que necesitamos para leer el archivo
                     */
                    int size = atoi(arg_2);
                    new_file = get_file(sock, size);
                    
                    printf("falta el END\n");
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
            put_file(sock);
            break;
        case 3:
            break;
        case 4:
            ls(sock);
            break;
            
    }
}

void set_user(int sock){
    memset(user, '\0', 256);
    strcpy(user, arg_1);
    
    /*
        Manda el output a lo bruto, concatena todo el mensaje
        asi: Ok#User identified as: user#END#
     
     */
    char out[256];
    memset(out, '\0', 256);
    char *end = "END#\0";
    char *msj = "Ok#User identified as: \0";
    int i, j ,k;
    for(i = 0; i < 256; i++){
        out[i] = msj[i];
        if(msj[i] == '\0')
            break;
    }
    
    for(k = 0; k < strlen(user); k++){
        out[k + i] = user[k];
    }
    i = i + k;
    out[i] = '#';
    i+=1;
    for(j = 0; j < 4; j++){
        out[i + j] = end[j];
    }
    
    send_msg(out, sock);
    
    free(arg_1);
}

void ls(int sock){
    
}

void send_msg(char *msg, int sock){
    int n;
    
    printf("[*] SE INTENTA MANDAR: %s\n", msg);
    
    n = write(sock, msg, strlen(msg));
    if (n < 0) error ("ERROR writing to socket");
    
    printf("%d bytes enviados al cliente\n", n);
    return;
}


int clean_out(char *out, int n){
    int i;
    for(i = 0; i < n; i++){
        if(out[i] == '\0'){
            return i;
        }
    }
    return n;
}

void create_file(char *buff, char *name, int size){
    int i;
    int listo = 0;
    for(i = 0; i < file_cnt; i++){
        if(archivos[i].shared == -1){
            archivos[i].name = name;
            archivos[i].owner = user;
            archivos[i].shared = 1;
            listo = 1;
        }
    }
    /* manejar caso de que ya esten llena la lista de archivos */
    if(!listo){
        
    }
    
    FILE *fp;
    fp = fopen(name, "w");
    fwrite(buff, 1, size, fp);
    fclose(fp);
    
    free(buff);
    return;
}

void put_file(int sock){
    create_file(new_file, arg_1, atoi(arg_2));
    free(arg_2);
    
    
    char out[256];
    memset(out, '\0', 256);
    char *end = "END#\0";
    char *msj = "Ok#File saved as: \0";
    int i, j ,k;
    for(i = 0; i < 256; i++){
        out[i] = msj[i];
        if(msj[i] == '\0')
            break;
    }
    
    for(k = 0; k < strlen(arg_1); k++){
        out[k + i] = arg_1[k];
    }
    i = i + k;
    out[i] = '#';
    i+=1;
    for(j = 0; j < 4; j++){
        out[i + j] = end[j];
    }
    
    send_msg(out, sock);
    
}

int parse_argumento(char *input){
    printf("[*] PARSEANDO ARGUMENTO: %s\n", input);
    char *arg;
    
    int i, j, k, n;
    k = 0;
    j = 0;
    for(i = 0; i < 256; i++){
        if(input[i] == ':'){
            j = i + 2;
        }
        if(input[i] == '\0')
            k = i;
    }
    if(j == 0){
        printf("NO SE ENCONTRO EL DELIMITADOR\n");
        return 0;
    }
    n = k - j + 1;
    arg = malloc(sizeof(char) * n);
    
    int ii;
    for(ii = 0; ii < n; ii++){
        arg[ii] = input[ii+j];
        
    }
    last_arg = arg;
    printf("ARGUMENTO PARSIADO: %s\n", arg);
    return 1;
    
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

char *get_file(int sock, int size){
    printf("ESPERANDO ARCHIVO TAMAÑO %d\n", size);
    int n;
    char *buffer = malloc(sizeof(char) * (size + 1));
    
    bzero(buffer, (size + 1));
    n = read(sock, buffer, size + 1);
    
    if(n < 0)
        error("ERROR leyendo del socket");
    
    buffer[n - 1] = '\0';
    printf("[*] LEIDO archivo : %s [*]\n", buffer);
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
