#include <string>
#include <iostream>
#include <fstream>
#include "../fm_automata/grammar.hpp"
#include "../fm_automata/toDot.hpp"
#include "parsingDriver.hpp"

using namespace std;
using namespace automate;

int main(int argc, char *argv[]){
  if(argc <2){
    cout << "Nombre d'argument incorrecte\n";
  }
  autom_driver parser;
  automate::Automate*input_autom = new automate::Automate();
  int res = parser.parse (input_autom, argv[1]);
  cout << "Value returned by parse : " << res <<"\n";
  cout << "Number of state : " << input_autom->states.size() << "\nList of states : \n";
  for(automate::State st : input_autom->states){
    cout << "\t" << st.id << ": ";
    for(int assign : st.assignable){
      cout << assign << " ";
    }
    cout << "\n";
  }
  cout << "List of edges : " << input_autom->transitions.size() << "\n";
  for(automate::State & st : input_autom->states){
    for(automate::Transition & trans : input_autom->transitions[&st]){
      cout << "\t" << trans.origine->id << "->" << trans.destination->id << " : ";
      for(int trigger : trans.triggers){
        cout << trigger << " ";
      }
      cout << "\n";
    }
  }
  
  if(argc>=3){
    ofstream output(argv[2]);
    convert_to_dot(input_autom, output);
    output.close();
  }
  delete (input_autom);
  return 0;
}
