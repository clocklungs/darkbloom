#warnings and strictness
CFLAGS=-Wall -pedantic

#sdl flags
CFLAGS+=$(shell sdl-config --cflags)

CXXFLAGS= $(CFLAGS)

#sdl libs
LIBS=$(shell sdl-config --libs) -lSDL_image -lSDL_mixer

#c compiler
CC=gcc
CXX=g++

#objects
SRCS=$(wildcard src/*.cc)
OBJS=$(SRCS:.cc=.o)

#targets
TARGET=darkbloom

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

.PHONY: clean dist

clean:
	@rm -f src/*.o *~ $(TARGET) $(TARGET).exe stdout.txt stderr.txt

dist:
	@rm -f *.o *~ stderr.txt stdout.txt
	@strip $(TARGET) $(TARGET).exe
