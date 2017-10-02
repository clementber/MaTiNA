all:AutomRecognizer AutomChecker

AutomRecognizer: modelRecognizer.cpp inputAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11  $^ -L../parser_file2auto -lautomatefromfile -o $@

AutomChecker: modelRecognizer.cpp checkerAutom.cpp ../parser_file2auto/libautomatefromfile.a
	g++ -std=c++11  $^ -L../parser_file2auto -lautomatefromfile -o $@

../parser_file2auto/libautomatefromfile.a :
	cd ../parser_file2auto/ && $(MAKE)

clean:
	rm -f AutomRecognizer RegexRecognizer RegexChecker AutomChecker

cleanall:
	make clean all
