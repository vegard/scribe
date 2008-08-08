all: link

%.o: %.cc
	g++ -Wall -g -I. -c -o $@ $^

link: $(patsubst %.cc,%.o,$(wildcard *.cc) $(wildcard sys/*.cc))
	g++ -Wall -g -o $@ $^ -lGL -lGLU -lglut -lpng
