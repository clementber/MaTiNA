#include <string>
#include <iostream>
#include <fstream>
#include "grammar.hpp"
#include "../parser_file2auto/parsingDriver.hpp"
#include <typeinfo>

using namespace std;
using namespace automate;

int main(int argc, char *argv[]){
  if(argc <3){
    cout << "Nombre d'argument incorrecte\n";
    return 1;
  }
  autom_driver parser;
  automate::Automate* prefix_autom = new automate::Automate();
  int res = parser.parse (prefix_autom, argv[1]);
  if(res != 0){
    if(prefix_autom != nullptr){
      delete prefix_autom;
    }
    return 1;
  }
  
  cout << "Automate " << argv[1] <<" parsé\n";
  
  automate::Automate* suffix_autom = new automate::Automate();
  res = parser.parse (suffix_autom, argv[2]);
  if(res != 0){
    if(suffix_autom != nullptr){
      delete suffix_autom;
    }
    delete prefix_autom;
    return 1;
  }
  
  cout << "Automate " << argv[2] <<" parsé\n";
  
  Automate * concat = interesection(prefix_autom, suffix_autom);
  
  cout <<"Fin de concate\n";
  
  cout << concat->to_string();

  delete suffix_autom;
  delete prefix_autom;
  delete concat;
  return 0;
}
