#include "toDot.hpp"
#include <limits>

using namespace std;
using namespace automate;

void output_Clocks_constraint(ostream& output, DBM const& clocks_constraints,
                              vector<Clock*> const& clocks){
  for(Clock* clk : clocks){
    double value_min = clocks_constraints.matrice[0][clk->getId()].value;
    double value_max = clocks_constraints.matrice[clk->getId()][0].value;
    if(value_min == 0 && value_max==numeric_limits<double>::max())
      continue;
    output << "{" << clk->name << ":[";
    if(value_min == 0){
      output << "0";
    }else{
      output << (-1*value_min);
    }
    output << "," ;
    if(value_max==numeric_limits<double>::max()){
      output << "inf";
    }else{
      output << value_max;
    }
    output << "]}";
  }
}

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
      output_Clocks_constraint(output, state.clocks_constraints, autom->clocks);
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
        output_Clocks_constraint(output, trans.clocks_constraints, autom->clocks);
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
