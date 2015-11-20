#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "cliente.h"

int last_cmd;

void error(char *msg)
{
    printf("ERROR: %s\n", msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    last_cmd = 0;
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
        /* PUT */
        if(last_cmd == 2){
            printf("C: ");
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            //printf("[put] CLIENTE ENVIA MSJ : %s", buffer); 
            
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0)
                error("no se pudo escribir al socket");
            
            if(!arg_1_enviado){
                /* guardar valor ingresado */
                arg_1_enviado = 1;
                last_cmd = 2;
            }
            else{
                newline_to_zero(buffer);
                parse_argumento(buffer);
                buff_size_put = atoi(buffer);
                arg_1_enviado = 0;
                last_cmd = 8;
                
                /* enviar archivo con buffer tamaño nuevo*/
                put(sockfd, buff_size_put);
            }
            
            continue;
        }
            
        printf("C: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        //printf("[+] CLIENTE ENVIA MSJ : %s", buffer); 
        
        /* escribir en el socket */
        //printf("[+] ESCRIBIENDO %s\n", buffer);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("no se pudo escribir al socket");
        
        
        /* leer del socket despues que hayamos mandado END y hasta que 
            hayamos recibido END
         */

        if(check_end(buffer)){
            
        /* GET */
            if(last_cmd == 3){
                int size;
                /* se recibe ok o fail*/
                bzero(buffer, 256);
                n = read(sockfd, buffer, 256);
                printf("[S: %s\n", buffer);
                if(!strcmp("FAIL", buffer)){
                    while(1){
                        memset(buffer,'\0', 256);
                        n = read(sockfd, buffer, 256);
                        if (n < 0)
                            printf("ERROR AL LEER MENSAJES DE ERROR\n");
                        printf("S: ");
                        printf("%s\n", buffer);
                        if(check_end_server(buffer))
                            break;

                    }
                    last_cmd = 0;
                    continue;
                }
                if(!strcmp("OK", buffer)){
                    
                    /* hay que recibir el tamaño del archivo */
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 256);
                    printf("[GET] recibido: size = %s\n", buffer);
                    parse_argumento(buffer);
                    size = atoi(buffer);
                    printf("[GET] tamaño guardado: %d\n", size);
                    /* preparar el buffer para recibir el archivo*/
                    char buff_file[size];
                    n = read(sockfd, buff_file, size);
                

                    FILE *fp;
                    fp = fopen("nombrefijo.txt", "w");
                    fwrite(buff_file, 1, size, fp);
                    fclose(fp);
                    printf("archivo creado!\n");
                    last_cmd = 0;
                    continue;
                }
                
            }
            if(last_cmd == 4){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if (n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);
                    
                    if(check_end_server(buffer))
                        break;
                }
                last_cmd = 0;
                continue;
            }

            if(last_cmd == 5){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if (n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);
                    
                    if(check_end_server(buffer))
                        break;
                }
                last_cmd = 0;
                continue;
            }

            if(last_cmd == 6){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if (n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);
                    
                    if(check_end_server(buffer))
                        break;
                }
                last_cmd = 0;
                continue;
            }
            if(last_cmd == 7){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if(n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);

                    if(check_end_server(buffer)){
                        close(sockfd);
                        printf("TERMINANDO CONEXION Y CERRANDO\n");
                        exit(0);
                    }
                }
            }
            if(last_cmd == 1){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if(n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);

                    if(check_end_server(buffer)){
                        break;
                    }
                }
                last_cmd = 0;
                continue;
            }
            if(last_cmd == 8){
                while(1){
                    memset(buffer, '\0', 256);
                    n = read(sockfd, buffer, 256);
                    if(n < 0)
                        printf("ERROR AL LEER MENSAJE\n");
                    printf("S: %s\n", buffer);

                    if(check_end_server(buffer)){
                        break;
                    }
                }
                last_cmd = 0;
                continue;
            }
            else{
                /* esto procesa USER */
                bzero(buffer,256);
                n = read(sockfd, buffer, 255);
                
                if (n < 0)
                    error("error leyendo del socket");
                
                /* parsear el mensaje del server, aca se maneja que
                    el server haya mandando END
                */
                parse_msj(buffer);
            }
        }
        check_cmd(buffer);
    }
    
    return 0;
}

void put(int sock, int buff_size){
    printf("[PUT] enviando archivo tamaño %d\n", buff_size);
    int n;
    char buffer[buff_size];
    bzero(buffer, buff_size);
    fgets(buffer, buff_size + 1, stdin);
    printf("[PUT] se envio: %s\n", buffer);
    
    n = write(sock, buffer, strlen(buffer));
    if (n < 0)
        error("no se pudo escribir al socket");

}

void newline_to_zero(char *buff){
    int i;
    int n = strlen(buff);
    for(i = 0; i < n; i++){
        if(buff[i] == '\n'){
            buff[i] = '\0';
            return;
        }
    }
}


void parse_argumento(char *input){
    //printf("[*] PARSEANDO ARGUMENTO: %s\n", input);
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
        return;
    }
    n = k - j + 1;
    arg = malloc(sizeof(char) * n);
    
    int ii;
    for(ii = 0; ii < n; ii++){
        arg[ii] = input[ii+j];
        
    }
    bzero(input, 256);
    strcpy(input, arg);
    free(arg);
    //printf("ARGUMENTO PARSIADO: %s\n", input);
    
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
        printf("WAT: %s\n", buff2);
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

void check_cmd(char *cmd){
    if(!strcmp(cmd, "USER\n")){
        last_cmd = 1;
    }
    else if(!strcmp(cmd, "PUT\n")){
        last_cmd = 2;
    }
    else if(!strcmp(cmd, "GET\n")){
        last_cmd = 3;
    }
    else if(!strcmp(cmd, "LS\n")){
        last_cmd = 4;
    }
    else if(!strcmp(cmd, "RM\n")){
        last_cmd = 5;
    }
    else if(!strcmp(cmd, "SHARE\n")){
        last_cmd = 6;
    }
    else if(!strcmp(cmd, "CLOSE\n")){
        last_cmd = 7;
    }
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

