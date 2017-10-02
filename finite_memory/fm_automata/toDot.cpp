#include "toDot.hpp"

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
    if(!state.assignable.empty()){
      output << " {";
      output << state.assignable[0];
      for(unsigned int i=1; i<state.assignable.size(); i++){
        output <<"," << state.assignable[i];
      }
      output << "}";
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
      output << "\"]\n";
    }
  }
  output << "}";
}
