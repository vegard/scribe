SYS		:= $(shell uname -s)

INCLUDES	= -I.
LIBS		= -lpng
FRAMEWORKS	=

ifeq ($(SYS),Darwin)
INCLUDES	+= -I/sw/include
LIBS		+= -L/sw/lib
FRAMEWORKS	+= -framework OpenGL -framework GLUT
else
LIBS		+= -lGL -lGLU -lglut
endif

all: scribe

%.o: %.cc
	g++ -Wall -g $(INCLUDES) -c -o $@ $^

scribe: $(patsubst %.cc,%.o,$(wildcard *.cc) $(wildcard sys/*.cc))
	g++ -Wall -g -o $@ $^ $(LIBS) $(FRAMEWORKS)
