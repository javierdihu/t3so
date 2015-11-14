#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void dostuff(int);

void error (char *msg){
    perror(msg);
    exit(1);
}

int main (int argc, char *argv[])
{
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
    
    portno=atoi(argv[1]);
    //el numero del puerto donde el servidor va a escuchar
    
    serv_addr.sin_family = AF_INET;
    
    serv_addr.sin_port = htons(portno);
    
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    //La llamada bind() une al socket con la direccion
    
    listen(sockfd,5);
    // listen permite al proceso escuchar al socket. el primero argumento es el descriptor del archivo
    //el segundo argumento es el tamaño del backlog queue
    
    clilen = sizeof(cli_addr);
    while (1){
        
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error ("Error al aceptar");
        pid = fork();
        if(pid<0)
            error("Error de fork");
        if(pid==0){
            close(sockfd);
            while(1)
            {
            dostuff(newsockfd);
            }
            exit(0);
        }
        else close(newsockfd);
    }
    return 0;
    
}

void dostuff (int sock)
{
    int n;
    char buffer[256];
    
    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(sock,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    
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