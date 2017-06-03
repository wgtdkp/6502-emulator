CFLAGS = -Wall
SRCS = ../common/memory.c ../common/instruction.c ../common/utility.c ../common/hex_handler.c cpu6502.c main.c debug.c 

TARGET = fc

OBJS = $(SRCS:.c=.o)

CC = gcc -std=c11

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lm

INC = -I../common
DEBUG = -DDEBUG

%.o : %.c
	$(CC) $(CFLAGS) $(INC) $(DEBUG) -o $@ -c $<
	
.PHONY : clean
clean :
	-rm -rf $(TARGET) $(OBJS)
