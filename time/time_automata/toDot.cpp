#include "toDot.hpp"
#include <limits>

using namespace std;
using namespace automate;

void convert_to_dot(Automate* autom, ostream& output){
  output << "digraph {\n";
  for(State & state : autom->states){
    int color = 0;
    if(&state == autom->start){
      color++;
    }
    for(State * endState : autom->endStates){
      if(&state == endState){
        color += 2;
      }
    }
    output << state.id << " [color=";
    switch (color) {
      case 0: output << "black";break;
      case 1: output << "blue";break;
      case 2: output << "red";break;
      case 3: output << "green";break;
    }
    output << "][label=\"";
    output << state.id;
    if(!(state.clocks_constraints.getClocks_number() == 0 || state.clocks_constraints.empty())){
      output << " ";
      for(Clock clk : autom->clocks){
        output << "{" << clk.name << ":[";
        double value = state.clocks_constraints.matrice[0][clk.getId()].value;
        output << (value==0?"0":to_string(-1*value)) << "," ;
        value = state.clocks_constraints.matrice[clk.getId()][0].value;
        output<<(value==numeric_limits<double>::max()?"\\inf":to_string(value));
        output << "]}";
      }
    }
    output << "\"]\n";
  }

  for(State & state : autom->states){
    for(Transition const& trans : autom->transitions[&state]){
      output << trans.origine->id << "->" << trans.destination->id <<"[label=\"";
      if(trans.triggers.empty()){
        output << "epsi";
      }else{
        output << trans.triggers[0];
        for(unsigned int i = 1; i<trans.triggers.size(); i++){
          output << "," << trans.triggers[i];
        }
      }
      if(!(trans.clocks_constraints.getClocks_number() == 0 || trans.clocks_constraints.empty())){
        output << "\\n";
        for(Clock clk : autom->clocks){
          output << "{" << clk.name << ":[";
          double value = trans.clocks_constraints.matrice[0][clk.getId()].value;
          output << (value==0?"0":to_string(-1*value)) << ",";
          value = trans.clocks_constraints.matrice[clk.getId()][0].value;
          output << (value==numeric_limits<double>::max()? "\\inf":to_string(value));
          output << "]}";
        }
      }
      if(!trans.clocks_to_reset.empty()){
        output << "\\n";
        output << trans.clocks_to_reset[0]->name << ":=0";
        for(unsigned int i =1; i<trans.clocks_to_reset.size(); i++){
          output << "," << trans.clocks_to_reset[i]->name << ":=0";
        }
      }
      output << "\"]\n";
    }
  }
  output << "}";
}
