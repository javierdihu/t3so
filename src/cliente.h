#ifndef cliente_h
#define cliente_h

int check_end(char *);
int check_end_server(char *);
void check_cmd(char *);
int parse_msj(char *);
void put(int sock, int buff_size);
void newline_to_zero(char *);
void parse_argumento(char *);
#endif /* cliente_h */
