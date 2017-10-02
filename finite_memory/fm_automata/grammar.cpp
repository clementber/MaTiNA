#include "grammar.hpp"
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace automate;
using namespace std;

State::State(string identifiant,
             vector<int> assign) :
               id(identifiant), assignable(assign){ }
State::State(string identifiant):id(identifiant),assignable() { }
State::State() = default;
State::~State() = default;


Transition::Transition(State* const& ori, State* const& dest, vector<int> events) :
           origine(ori), destination(dest), triggers(events){
}
Transition::Transition(State* const& ori, State* const& dest):origine(ori), destination(dest){}
Transition::~Transition() = default;

Automate::Automate() = default;

Automate::Automate(list<State> p_states, map<State*, vector<Transition>> p_transitions,
         vector<string> p_config,State* p_start,vector<State*> p_endStates) :
         states(p_states), transitions(p_transitions), initial_config(p_config),
         start(p_start), endStates(p_endStates) {}
Automate::~Automate() = default;

State* Automate::getState(string state_name){
  for(State& state : this->states){
    if(state.id == state_name){
      return &state;
    }
  }
  return nullptr;
}
