#ifndef cliente_h
#define cliente_h

int check_end(char *);
int check_end_server(char *);
int check_cmd(char *);
int parse_msj(char *);
void put(int sock, int buff_size);
#endif /* cliente_h */
