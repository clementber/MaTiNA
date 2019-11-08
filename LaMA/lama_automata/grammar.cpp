#include "grammar.hpp"
using namespace automate;
using namespace std;

//---------------------------------------Variable-------------------------------
Variable::Variable(int name, int layer): name(name),layer(layer){ }

//-----------------------Memory Valuation---------------------------------------

bool Valuation::isFresh(int const& layer, string const& event) const{
  //TODO
}

void Valuation::alloc(vector<Variable> to_alloc){
  //TODO
}

void Valuation::desalloc(vector<Variable> to_alloc){
  //TODO
}

bool Valuation::use(vector<Variable> to_alloc, string const& value){
  //TODO
}

bool Valuation::isEmpty() const{
  //TODO
}

//-----------------------State--------------------------------------------------
State::State(string identifiant) : id(identifiant){ }
State::State() = default;
State::~State() = default;

//---------------------Transition-----------------------------------------------

Transition::Transition(State* const& ori, State* const& dest, vector<int> allocs,
                       vector<int> freez) :
  origine(ori), destination(dest), allocations(allocs), frees(freez){}
         
Transition::Transition(State* const& ori, State* const& dest):
  origine(ori), destination(dest){}
  
Transition::~Transition() = default;

Valuation Transition::accept_epsilon(Valuation memory){
  return Valuation();
}

Valuation Transition::accept_value(Valuation memory, string p_event){
  return Valuation();
}
//--------------------------Epsilon-Transition-------------------------------

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest, 
                                       vector<int> allocs, vector<int> freez):
  Transition(ori,dest,allocs,freez){}
           
Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest): 
  Transition(ori,dest){}

Epsilon_Transition::~Epsilon_Transition() = default;

Valuation Epsilon_Transition::accept_epsilon(Valuation memory){
  memory.alloc(this->allocations);
  memory.desalloc(this->frees);
  return memory;
}
string Epsilon_Transition::to_string(){
  //TODO
  return "epsi";
}


//-------------------------Event-Transition-------------------------------------

Event_Transition::Event_Transition(State* const& ori, State* const& dest,
                                   vector<int> allocs, vector<int> freez, 
                                   vector<Variable> triggers):
  Transition(ori,dest,allocs,freez),trigger(trigger){}
  
Event_Transition::Event_Transition(State* const& ori, State* const& dest, 
                                   vector<Variable> triggers):
  Transition(ori,dest), triggers(triggers){}
  
Event_Transition::~Event_Transition() = default;

Valuation Event_Transition::accept_event(Valuation memory, string event){
  memory.alloc(this->allocations);
  if(!memory.use(this->triggers)){
    return Valuation();
  }
  memory.desalloc(this->frees);
  return memory;
}

string Event_Transition::to_string(){
  //TODO
  return "{" + std::to_string(variable) + "}";
}

//--------------------------Universal-Transition--------------------------------

Universal_Transition::Universal_Transition(State* const& ori, State* const& dest,
                                           vector<int> allocs,vector<int> freez):
  Transition(ori,dest,allocs,freez) {}
  
Universal_Transition::Universal_Transition(State* const& ori, State* const& dest):
  Transition(ori,dest) {}
  
Universal_Transition::~Universal_Transition() = default;

Valuation Universal_Transition::accept_value(Valuation memory, string event){
  memory.alloc(this->allocations);
  memory.desalloc(this->frees);

  return memory;
}

string Constant_Transition::to_string(){
  return "@";
}

//-------------------------Automata---------------------------------------------

Automate::Automate() = default;

Automate::Automate(int p_ressources, Valuation initial_valuation,
            list<State> p_states, map<State*, vector<Transition*>> p_transitions,
            State* p_start, vector<State*> p_endStates) :
  ressources(p_ressources), initial_valuation(initial_valuation),
  states(p_states), transitions(p_transitions),
  start(p_start), endStates(p_endStates) {}
  
Automate::~Automate() {
  //TODO
  for(auto & pair : transitions){
    for(Transition * trans : pair.second){
      delete trans;
    }
  }
}

State* Automate::getState(string state_name){
  for(State& state : this->states){
    if(state.id == state_name){
      return &state;
    }
  }
  return nullptr;
}
