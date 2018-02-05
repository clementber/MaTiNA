all: libautomatefromregexp.a

test : yaccAST.tab.o lexer.yy.o ../time_automata/grammar.o parsingDriver.o main.o ../time_automata/toDot.o
	g++ -std=c++11 -Wall -Wextra -o $@ $^

libautomatefromregexp.a: yaccAST.tab.o lexer.yy.o ../time_automata/grammar.o parsingDriver.o
	ar rvs $@ $^
	ranlib $@

%.o : %.cpp
	g++ -c -std=c++11 -Wall -Wextra -o $@ $^

%.o : %.cc
		g++ -c -std=c++11 -Wall -Wextra -o $@ $^

yaccAST.tab.cpp : yaccAST.ypp ../time_automata/grammar.hpp
	bison -d -v yaccAST.ypp

lexer.yy.cc : lexer.lex yaccAST.tab.hpp
	flex --outfile=lexer.yy.cc lexer.lex

clean :
	rm -f lexer.yy.cc location.hh position.hh stack.hh yaccAST.output yaccAST.tab.*pp *.o *.a
	rm -f test
