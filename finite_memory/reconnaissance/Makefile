all:AutomRecognizer RegexRecognizer RegexChecker AutomChecker

AutomRecognizer: modelRecognizer.cpp inputAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11  $^ -L../parser_file2auto -lautomatefromfile -o $@

RegexRecognizer: modelRecognizer.cpp inputRegex.cpp ../parser_regex2auto/libautomatefromregexp.a
	g++ -std=c++11  $^ -L../parser_regex2auto -lautomatefromregexp -o $@

RegexChecker: modelRecognizer.cpp checkRegex.cpp ../parser_regex2auto/libautomatefromregexp.a
	g++ -std=c++11  $^ -L../parser_regex2auto -lautomatefromregexp -o $@

AutomChecker: modelRecognizer.cpp checkerAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11  $^ -L../parser_file2auto -lautomatefromfile -o $@

../parser_file2auto/libautomatefromfile.a :
	cd ../parser_file2auto/ && $(MAKE)

../parser_regex2auto/libautomatefromregexp.a :
	cd ../parser_regex2auto/ && $(MAKE)

clean:
	rm -f AutomRecognizer RegexRecognizer RegexChecker AutomChecker

cleanall:
	make clean all
