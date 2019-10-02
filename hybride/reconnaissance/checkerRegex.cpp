#include "modelRecognizer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "../hybride_automata/grammar.hpp"
#include "../parser_regex2auto/parsingDriver.hpp"
#include <ctime>

using namespace std;
using namespace automate;
using namespace recognizer;

void input(string const& str, Checker & checker){
  if(str[0]>= '0' && str[0]<= '9'){
    checker.input(stod(str));
  }else{
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

  int res = parser.parse (input_autom, argv[1]);


  ifstream tes(argv[2], std::ifstream::in);

  Checker checker(input_autom);

  string str = "";
  char c;
  int cpt = 0;
  auto ite = checker.modele->states.begin();

  try{
    while((c = tes.get()) != EOF){
      if(c == ' ' || c == '\n' || c=='\t'){
        if(str != ""){
          cpt++;
          input(str,checker);
          //Printing the token in final state
          for(State * endState : checker.modele->endStates){
            for(Token const& tok : checker.map_tokens[endState]){
              cout << tok.to_string();
              cout << "\n";
            }
          }
          str = "";
        }
      }else{
        str+=c;
      }
    }
    if(str != ""){
      input(str,checker);
      str = "";
    }
  }catch(const std::out_of_range & oor){
    cout << "Error come from the input " << cpt <<" of value : " << str << "\n";
    throw oor;
  }
  cout << "Input read : "<< cpt << "\n";
  checker.print_state("final");
  return 0;
}
