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
void parse_comando(char *);
#endif