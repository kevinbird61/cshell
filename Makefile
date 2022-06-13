OBJS:=$(patsubst %.c, %.o, $(wildcard *.c))
TEST_PREFIX:=examples/demo_
TESTS:=$(patsubst examples/%.c, $(TEST_PREFIX)%, $(wildcard examples/*.c))
RELEASE:=libcshell.a

CC:=gcc
CFLAGS:=-std=gnu99 -static
FLAGS:=
INC:=-I .

all: $(RELEASE) $(TESTS)
release: $(RELEASE)

%.o: src/%.c
	$(CC) $(CFLAGS) $(FLAGS) -c $^ $(INC)

$(TEST_PREFIX)%: examples/%.c $(RELEASE)
	$(CC) $< $(INC) $(CFLAGS) $(FLAGS) -o $@ -L. -lcshell

$(RELEASE): $(OBJS)
	ar rcs $(RELEASE) $(OBJS)
	ranlib $(RELEASE)

.PHONY=clean

clean:
	rm -rf $(OBJS) $(EXEC) $(RELEASE)
	rm $(TEST_PREFIX)*
