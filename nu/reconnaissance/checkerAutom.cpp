#include "modelRecognizer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "../nu_automata/grammar.hpp"
#include "../parser_file2auto/parsingDriver.hpp"
#include <ctime>

using namespace std;
using namespace automate;
using namespace recognizer;

void input(string const& str, Checker & checker){
    checker.input(str);
}

int main(int argc, char**argv){
  if(argc <3){
    cout << "Nombre d'argument incorrecte\n";
    cout << "Première argument : fichier .autom\n";
    cout << "Second argument : time event sequence\n";
    return 1;
  }
  automate::Automate*input_autom = new automate::Automate();
  autom_driver parser;

  int res = parser.parse (input_autom, argv[1]);


  ifstream tes(argv[2], std::ifstream::in);

  Checker checker(input_autom);

  string str = "";
  char c;
  //checker.print_state("Initial");
int cpt = 0;
  while((c = tes.get()) != EOF){
    if(c == ' ' || c == '\n' || c=='\t'){
      if(str != ""){
        cpt++;
        input(str,checker);
        if(cpt%100 == 0){
          cout << cpt << "/2000 - "<< (((double)clock())/CLOCKS_PER_SEC)<< "\n";
        }
        str = "";
      }
      continue;
    }else{
      str+=c;
    }
  }
  if(str != ""){
    input(str,checker);
    str = "";
  }
  checker.print_state("final");
  return 0;
}
