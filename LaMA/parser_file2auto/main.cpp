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
  cout << "Initial Valuation :\n" << input_autom->initial_valuation.to_string();
  cout <<"\n";
  cout << "List of states : " <<input_autom->states.size() <<"\n";
  for(State st : input_autom->states){
    cout << "\t" << st.id << "\n";
  }
  cout << "List of edges : " << input_autom->transitions.size() << "\n";
  for(State & st : input_autom->states){
    for(Transition * trans : input_autom->transitions[&st]){
      cout << "\t" << trans->origine->id << "->" << trans->destination->id << " : ";
      cout << trans->to_string() ;

      if(!trans->allocations.empty()){
        cout << "  nu : {" << trans->allocations[0].to_string();
        for(unsigned int i = 1;i< trans->allocations.size(); i++){
          cout << ", " << trans->allocations[i].to_string();
        }
        cout << "}";
      }
      if(!trans->frees.empty()){
        cout << "  free : {" << trans->frees[0].to_string();
        for(unsigned int i = 1;i< trans->frees.size(); i++){
          cout << ", " << trans->frees[i].to_string();
        }
        cout << "}";
      }
      cout << "\n";
    }
  }

  /*if(argc>=3){
    ofstream output(argv[2]);
    convert_to_dot(input_autom, output);
    output.close();
  }*/
  delete (input_autom);
  return 0;
}
