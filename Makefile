OBJS:=$(patsubst %.c, %.o, $(subst src/,,$(wildcard src/*.c)))
EXEC:=demo

CC:=gcc
CFLAGS:=-std=gnu99
FLAGS:=
INC:=-I include/

all: $(OBJS) $(EXEC)

%.o: src/%.c
	$(CC) $(CFLAGS) $(FLAGS) -c $^ $(INC)

$(EXEC): main.c $(OBJS)
	$(CC) $< $(INC) $(CFLAGS) $(FLAGS) -o $@ $(OBJS)

.PHONY=clean

clean:
	rm -rf $(OBJS) $(EXEC)