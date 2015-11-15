CFLAGS = -Wall
SRCS = instruction.c parser.c main.c token.c utility.c symbol.c

TARGET = asmer

OBJS_DIR = build/
OBJS = $(SRCS:.c=.o)

CC = gcc

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lm

INC = -I./inc -I../src

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
	
.PHONY : clean
clean :
	-rm -rf $(TARGET) $(OBJS)
