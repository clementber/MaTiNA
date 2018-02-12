#include "modelRecognizer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "../time_automata/grammar.hpp"
#include "../parser_regex2auto/parsingDriver.hpp"

using namespace std;
using namespace automate;
using namespace recognizer;

void input(string const& str, Checker & checker){
  try{
    checker.input(stod(str));
  }catch(const invalid_argument& ia){
    checker.input(str);
  }
}

int main(int argc, char**argv){
  if(argc <3){
    cout << "Nombre d'argument incorrecte\n";
    cout << "Première argument : fichier .autom\n";
    cout << "Second argument : time event sequence\n";
    return 1;
  }
  automate::Automate*input_autom;
  regex_driver parser;

  int res = parser.parse (&input_autom, argv[1]);


  ifstream tes(argv[2], std::ifstream::in);

  Checker checker(input_autom);

  string str = "";
  char c;
  //checker.print_state("Initial");

  while((c = tes.get()) != EOF){
    if(c == ' ' || c == '\n' || c=='\t'){
      if(str != ""){
        input(str,checker);
        //cout << "input : " << str << " pattern reconnu? " << checker.check()<<"\n";
        str = "";
      }
      continue;
    }else{
      str+=c;
    }
  }
  if(str != ""){
    input(str,checker);
    //cout << "input : " << str << " pattern reconnu? " << checker.check()<<"\n";
    str = "";
  }
  //checker.print_state("final");
  return 0;
}
