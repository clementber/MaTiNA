all: libautomatefromfile.a

test :  ../hybride_automata/grammar.o yacc.tab.o lexer.yy.o parsingDriver.o ../hybride_automata/toDot.o main.o
	g++ -O3 -std=c++11 -Wall -Wextra -o $@ $^

libautomatefromfile.a: yacc.tab.o lexer.yy.o ../hybride_automata/grammar.o parsingDriver.o
	ar rvs $@ $^
	ranlib $@

%.o : %.cpp
	g++ -O3 -c -std=c++11 -Wall -Wextra -o $@ $^

%.o : %.cc
	g++ -O3 -c -std=c++11 -Wall -Wextra -o $@ $^

yacc.tab.cpp : yacc.ypp ../hybride_automata/grammar.hpp
	bison -d -v yacc.ypp

lexer.yy.cc : lexer.lex yacc.tab.hpp
	flex --outfile=lexer.yy.cc lexer.lex

clean :
	rm -f lexer.yy.cc location.hh position.hh stack.hh yacc.output yacc.tab.*pp *.o *.a
	rm -f test
