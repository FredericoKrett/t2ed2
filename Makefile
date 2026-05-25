CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -Werror=implicit-function-declaration -Isrc -Itst/unity
LDFLAGS =

PROG = ted

CORE_SRC = src/config.c
MAIN_SRC = src/main.c

.PHONY: all ted test tstall clean

all: ted

ted: $(CORE_SRC) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(CORE_SRC) $(MAIN_SRC) -o $(PROG) $(LDFLAGS)

test: tstall

tstall: test_config
	./test_config

test_config: src/config.c src/config.h tst/test_config.c tst/unity/unity.c
	$(CC) $(CFLAGS) src/config.c tst/test_config.c tst/unity/unity.c -o test_config $(LDFLAGS)

clean:
	rm -f $(PROG) *.o src/*.o tst/*.o test_*
