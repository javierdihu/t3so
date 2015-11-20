#ifndef server_h_
#define server_h_

struct config
{
    char port[1024];
    char dir[1024];
};

typedef struct archivo
{
    char *name;
    char *owner;
    int size;
    int shared;
} archivo;

struct config get_config(char *);
void error(char *);
void leer_comandos(int);
char * get_input(int sock);
char * get_file(int sock, int size);
int parse_comando(char *);
int parse_argumento(char *);
void run_accion(int);
void send_msg(char *, int);
void set_user(int);
int clean_out(char *, int);
void ls(int sock);
void create_file(char*,char*,int);
void put_file(int);
void run_get(int);
void run_ls(int);
void send_mensaje(char *, int);
void run_close(int);
void run_rm(int);
void run_share(int);

#endif
