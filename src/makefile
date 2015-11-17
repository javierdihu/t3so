HEADERS_SERVER = server.h
HEADERS_CLIENT = cliente.h

HEADERS = HEADERS_CLIENT HEADERS_SERVER

OBJECTS_CLIENT = cliente.o
OBJECTS_SERVER = server.o

CFLAGS = -Wall -g

default: server cliente

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c -o $@ $<

cliente: $(OBJECTS_CLIENT)
	gcc $(CFLAGS) -o $@ $^

server: $(OBJECTS_SERVER)
	gcc $(CFLAGS) -o $@ $^

clean:
	-rm -f $(OBJECTS_CLIENT)
	-rm -f $(OBJECTS_SERVER)
	-rm -f server
	-rm -f cliente



