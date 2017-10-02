#include <string>
#include <iostream>
#include <fstream>
#include "../time_automata/toDot.hpp"
#include "../time_automata/grammar.hpp"
#include "parsingDriver.hpp"

using namespace std;
using namespace automate;

int main(int argc, char *argv[]){
  if(argc != 3){
    cout << "Nombre d'argument incorrecte : <fichier .regex> <sortie .dot>\n";
  }
  regex_driver parser;
  automate::Automate*input_autom = new automate::Automate();
  int res = parser.parse (input_autom, argv[1]);
  cout << "Value returned by parse : " << res <<"\n";
  cout << "Number of state : " << input_autom->states.size() << "\nTheir names : \n";
  for(automate::State st : input_autom->states){
    cout << "\t" << st.id << "\n";
  }
  cout << "List of edges : " << input_autom->transitions.size() << "\n";
  for(automate::State & st : input_autom->states){
    for(automate::Transition & trans : input_autom->transitions[&st]){
      cout << "\t" << trans.origine->id << "->" << trans.destination->id << "\n";
      for(pair<string,Interval> elmt : trans.clocks_constraints){
        cout << "\t\t" << elmt.first << " " << elmt.second.borne_inf << "," << elmt.second.borne_sup << "\n";
      }
      for(string clock_name : trans.clocks_to_reset){
        cout << "\t\t" << clock_name <<"\n";
      }
    }
  }

  cout << "clocks names : \n";
  for(automate::Clock cl : input_autom->clocks)
    cout << "\t"<< cl.name << "\n";

  ofstream output(argv[2]);
  convert_to_dot(input_autom,output);
  output.close();

  delete (input_autom);
  return 0;
}
