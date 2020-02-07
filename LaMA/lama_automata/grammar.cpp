#include "grammar.hpp"
#include <sstream>
#include <iostream>

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
  nb_variable(nb_variables), value(nb_layers)
{
  for(unsigned int i = 0; i< nb_layers; i++){
    value[i] = vector<pair<bool,unordered_set<string>>>(nb_variables);
    for(unsigned int j = 0; j< nb_variables; j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
    }
  }
}

Valuation::Valuation(vector<vector<vector<string>>> const& source, int nb_vars):
  nb_variable(nb_vars), value(source.size())
{
  for(unsigned int i = 0; i< source.size(); i++){
    value[i] = vector<pair<bool,unordered_set<string>>>(nb_variable);
    unsigned int j(0);
    for(; j< source[i].size(); j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
      value[i][j].second.insert(source[i][j].begin(),source[i][j].end());
    }
    for(;j< nb_variable; j++){
      value[i][j] = pair<bool,unordered_set<string>>(false,unordered_set<string>());
    }
  }
}

Valuation::Valuation(Valuation const& original):
  nb_variable(original.nb_variable), value(original.value){}

Valuation::~Valuation()= default;

unsigned int Valuation::nb_layers() const { return value.size(); }

unsigned int Valuation::nb_variables() const { return nb_variable; }

//TODO optimize
bool Valuation::isFresh(int const& layer, string const& event) const{
  for(unsigned int i = 0; i < nb_variable; i++){
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
  res.nb_variable = (nb_variable< val2.nb_variable)?val2.nb_variable:nb_variable;
  unsigned int common_layers = (nb_layers()> val2.nb_layers())?val2.nb_layers():nb_layers();
  unsigned int common_vars = (nb_variable> val2.nb_variable)?val2.nb_variable:nb_variable;
  unsigned int i=0;
  for(; i< common_layers;i++){
    unsigned int j = 0;
    for(; j < common_vars; j++){
      res.value[i][j].second.insert(val2.value[i][j].second.begin(),val2.value[i][j].second.end());
    }
    for(; j < val2.nb_variable; j++){
      res.value[i][j] = val2.value[i][j];
    }
  }
  for(; i < val2.nb_layers(); i++){
    unsigned int j = 0;
    for(; j< val2.nb_variable;j++){
      res.value[i][j] = val2.value[i][j];
    }
    for(;j < this->nb_variable; j++){
      res.value[i][j] = pair<bool,unordered_set<string>>(false, unordered_set<string>());
    }
  }
  return res;
}

bool Valuation::operator==(Valuation const& val2) const{
  return value == val2.value;
}

string Valuation::to_string() const{
  stringstream res;
  for(unsigned int i = 0; i < nb_layers(); i++){
    res << "L"<< i << ": ";
    for(unsigned int j = 0; j < nb_variable; j++){
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

Valuation Transition::accept_value(Valuation memory, string value){
  return Valuation();
}

Valuation Transition::accept_constant(Valuation memory, string constant){
  return Valuation();
}

bool Transition::is_epsilon() const {return false;}

//Method used to combine transitions.
Transition * Transition::intersect_trans(Transition *trans){return nullptr;}

Transition * Transition::intersect_trans(Var_Transition *trans) { 
  return nullptr;
}

Transition * Transition::intersect_trans(Constant_Transition *trans) { 
  return nullptr;
}

Transition * Transition::intersect_trans(Universal_Transition *trans) { 
  return nullptr;
}

Transition * Transition::intersect_trans(Epsilon_Transition *trans){ 
  return nullptr;
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

bool Epsilon_Transition::is_epsilon() const {return true;}

Transition* Epsilon_Transition::copy() const{
  return new Epsilon_Transition(*this);
}

string Epsilon_Transition::to_string() const{
  return "epsi";
}

//-------------------------Var-Transition-------------------------------------

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

//Method used to combine transitions.
Transition * Var_Transition::intersect_trans(Transition *trans){
  return trans->intersect_trans(this);
}

Transition * Var_Transition::intersect_trans(Var_Transition *trans) { 
  Var_Transition * res = new Var_Transition(*this);
  res->allocations.insert(trans->allocations.end(),trans->allocations.begin(),trans->allocations.end());
  res->frees.insert(trans->frees.end(),trans->frees.begin(),trans->frees.end());
  res->triggers.insert(trans->triggers.end(),trans->triggers.begin(), trans->triggers.end());
  return res;
}

Transition * Var_Transition::intersect_trans(Universal_Transition *trans) { 
  Var_Transition * res = new Var_Transition(*this);
  res->allocations.insert(trans->allocations.end(),trans->allocations.begin(),trans->allocations.end());
  res->frees.insert(trans->frees.end(),trans->frees.begin(),trans->frees.end());
  return res;
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

Valuation Constant_Transition::accept_constant(Valuation memory, string const_in){
  if(constant != const_in){
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

//Method used to combine transitions.
Transition * Constant_Transition::intersect_trans(Transition *trans){
  return trans->intersect_trans(this);
}

Transition * Constant_Transition::intersect_trans(Constant_Transition *trans) {
  if(this->constant!=trans->constant){return nullptr;} 
  Constant_Transition * res = new Constant_Transition(*this);
  res->allocations.insert(trans->allocations.end(),trans->allocations.begin(),trans->allocations.end());
  res->frees.insert(trans->frees.end(),trans->frees.begin(),trans->frees.end());
  return res;
}

Transition * Constant_Transition::intersect_trans(Universal_Transition *trans) { 
  Constant_Transition * res = new Constant_Transition(*this);
  res->allocations.insert(trans->allocations.end(),trans->allocations.begin(),trans->allocations.end());
  res->frees.insert(trans->frees.end(),trans->frees.begin(),trans->frees.end());
  return res;
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

Valuation Universal_Transition::accept_constant(Valuation memory, string constant){
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

//Method used to combine transitions.
Transition * Universal_Transition::intersect_trans(Transition *trans){
  return trans->intersect_trans(this);
}

Transition * Universal_Transition::intersect_trans(Var_Transition *trans) {
  Var_Transition * res = new Var_Transition(*trans);
  res->allocations.insert(this->allocations.end(),this->allocations.begin(),this->allocations.end());
  res->frees.insert(this->frees.end(),this->frees.begin(),this->frees.end());
  return res;
}

Transition * Universal_Transition::intersect_trans(Constant_Transition *trans) {
  Constant_Transition * res = new Constant_Transition(*trans);
  res->allocations.insert(this->allocations.end(),this->allocations.begin(),this->allocations.end());
  res->frees.insert(this->frees.end(),this->frees.begin(),this->frees.end());
  return res;
}

Transition * Universal_Transition::intersect_trans(Universal_Transition *trans) { 
  Universal_Transition * res =new Universal_Transition(*this);
  res->allocations.insert(trans->allocations.end(),trans->allocations.begin(),trans->allocations.end());
  res->frees.insert(trans->frees.end(),trans->frees.begin(),trans->frees.end());
  return res;
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

string Automate::to_string() const{
  stringstream res;
  res << "Initial Valuation :\n" << initial_valuation.to_string();
  res <<"\n";
  res << "List of states : " <<states.size() <<"\n";
  for(State const& st : states){
    res << "\t" << st.id << " ";
    if(&st==start){ res << "#S ";}
    for(State * endState : endStates) {
      if(&st == endState) {
        res << "#E";
        break;
      }
    }
    res << "\n";
  }
  res << "List of edges : ";
  int number_edges(0);
  for(auto const& elmt : transitions)
    number_edges+= elmt.second.size();
  res << number_edges << "\n";
  for(auto elmt : transitions){
    for(Transition * trans : elmt.second){
      res << "\t" << trans->origine->id << "->" << trans->destination->id << " : ";
      res << trans->to_string() ;

      if(!trans->allocations.empty()){
        res << "  nu : {" << trans->allocations[0].to_string();
        for(unsigned int i = 1;i< trans->allocations.size(); i++){
          res << ", " << trans->allocations[i].to_string();
        }
        res << "}";
      }
      if(!trans->frees.empty()){
        res << "  free : {" << trans->frees[0].to_string();
        for(unsigned int i = 1;i< trans->frees.size(); i++){
          res << ", " << trans->frees[i].to_string();
        }
        res << "}";
      }
      res << "\n";
    }
  }
  return res.str();
}

//---------------------Fusion of Automata---------------------------------------

Automate *automate::concatenation(Automate * prefix, Automate * suffix){
  Automate * generated_autom = new Automate();
  //Copy states in the new automaton. Create dictionnary for pointers
  map<State*,State*> prefix_states,suffix_states;
  for(State & state : prefix->states){
    generated_autom->states.push_back(state);
    prefix_states[&state] =&*--(generated_autom->states.end());
  }
  for(State & state : suffix->states){
    generated_autom->states.push_back(state);
    suffix_states[&state]=&*--(generated_autom->states.end());
  }
  //Set initial and final states
  generated_autom->start = prefix_states.at(prefix->start);
  for(State * end_state : suffix->endStates){
    generated_autom->endStates.push_back(suffix_states.at(end_state));
  }
  //Copy the transitions of the prefix automaton
  for(pair<State*,vector<Transition*>> elmt : prefix->transitions){
    vector<Transition*>& trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                        prefix_states.at(elmt.first),{})).first->second;
    for(Transition* const& prefix_trans : elmt.second){
      Transition* new_trans = prefix_trans->copy();
      new_trans->origine = prefix_states.at(prefix_trans->origine);
      //Each final transitions a copied an other time to head to the initial 
      //state of the suffix automaton.
      for(State* endState : prefix->endStates){
        if(endState == new_trans->destination){
          Transition * end_trans = new_trans->copy();
          end_trans->destination = suffix_states.at(suffix->start);
          trans_vec.push_back(end_trans);
          break;
        }
      }
      new_trans->destination = prefix_states.at(prefix_trans->destination);
      trans_vec.push_back(new_trans);
    } 
  }
  // Copy the transitions of the suffix automaton.
  for(pair<State*,vector<Transition*>> elmt : suffix->transitions){
    vector<Transition*>& trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                      suffix_states.at(elmt.first),{})).first->second;
    for(Transition* const& suffix_trans : elmt.second){
      Transition* new_trans = suffix_trans->copy();
      trans_vec.push_back(new_trans);
      new_trans->origine = suffix_states.at(suffix_trans->origine);
      new_trans->destination = suffix_states.at(suffix_trans->destination);
    } 
  }
  //Create the initial valuation of the automaton.
  generated_autom->initial_valuation = prefix->initial_valuation.join(suffix->initial_valuation);
  
  //Fusion constants alphabet
  generated_autom->constants.insert(prefix->constants.begin(),prefix->constants.end());
  generated_autom->constants.insert(suffix->constants.begin(),suffix->constants.end());
  return generated_autom;
}

Automate *automate::disjonction(Automate * autom1, Automate * autom2){
  Automate * generated_autom = new Automate();
  //Copy states in the new automaton. Create dictionnary for pointers
  map<State*,State*> autom1_states,autom2_states;
  for(State & state : autom1->states){
    generated_autom->states.push_back(state);
    autom1_states[&state]=&*--(generated_autom->states.end());
  }
  for(State & state : autom2->states){
    generated_autom->states.push_back(state);
    autom2_states[&state] = &*--(generated_autom->states.end());
  }
  //Set final states
  for(State * end_state : autom1->endStates){
    generated_autom->endStates.push_back(autom1_states.at(end_state));
  }
  for(State * end_state : autom2->endStates){
    generated_autom->endStates.push_back(autom2_states.at(end_state));
  }
  //Set initial state and initial transitions.
  generated_autom->states.push_back(State(autom1->start->id +"|"+autom2->start->id));
  generated_autom->start = &*(--generated_autom->states.end());
  generated_autom->transitions[generated_autom->start].push_back(new Epsilon_Transition(generated_autom->start,autom1_states[autom1->start]));
  generated_autom->transitions[generated_autom->start].push_back(new Epsilon_Transition(generated_autom->start,autom2_states[autom2->start]));
  //Copy the transitions of the autom1 automaton
  for(pair<State*,vector<Transition*>> elmt : autom1->transitions){
    vector<Transition*>& trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                        autom1_states.at(elmt.first),{})).first->second;
    for(Transition* const& autom1_trans : elmt.second){
      Transition* new_trans = autom1_trans->copy();
      new_trans->origine = autom1_states.at(autom1_trans->origine);
      new_trans->destination = autom1_states.at(autom1_trans->destination);
      trans_vec.push_back(new_trans);
    } 
  }
  // Copy the transitions of the autom2 automaton.
  for(pair<State*,vector<Transition*>> elmt : autom2->transitions){
    vector<Transition*>& trans_vec = generated_autom->transitions.
        insert(pair<State*,vector<Transition*>>(
                      autom2_states.at(elmt.first),{})).first->second;
    for(Transition* const& autom2_trans : elmt.second){
      Transition* new_trans = autom2_trans->copy();
      new_trans->origine = autom2_states.at(autom2_trans->origine);
      new_trans->destination = autom2_states.at(autom2_trans->destination);
      trans_vec.push_back(new_trans);
    } 
  }
  //Create the initial valuation of the automaton.
  generated_autom->initial_valuation = autom1->initial_valuation.join(autom2->initial_valuation);
  
  //Fusion constants alphabet
  generated_autom->constants.insert(autom1->constants.begin(),autom1->constants.end());
  generated_autom->constants.insert(autom2->constants.begin(),autom2->constants.end());
  return generated_autom;
}

// TODO : À optimiser
Automate *intersection(Automate * autom1, Automate * autom2){
  Automate * generated_autom = new Automate();
  //Copy states in the new automaton. Create dictionnary for pointers
  map<State*,map<State*,State*>> dic_states;
  for(State & autom1_state : autom1->states){
    for(State & autom2_state : autom2->states){
      generated_autom->states.push_back(State(autom1_state.id +"&"+ autom2_state.id));
      dic_states[&autom1_state][&autom2_state] = &*--(generated_autom->states.end());
    }
  }
  
  //Set initial/final states to the product of the same states of both automata
  generated_autom->start = dic_states[autom1->start][autom2->start];
  for(State* endState1 : autom1->endStates){
    for(State* endState2 : autom2->endStates){
      generated_autom->endStates.push_back(dic_states[endState1][endState2]);
    }
  }
  
  //Create copy of the transitions
  for(auto elmt1 : autom1->transitions){
    for(Transition * trans1 : elmt1.second){
      if(trans1->is_epsilon()){
        for(State & st2 : autom2->states){
          Transition * new_trans = trans1->copy();
          new_trans->origine = dic_states[trans1->origine][&st2];
          new_trans->destination = dic_states[trans1->destination][&st2];
          generated_autom->transitions[new_trans->origine].push_back(new_trans);
        }
      }else{
        for(auto elmt2 : autom2->transitions){
          for(Transition * trans2: elmt2.second){
            Transition * new_trans = trans1->intersect_trans(trans2);
            if(new_trans != nullptr){
              new_trans->origine = dic_states[trans1->origine][trans2->origine];
              new_trans->destination = dic_states[trans1->destination][trans2->destination];
              generated_autom->transitions[new_trans->origine].push_back(new_trans);
            }
          }
        }
      }
    }
  }
  for(auto elmt2 : autom2->transitions){
    for(Transition * trans2: elmt2.second){
      if(trans2->is_epsilon()){
        for(State & st1 : autom1->states){
          Transition * new_trans = trans2->copy();
          new_trans->origine = dic_states[&st1][trans2->origine];
          new_trans->destination = dic_states[&st1][trans2->destination];
          generated_autom->transitions[new_trans->origine].push_back(new_trans);
        }
      }
    }
  }
  
  //Set initial Valuation
  generated_autom->initial_valuation = autom1->initial_valuation.join(autom2->initial_valuation);
  
  //Fusion constants alphabet
  generated_autom->constants.insert(autom1->constants.begin(),autom1->constants.end());
  generated_autom->constants.insert(autom2->constants.begin(),autom2->constants.end());
  return generated_autom;
}
/*
enum var_state{ U, A, OU, OA};

Automate *iteration(Automate * automate, vector<int> evolving_layers){
  //Initialization of structures;
  Automate * generated_autom = new Automate();
  map<State*, map<var_state,State*>> dic_states;
  vecotr<pair<State*,var_state>> to_do;
  //Creation of the initial state
  generated_autom->states.push_back(State(automate->start->id + "U"));
  generated_autom->start = &*(--generated_autom->states.end());
  to_do.push_back(pair<State*,var_state>(automate->start,U));
  dic_states[automate->start][U] = generated_autom->start;
  
  //Breadth First search state/transition generation
  while(! to_do.empty()){
    pair<State*, var_state> doing = to_do.pop_back();
    State *curr_state = dic_states[doing.first][doing.second];
    if(!generated_autom->transitions[curr_state].empty()){
      continue;
    }
    
  }
}*/
