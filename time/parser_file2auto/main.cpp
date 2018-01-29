#include <string>
#include <iostream>
#include <fstream>
#include "../time_automata/grammar.hpp"
#include "../time_automata/toDot.hpp"
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
  cout << "Clocks : \n";
  for(automate::Clock cl : input_autom->clocks)
    cout << "\t"<< cl.name << " : " << cl.getId() << "\n";
  cout << "Number of state : " << input_autom->states.size() << "\nTheir names : \n";
  for(automate::State st : input_autom->states){
    cout << "\t" << st.id << "\n";
  }
  cout << "List of edges : " << input_autom->transitions.size() << "\n";
  for(automate::State & st : input_autom->states){
    for(automate::Transition & trans : input_autom->transitions[&st]){
      cout << "\t" << trans.origine->id << "->" << trans.destination->id << "\n";
      if(trans.clocks_constraints.length!=0){
        cout << "\t\tThe clocks_constraints DBM :" <<"\n";
        for(int i=0; i<trans.clocks_constraints.length;i++){
          cout << "\t\t\t";
          for(int j=0; j<trans.clocks_constraints.length;j++){
            cout << "(" << trans.clocks_constraints.matrice[i][j].value;
            cout << "," << trans.clocks_constraints.matrice[i][j].inclusion <<")";
            cout << "\t";
          }
          cout << "\n";
        }
      }
      if(! trans.clocks_to_reset.empty()){
        cout << "\t\tClocks to reset : ";
        for(Clock* clock : trans.clocks_to_reset){
          cout << clock->name << " : " << clock->getId() <<"\t";
        }
      }
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
