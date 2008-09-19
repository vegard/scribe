SYS		:= $(shell uname -s)

INCLUDES	= -I. -I/usr/include/SDL
LIBS		= -lpng -lSDL
FRAMEWORKS	=

ifeq ($(SYS),Darwin)
INCLUDES	+= -I/sw/include
LIBS		+= -L/sw/lib
FRAMEWORKS	+= -framework OpenGL -framework GLUT
else
LIBS		+= -lGL -lGLU
endif

all: scribe

%.o: %.cc
	g++ -Wall -g $(INCLUDES) -c -o $@ $^

scribe: $(patsubst %.cc,%.o,$(wildcard *.cc) $(wildcard sys/*.cc))
	g++ -Wall -g -o $@ $^ $(LIBS) $(FRAMEWORKS)
