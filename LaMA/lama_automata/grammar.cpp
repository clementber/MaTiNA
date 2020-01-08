#include "grammar.hpp"
using namespace automate;
using namespace std;

//---------------------------------------Variable-------------------------------
Variable::Variable(int name, int layer): name(name),layer(layer){ }

//-----------------------Memory Valuation---------------------------------------
Valuation::Valuation() : nb_layers(0), nb_variables(0){
  value = NULL;
}
  
Valuation::Valuation(unsigned int nb_layers, unsigned int nb_variables):
  nb_layers(nb_layers),nb_variables(nb_variables)
{
  value = new pair<bool,unordered_set<string>>*[nb_layers];
  for(unsigned int i = 0; i< nb_layers; i++){
    value[i] = new pair<bool,unordered_set<string>>[nb_variables];
    for(unsigned int j = 0; j< nb_variables; j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
    }
  }
}

Valuation::Valuation(vector<vector<vector<string>>> const& source):
  nb_layers(source.size())
{
  value = new pair<bool,unordered_set<string>>*[nb_layers];
  for(int i = 0; i< nb_layers; i++){
    value[i] = new pair<bool,unordered_set<string>>[source[i].size()];
    for(int j = 0; j< source[i].size(); j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
      value[i][j].second.insert(source[i][j].begin(),source[i][j].end());
    }
  }
}

Valuation::Valuation(Valuation const& original):nb_layers(original.nb_layers),
  nb_variables(original.nb_variables){
  value = new pair<bool,unordered_set<string>>*[nb_layers];
  for(unsigned int i = 0; i< nb_layers; i++){
    value[i] = new pair<bool,unordered_set<string>>[nb_variables];
    for(unsigned int j = 0; j< nb_variables; j++){
      value[i][j] = original.value[i][j];
    }
  } 
}

Valuation::~Valuation(){
  for(unsigned int i= 0; i < nb_layers; i++){ delete[] value[i];}
  if(value != NULL){ delete[] value; }
}

int Valuation::get_nb_layers(){ return nb_layers; }

int Valuation::get_nb_variables(){ return nb_variables; }

//TODO optimize
bool Valuation::isFresh(int const& layer, string const& event) const{
  for(int i = 0; i < nb_variables; i++){
    if(value[layer][i].second.find(event) != value[layer][i].second.end()){
      return false;
    }
  }
  return true;
}

void Valuation::alloc(vector<Variable> const& to_alloc){
  for(Variable var : to_alloc){
    value[var.layer][var.name].first = true;
  }
}

void Valuation::desalloc(vector<Variable> const& to_free){
  for(Variable var : to_free){
    value[var.layer][var.name].first = false;
    value[var.layer][var.name].second.clear();
  }
}

//TODO optimize
bool Valuation::can_use(vector<Variable> const& to_use, string const& event){
  for(Variable var : to_use){
    if(value[var.layer][var.name].first){ //Allocated case (write mode)
      if( ! this->isFresh(var.layer, event)){
        return false;
      }
    }else{ // Not allocated (Read mode)
      if(value[var.layer][var.name].second.find(event)==value[var.layer][var.name].second.end()){
        return false;
      }
    }
  }
  return true;
}

//TODO optimize
bool Valuation::use(vector<Variable> const& to_use, string const& event){
  if( ! can_use(to_use, event)) return false;
  for(Variable var : to_use){
    if(value[var.layer][var.name].first){ //Allocated case (write mode)
      value[var.layer][var.name].second.insert(event);
    }
  }
  return true;
}

bool Valuation::isNull() const{
  return value==NULL;
}

//-----------------------State--------------------------------------------------
State::State(string identifiant) : id(identifiant){ }
State::State() = default;
State::~State() = default;

//---------------------Transition-----------------------------------------------

Transition::Transition(State* const& ori, State* const& dest, 
                       vector<Variable> allocs, vector<Variable> freez) :
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
                                       vector<Variable> allocs,
                                       vector<Variable> freez):
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
                                   vector<Variable> allocs,
                                   vector<Variable> freez, 
                                   vector<Variable> triggers):
  Transition(ori,dest,allocs,freez),triggers(triggers){}
  
Event_Transition::Event_Transition(State* const& ori, State* const& dest, 
                                   vector<Variable> triggers):
  Transition(ori,dest), triggers(triggers){}
  
Event_Transition::~Event_Transition() = default;

Valuation Event_Transition::accept_value(Valuation memory, string event){
  memory.alloc(this->allocations);
  if(!memory.use(this->triggers, event)){
    return Valuation();
  }
  memory.desalloc(this->frees);
  return memory;
}

string Event_Transition::to_string(){
  //TODO
  return "event";//"{" + std::to_string(triggers) + "}";
}

//---------------------------Constant-Transition--------------------------------
Constant_Transition::Constant_Transition(State* const& ori, State* const& dest,
                                           vector<Variable> allocs, 
                                           vector<Variable> freez,
                                           string constant):
  Transition(ori,dest,allocs,freez),constant(constant) {}
  
Constant_Transition::Constant_Transition(State* const& ori, State* const& dest,
                                         string constant):
  Transition(ori,dest),constant(constant) {}
  
Constant_Transition::~Constant_Transition() = default;

Valuation Constant_Transition::accept_value(Valuation memory, string event){
  if(constant != event){
    return Valuation();
  }
  memory.alloc(this->allocations);
  memory.desalloc(this->frees);
  return memory;
}

string Constant_Transition::to_string(){
  return constant;
}

//--------------------------Universal-Transition--------------------------------

Universal_Transition::Universal_Transition(State* const& ori, State* const& dest,
                                           vector<Variable> allocs,
                                           vector<Variable> freez):
  Transition(ori,dest,allocs,freez) {}
  
Universal_Transition::Universal_Transition(State* const& ori, State* const& dest):
  Transition(ori,dest) {}
  
Universal_Transition::~Universal_Transition() = default;

Valuation Universal_Transition::accept_value(Valuation memory, string event){
  memory.alloc(this->allocations);
  memory.desalloc(this->frees);

  return memory;
}

string Universal_Transition::to_string(){
  return "@";
}

//-------------------------Automata---------------------------------------------

Automate::Automate() = default;

Automate::Automate(Valuation initial_valuation, list<State> p_states, 
                   map<State*, vector<Transition*>> p_transitions,
                   State* p_start, vector<State*> p_endStates) :
  initial_valuation(initial_valuation), states(p_states),
  transitions(p_transitions), start(p_start), endStates(p_endStates) {}
  
Automate::~Automate() {
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
