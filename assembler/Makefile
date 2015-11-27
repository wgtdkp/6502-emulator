CFLAGS = -Wall
SRCS = ../common/instruction.c ../common/utility.c  parser.c main.c token.c symbol.c

TARGET = asmer

OBJS_DIR = build/
OBJS = $(SRCS:.c=.o)

CC = gcc

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lm

INC = -I../common

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
	
.PHONY : clean
clean :
	-rm -rf $(TARGET) $(OBJS)
