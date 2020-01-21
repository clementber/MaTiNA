#include "grammar.hpp"
#include <sstream>

using namespace automate;
using namespace std;

//---------------------------------------Variable-------------------------------
Variable::Variable(int name, int layer): name(name),layer(layer){ }

string Variable::to_string() const{
  stringstream res;
  res << "("<<name<<","<<layer<<")";
  return res.str();
}
//-----------------------Memory Valuation---------------------------------------
Valuation::Valuation() : value(0){}
  
Valuation::Valuation(unsigned int nb_layers, unsigned int nb_variables):
  nb_variables(nb_variables), value(nb_layers)
{
  for(unsigned int i = 0; i< nb_layers; i++){
    value[i] = vector<pair<bool,unordered_set<string>>>(nb_variables);
    for(unsigned int j = 0; j< nb_variables; j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
    }
  }
}

Valuation::Valuation(vector<vector<vector<string>>> const& source, int nb_vars):
  nb_variables(nb_vars), value(source.size())
{
  for(unsigned int i = 0; i< source.size(); i++){
    value[i] = vector<pair<bool,unordered_set<string>>>(nb_variables);
    unsigned int j(0);
    for(; j< source[i].size(); j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
      value[i][j].second.insert(source[i][j].begin(),source[i][j].end());
    }
    for(;j< nb_variables; j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
    }
  }
}

Valuation::Valuation(Valuation const& original):
  nb_variables(original.nb_variables), value(original.value){}

Valuation::~Valuation()= default;

unsigned int Valuation::nb_layers() const { return value.size(); }

unsigned int Valuation::get_nb_variables() const { return nb_variables; }

//TODO optimize
bool Valuation::isFresh(int const& layer, string const& event) const{
  for(unsigned int i = 0; i < nb_variables; i++){
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

Valuation Valuation::join(Valuation const& val2) const{
  Valuation res(*this);
  res.value.resize((nb_layers()< val2.nb_layers())?val2.nb_layers():nb_layers());
  res.nb_variables = (nb_variables< val2.nb_variables)?val2.nb_variables:nb_variables;
  unsigned int common_layers = (nb_layers()> val2.nb_layers())?val2.nb_layers():nb_layers();
  unsigned int common_vars = (nb_variables> val2.nb_variables)?val2.nb_variables:nb_variables;
  unsigned int i=0;
  for(; i< common_layers;i++){
    unsigned int j = 0;
    for(; j < common_vars; j++){
      res.value[i][j].second.insert(val2.value[i][j].second.begin(),val2.value[i][j].second.end());
    }
    for(; j < val2.nb_variables; j++){
      res.value[i][j] = val2.value[i][j];
    }
  }
  for(; i < val2.nb_layers(); i++){
    unsigned int j = 0;
    for(; j< val2.nb_variables;j++){
      res.value[i][j] = val2.value[i][j];
    }
    for(;j < this->nb_variables; j++){
      res.value[i][j] = pair<bool,unordered_set<string>>(false, unordered_set<string>());
    }
  }
  return res;
}

string Valuation::to_string() const{
  stringstream res;
  for(unsigned int i = 0; i < nb_layers(); i++){
    res << "L"<< i << ": ";
    for(unsigned int j = 0; j < nb_variables; j++){
      res << "{";
      for(string val : value[i][j].second){
        res << val << ",";
      }
      res << "}" << ((value[i][j].first)?"* ":" ");
    }
    res << "\n";
  }
  return res.str();
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

Transition* Epsilon_Transition::copy() const{
  return new Epsilon_Transition(*this);
}

string Epsilon_Transition::to_string() const{
  return "epsi";
}


//-------------------------Event-Transition-------------------------------------

Var_Transition::Var_Transition(State* const& ori, State* const& dest,
                                   vector<Variable> allocs,
                                   vector<Variable> freez, 
                                   vector<Variable> triggers):
  Transition(ori,dest,allocs,freez),triggers(triggers){}
  
Var_Transition::Var_Transition(State* const& ori, State* const& dest, 
                                   vector<Variable> triggers):
  Transition(ori,dest), triggers(triggers){}
  
Var_Transition::~Var_Transition() = default;

Valuation Var_Transition::accept_value(Valuation memory, string event){
  memory.alloc(this->allocations);
  if(!memory.use(this->triggers, event)){
    return Valuation();
  }
  memory.desalloc(this->frees);
  return memory;
}

Transition* Var_Transition::copy() const{
  return new Var_Transition(*this);
}

string Var_Transition::to_string() const{
  stringstream res;
  for(Variable const& var : this->triggers){
    res << var.to_string() <<" ";
  }
  return res.str();
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

Transition* Constant_Transition::copy() const{
  return new Constant_Transition(*this);
}

string Constant_Transition::to_string() const{
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

Transition* Universal_Transition::copy() const{
  return new Universal_Transition(*this);
}


string Universal_Transition::to_string() const{
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

//---------------------Fusion of Automata---------------------------------------

Automate *concatenation(Automate & prefix, Automate & suffix){
  Automate * generated_autom = new Automate();
  map<State*,State*> prefix_states,suffix_states;
  for(State & state : prefix.states){
    generated_autom->states.push_back(state);
    prefix_states.insert(pair<State*,State*>(&state,&(*(--(generated_autom->states.end())))));
  }
  for(State & state : suffix.states){
    generated_autom->states.push_back(state);
    suffix_states.insert(pair<State*,State*>(&state,&(*(--(generated_autom->states.end())))));
  }
  generated_autom->start = prefix_states.at(prefix.start);
  for(State * end_state : suffix.endStates){
    generated_autom->endStates.push_back(suffix_states.at(end_state));
  }
  for(pair<State*,vector<Transition*>> elmt : prefix.transitions){
    vector<Transition*> trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                        prefix_states.at(elmt.first),{})).first->second;
    for(Transition* const& prefix_trans : elmt.second){
      Transition* new_trans = prefix_trans->copy();
      new_trans->origine = prefix_states.at(prefix_trans->origine);
      for(State* endState : prefix.endStates){
        if(endState == new_trans->destination){
          Transition * end_trans = new_trans->copy();
          end_trans->destination = suffix_states.at(suffix.start);
          trans_vec.push_back(end_trans);
          break;
        }
      }
      new_trans->destination = prefix_states.at(prefix_trans->destination);
      trans_vec.push_back(new_trans);
    } 
  }
  for(pair<State*,vector<Transition*>> elmt : suffix.transitions){
    vector<Transition*> trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                      suffix_states.at(elmt.first),{})).first->second;
    for(Transition* const& suffix_trans : elmt.second){
      Transition* new_trans = suffix_trans->copy();
      trans_vec.push_back(new_trans);
      new_trans->origine = suffix_states.at(suffix_trans->origine);
      new_trans->destination = suffix_states.at(suffix_trans->destination);
    } 
  }
  
  return generated_autom;
}

Automate *disjonction(Automate & automate1, Automate & automate2){

}

Automate *intersection(Automate & automate1, Automate & automate2){

}

Automate *iteration(Automate & automate, vector<int> evolving_layers){

}
