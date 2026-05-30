CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -Werror=implicit-function-declaration -Isrc -Itst/unity
LDFLAGS =

PROG = ted

CORE_SRC = src/config.c src/quadra.c src/quadra_store.c src/geo_parser.c src/grafo.c
MAIN_SRC = src/main.c

.PHONY: all ted test tstall clean

all: ted

ted: $(CORE_SRC) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(CORE_SRC) $(MAIN_SRC) -o $(PROG) $(LDFLAGS)

test: tstall

tstall: test_config test_quadra test_quadra_store test_geo_parser test_grafo
	./test_config
	./test_quadra
	./test_quadra_store
	./test_geo_parser
	./test_grafo

test_config: src/config.c src/config.h tst/test_config.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/config.c tst/test_config.c tst/unity/unity.c -o test_config $(LDFLAGS)

test_quadra: src/quadra.c src/quadra.h tst/test_quadra.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/quadra.c tst/test_quadra.c tst/unity/unity.c -o test_quadra $(LDFLAGS)

test_quadra_store: src/quadra_store.c src/quadra_store.h src/quadra.c src/quadra.h tst/test_quadra_store.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/quadra_store.c src/quadra.c tst/test_quadra_store.c tst/unity/unity.c -o test_quadra_store $(LDFLAGS)

test_geo_parser: src/geo_parser.c src/geo_parser.h src/quadra_store.c src/quadra_store.h src/quadra.c src/quadra.h tst/test_geo_parser.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/geo_parser.c src/quadra_store.c src/quadra.c tst/test_geo_parser.c tst/unity/unity.c -o test_geo_parser $(LDFLAGS)

test_grafo: src/grafo.c src/grafo.h tst/test_grafo.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/grafo.c tst/test_grafo.c tst/unity/unity.c -o test_grafo $(LDFLAGS)

clean:
	rm -f $(PROG) *.o src/*.o tst/*.o test_*
