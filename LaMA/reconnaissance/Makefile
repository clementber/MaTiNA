all:AutomRecognizer AutomChecker RegexRecognizer RegexChecker

AutomRecognizer: modelRecognizer.cpp inputAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11 -O3 $^ -L../parser_file2auto -lautomatefromfile -o $@

AutomChecker: modelRecognizer.cpp checkerAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11 -O3 $^ -L../parser_file2auto -lautomatefromfile -o $@

RegexRecognizer: modelRecognizer.cpp inputRegexp.cpp ../parser_regex2auto/libautomatefromregexp.a
	g++ -std=c++11 -O3 $^ -L../parser_regex2auto -lautomatefromregexp -o $@

RegexChecker: modelRecognizer.cpp checkerRegexp.cpp ../parser_regex2auto/libautomatefromregexp.a
	g++ -std=c++11 -O3 $^ -L../parser_regex2auto -lautomatefromregexp -o $@

../parser_file2auto/libautomatefromfile.a :
	cd ../parser_file2auto/ && $(MAKE)

clean:
	rm -f AutomRecognizer RegexRecognizer RegexChecker AutomChecker

cleanall:
	make clean all
