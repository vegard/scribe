all: link

%.o: %.cc
	g++ -Wall -g -c -o $@ $^

link: $(patsubst %.cc,%.o,$(wildcard *.cc))
	g++ -Wall -g -o $@ $^ -lGL -lGLU -lglut -lpng
