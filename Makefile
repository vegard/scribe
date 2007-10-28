all: a.out

a.out: main.cc
	g++ -Wall -g main.cc -lGL -lGLU -lglut -lpng
