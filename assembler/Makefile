CFLAGS = -Wall
SRCS = ../common/memory.c ../common/instruction.c ../common/utility.c ../common/hex_handler.c parser.c main.c token.c symbol.c codegen.c

TARGET = asmer

OBJS_DIR = build/
OBJS = $(SRCS:.c=.o)

CC = gcc

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lm

INC = -I../common
DEBUG = -DDEBUG

%.o : %.c
	$(CC) $(CFLAGS) $(INC) $(DEBUG) -o $@ -c $<
	
.PHONY : clean
clean :
	-rm -rf $(TARGET) $(OBJS)
