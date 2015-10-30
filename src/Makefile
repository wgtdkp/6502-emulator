CFLAGS = -Wall
SRCS = cpu6502.c fc.c memory.c debug.c

TARGET = fc

OBJS = $(SRCS:.c=.o)

CC = gcc

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lm

INC = -I./inc

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
	
.PHONY : clean
clean :
	-rm -rf $(TARGET) $(OBJS)
