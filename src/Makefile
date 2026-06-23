CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -Werror=implicit-function-declaration -I.
LDFLAGS =

PROG = ted
SRC = config.c quadra.c quadra_store.c geo_parser.c grafo.c via_parser.c registradores.c fila_prioridade.c caminho.c qry_parser.c qry_executor.c svg.c main.c
HEADERS = config.h quadra.h quadra_store.h geo_parser.h grafo.h via_parser.h registradores.h fila_prioridade.h caminho.h qry_parser.h qry_executor.h svg.h

.PHONY: ted clean

ted: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)

clean:
	rm -f $(PROG) *.o
