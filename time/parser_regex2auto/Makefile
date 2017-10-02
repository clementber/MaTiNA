all: libautomatefromregexp.a

test : yacc.tab.o lexer.yy.o ../time_automata/grammar.o parsingDriver.o main.o ../time_automata/toDot.o
	g++ -std=c++11 -Wall -Wextra -o $@ $^

libautomatefromregexp.a: yacc.tab.o lexer.yy.o ../time_automata/grammar.o parsingDriver.o
	ar rvs $@ $^
	ranlib $@

%.o : %.cpp
	g++ -c -std=c++11 -Wall -Wextra -o $@ $^

%.o : %.cc
		g++ -c -std=c++11 -Wall -Wextra -o $@ $^

yacc.tab.cpp : yacc.ypp ../time_automata/grammar.hpp
	bison -d -v yacc.ypp

lexer.yy.cc : lexer.lex yacc.tab.hpp
	flex --outfile=lexer.yy.cc lexer.lex

clean :
	rm lexer.yy.cc location.hh position.hh stack.hh yacc.output yacc.tab.*pp *.o *.a
	rm -f test
