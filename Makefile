# Makefile

CC = g++
CFLAGS = -Wall -O3 -arch x86_64
INCPATH = `pkg-config --cflags opencv`
LFLAGS = -arch x86_64
LIBS = `pkg-config --libs opencv`

SRCDIR = ./src
OBJDIR = ./obj
EXEDIR = ./bin

OBJS = $(OBJDIR)/descriptor.o

TARGET1 = $(EXEDIR)/descriptor

all: $(TARGET1)

$(TARGET1): $(OBJDIR)/descriptor.o
	$(CC) $(LFLAGS) -o $(TARGET1) $^ $(LIBS)

$(OBJDIR)/descriptor.o: $(SRCDIR)/surf.cpp
	$(CC) -D__MAIN__ $(CFLAGS) -c $< -o $@ $(INCPATH)

clean:
	rm -f $(TARGET1) $(OBJS)
