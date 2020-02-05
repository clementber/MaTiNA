#include <string>
#include <iostream>
#include <fstream>
#include "../lama_automata/grammar.hpp"
// #include "../lama_automata/toDot.hpp"
#include "parsingDriver.hpp"
#include <typeinfo>

using namespace std;
using namespace automate;

int main(int argc, char *argv[]){
  if(argc <2){
    cout << "Nombre d'argument incorrecte\n";
  }
  autom_driver parser;
  automate::Automate * input_autom = new automate::Automate();
  int res = parser.parse (input_autom, argv[1]);
  
  if(res != 0){
    if(input_autom != nullptr){
      delete input_autom;
    }
    return 1;
  }
  
  cout << "Value returned by parse : " << res <<"\n";
  cout << input_autom->to_string();

  /*if(argc>=3){
    ofstream output(argv[2]);
    convert_to_dot(input_autom, output);
    output.close();
  }*/
  delete (input_autom);
  return 0;
}
