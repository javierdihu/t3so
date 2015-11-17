#ifndef server_h_
#define server_h_

struct config
{
    char port[1024];
    char dir[1024];
};

void dostuff(int);
struct config get_config(char *);
void error(char *);
void leer_comandos(int);
char * get_input(int sock);
int parse_comando(char *);
int parse_argumento(char *);
void run_accion(int);
void send_msg(char *, int);
void set_user(int);
int clean_out(char *, int);
#endif