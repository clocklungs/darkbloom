#warnings and strictness
CFLAGS=-Wall -pedantic -std=c++11

#sdl flags
CFLAGS+=$(shell sdl2-config --cflags)

CXXFLAGS= $(CFLAGS)

#sdl libs
LIBS=$(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer

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
