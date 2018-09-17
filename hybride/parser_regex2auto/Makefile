all: libautomatefromregexp.a

test : yacc.tab.o lexer.yy.o ../hybride_automata/grammar.o parsingDriver.o main.o ast.o ../hybride_automata/toDot.o
	g++ -std=c++11 -Wall -Wextra -o $@ $^

libautomatefromregexp.a: yacc.tab.o lexer.yy.o ../hybride_automata/grammar.o parsingDriver.o ast.o
	ar rvs $@ $^
	ranlib $@

%.o : %.cpp
	g++ -c -std=c++11 -Wall -Wextra -o $@ $^

%.o : %.cc
		g++ -c -std=c++11 -Wall -Wextra -o $@ $^

yacc.tab.cpp : yacc.ypp ast.hpp
	bison -d -v yacc.ypp

lexer.yy.cc : lexer.lex yacc.tab.hpp
	flex --outfile=lexer.yy.cc lexer.lex

clean :
	rm -f lexer.yy.cc location.hh position.hh stack.hh yacc.output yacc.tab.*pp
	rm -f *.o *.a *~
	rm -f test
