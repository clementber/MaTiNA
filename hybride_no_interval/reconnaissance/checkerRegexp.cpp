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
    for(Token const& tok : checker.map_tokens[checker.modele->endStates[0]]){
      cout << tok.toString();
      cout.flush();
    }
  }
}

int main(int argc, char**argv){
  if(argc <3){
    cout << "Nombre d'argument incorrecte\n";
    cout << "Première argument : fichier .autom\n";
    cout << "Second argument : time event sequence\n";
    return 1;
  }
  automate::Automate*input_autom = new automate::Automate();
  regex_driver parser;

  int res = parser.parse (input_autom, argv[1]);


  ifstream tes(argv[2], std::ifstream::in);

  Checker checker(input_autom);

  string str = "";
  char c;
  int cpt = 0;
  auto ite = checker.modele->states.begin();

  State * begin = &(*ite);
  ite++;
  State * bdel = &(*ite);

  try{
    while((c = tes.get()) != EOF){
      if(c == ' ' || c == '\n' || c=='\t'){
        if(str != ""){
          cpt++;
          input(str,checker);

          //State printing.
          if(cpt%300 == 0){
            cout << (cpt/3) << " - "<< (((double)clock())/CLOCKS_PER_SEC)<< "\n";
            cout.flush();
          }
          if(checker.map_tokens[begin].empty() && checker.map_tokens[bdel].empty()){
            cout << "L'analyse a crash à l'input : " << str << " - " << cpt << "\n";
            str ="";
            break;
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
  checker.print_state("final");
  delete input_autom;
  return 0;
}
