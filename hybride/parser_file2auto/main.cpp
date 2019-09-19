#include <string>
#include <iostream>
#include <fstream>
#include "../hybride_automata/grammar.hpp"
#include "../hybride_automata/toDot.hpp"
#include "parsingDriver.hpp"
#include <typeinfo>
using namespace std;
using namespace automate;

int main(int argc, char *argv[]){
  if(argc <2){
    cout << "Nombre d'argument incorrecte\n";
  }
  autom_driver parser;
  automate::Automate*input_autom = new automate::Automate();
  int res = parser.parse (input_autom, argv[1]);
  if(res != 0){
    if(input_autom != nullptr){
      delete input_autom;
    }
    return 1;
  }
  
  cout << "Value returned by parse : " << res <<"\n";
  cout << "Number of state : " << input_autom->states.size() << "\nList of states : \n";
  for(State st : input_autom->states){
    cout << "\t" << st.id << "\n";
    if(!(st.clocks_constraints.getClocks_number() == 0 || st.clocks_constraints.empty())){
      output_Clocks_constraint(cout, st.clocks_constraints, input_autom->clocks);
    }
    cout << "\n";
  }
  cout << "List of edges : " << input_autom->transitions.size() << "\n";
  for(State & st : input_autom->states){
    for(Transition * trans : input_autom->transitions[&st]){
      cout << "\t" << trans->origine->id << "->" << trans->destination->id << " : ";
      cout << trans->to_string() ;

      if(!trans->allocations.empty()){
        cout << " nu : {";
        cout << trans->allocations[0];
        for(int i = 1;i< trans->allocations.size(); i++){
          cout << ", " << trans->allocations[i];
        }
        cout << "}";
      }
      if(!trans->frees.empty()){
        cout << " free : {";
        cout << trans->frees[0];
        for(int i = 1;i< trans->frees.size(); i++){
          cout << ", " << trans->frees[i];
        }
        cout << "}";
      }
      cout << "\n";
      if(!(trans->clocks_constraints.getClocks_number() == 0 || trans->clocks_constraints.empty())){
        output_Clocks_constraint(cout, trans->clocks_constraints, input_autom->clocks);
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
