#warnings and strictness
CFLAGS=-Wall -ansi -pedantic

#sdl flags
CFLAGS+=$(shell sdl-config --cflags)

CXXFLAGS= $(CFLAGS)

#sdl libs
LIBS=$(shell sdl-config --libs) -lSDL_image -lSDL_mixer

#c compiler
CC=gcc
CXX=g++

#objects
OBJS=  video.o font.o menus.o config.o game.o sound.o button.o intro.o
OBJS+= dialog.o var.o dbox.o world.o plyr.o sidebar.o object.o
OBJS+= datatypes.o pathfinding.o script.o battle.o enemy.o path.o

#targets
TARGET=darkbloom

all : $(TARGET)

$(TARGET) : $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $(OBJS) main.o -o $(TARGET) $(LIBS)

.PHONY: clean dist

clean:
	@rm -f *.o *~ $(TARGET) $(TARGET).exe stdout.txt stderr.txt

dist:
	@rm -f *.o *~ stderr.txt stdout.txt
	@strip $(TARGET) $(TARGET).exe
