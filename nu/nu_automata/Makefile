all: grammar.o toDot.o

clean:
	rm *.o

%.o : %.cpp
	g++ -std=c++11 -Wall -Wextra -c -o $@ $^
