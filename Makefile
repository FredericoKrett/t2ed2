CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -Werror=implicit-function-declaration -Isrc -Itst/unity
LDFLAGS =

PROG = ted

CORE_SRC =
MAIN_SRC = src/main.c

.PHONY: all ted test tstall clean

all: ted

ted: $(CORE_SRC) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(CORE_SRC) $(MAIN_SRC) -o $(PROG) $(LDFLAGS)

test: tstall

tstall:
	@echo "Nenhum teste unitario cadastrado ainda."

clean:
	rm -f $(PROG) *.o src/*.o tst/*.o test_*
