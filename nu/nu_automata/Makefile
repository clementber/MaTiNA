all: grammar.o toDot.o

clean:
	rm -f *.o

%.o : %.cpp
	g++ -std=c++11 -Wall -Wextra -c -o $@ $^
