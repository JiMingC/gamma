LIBS = -lm
CC = gcc
LD = gcc

SUBDIRS = . ./src
SRCS = $(wildcard $(foreach S,$(SUBDIRS), $S/*.c $S/*.cpp))
OBJS = $(SRCS:.c = .o)

INCLUDES = $(foreach S,$(SUBDIRS),-I$S)
CCFAGS = -g -Wall -O0 $(INCLUDES)

TARGET = gamma_test


.PHONY:all clean
all:$(TARGET)
$(TARGET):$(OBJS) $(M_OBJS)
	$(LD) $(CCFLAGS) -o $@ $^ $(LIBS)


/%o:/%c
	$(CC) $(CCFLAGS) -c $^
 
clean:
	rm -rf *.o $(foreach D,$(SUBDIRS),$D*.o) $(TARGET)
