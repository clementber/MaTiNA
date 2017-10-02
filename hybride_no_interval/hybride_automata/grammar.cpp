#include "grammar.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace automate;
using namespace std;

Interval::Interval() : borne_inf(-1), borne_sup(-1),
                       include_inf(0), include_sup(0) {}
Interval::Interval(double constant):include_inf(0), include_sup(0){
  borne_inf = borne_sup = constant;
}
Interval::Interval(double born_inf, double born_sup){
  borne_inf = born_inf;
  borne_sup = born_sup;
  include_inf = include_sup = 0;
}
Interval::Interval(double born_inf, bool incl_inf, double born_sup, bool incl_sup):
                          include_inf(incl_inf?0:1), include_sup(incl_sup?0:-1){
  borne_inf = born_inf;
  borne_sup = born_sup;
}
Interval::Interval(double born_inf, int incl_inf, double born_sup, int incl_sup){
  borne_inf = born_inf;
  borne_sup = born_sup;
  include_inf = incl_inf;
  include_sup = incl_sup;
}

// SUBSET !!
bool Interval::operator<=(Interval const& interv){
  Interval left(Interval::validate(*this)), right(Interval::validate(interv));
  return (((left.borne_inf > right.borne_inf) ||
            (left.borne_inf==right.borne_inf
            && left.include_inf>=right.include_inf))
       && ((left.borne_sup < right.borne_sup) ||
            (left.borne_sup==right.borne_sup
            && left.include_sup<=right.include_sup)));
}

void Interval::operator=(double const val){
  this->borne_inf = val;
  this->borne_sup = val;
  this->include_inf = 0;
  this->include_sup = 0;
}

bool Interval::operator==( Interval const& interv) const{
  return (this->borne_inf == interv.borne_inf
          && this->borne_sup == interv.borne_sup
          && this->include_inf == interv.include_inf
          && this->include_sup == interv.include_sup)
      ||(this->borne_inf == interv.borne_sup
          && this->borne_sup == interv.borne_inf
          && this->include_inf == interv.include_sup
          && this->include_sup == interv.include_inf);
}

Interval Interval::operator+(Interval const& interv) const{
  Interval res;
  res.borne_inf = this->borne_inf+interv.borne_inf;
  res.borne_sup = this->borne_sup+interv.borne_sup;
  res.include_inf = this->include_inf + interv.include_inf;
  res.include_sup = this->include_sup + interv.include_sup;
  return res;
}

void Interval::operator+=(Interval const& interv){
  *this = *this + interv;
}

void Interval::operator+=(double const val){
  this->borne_inf+=val;
  this->borne_sup+=val;
}

bool Interval::isValid(){
  return (borne_inf > 0 || (borne_inf == 0 && include_inf>=0))
  && (borne_inf < borne_sup || (borne_inf==borne_sup && (include_inf <= include_sup)));
}

void Interval::validate(){
  if(!isValid()){
    double temp_borne = borne_inf;
    borne_inf = borne_sup;
    borne_sup = temp_borne;
    int temp_include = include_inf;
    include_inf = include_sup;
    include_sup = temp_include;
  }
}

State::State(string identifiant,
             map<string,Interval> clock_constraints) :
               id(identifiant), clocks_constraints(clock_constraints){ }
State::State(string identifiant):id(identifiant),clocks_constraints() { }
State::State() = default;
State::~State() = default;

vector<Clock> State::accept(vector<Clock> const& clocks_status){
  for(Clock const& clk : clocks_status){
    try{
      Interval & constraint = this->clocks_constraints.at(clk.name);
      if(clk.value > constraint.borne_sup
        || (clk.value == constraint.borne_sup
           && clk.include > constraint.include_sup)
        || clk.value < constraint.borne_inf
        || (clk.value == constraint.borne_inf
           && clk.include < constraint.include_inf)){
        return {};
      }
    }catch(const out_of_range& oor){}
  }
  return clocks_status;
}

Clock::Clock(string const& p_name): name(p_name), value(0), include(0){}
Clock::~Clock() = default;

void Clock::reset() {
  this->value=0;
  this->include = 0;
}

bool Clock::operator==(Clock const& c2){
  return this->name==c2.name;
}

void Clock::print() const{
  cout << "{" << value << (include > 0? "+" : (include < 0? "-" :"")) << "}";
}

//----------------------------------------------------------------------------//
//-----------------------Utilitaire-------------------------------------------//
//----------------------------------------------------------------------------//

void merge_constraints(map<string,Interval> & map1, map<string,Interval> const& map2){
  for(auto map2_element : map2){
    if(! map1.insert(map2_element).second){
      map1[map2_element.first] = Interval::intersect(map1[map2_element.first], map2_element.second);
    }
  }
}

bool isFresh(vector<pair<bool,unordered_set<string>>> const& memory, string const& event){
  for(auto const& variable : memory){
    if(variable.second.find(event) != variable.second.end()){
      return false;
    }
  }
  return true;
}

bool use(int ressource,
          vector<pair<bool,unordered_set<string>>> & memory,
          string const& event){
  pair<bool,unordered_set<string>> & variable = memory[ressource];
  if(variable.first){
    if(!isFresh(memory, event)){
      return false;
    }
    variable.first = false;
    variable.second.insert(event);
    return true;
  }
  return variable.second.find(event) != variable.second.end();
}

void alloc(vector<int> ressources, vector<pair<bool,unordered_set<string>>> & memory){
  for(uint ressource : ressources){
    memory[ressource].first = true;
  }
}

void desalloc(vector<int> ressources, vector<pair<bool,unordered_set<string>>> & memory){
  for(uint ressource : ressources){
    memory[ressource].first = false;
    memory[ressource].second.clear();
  }
}

//----------------------------------------------------------------------------//
//-----------------------Transition-------------------------------------------//
//----------------------------------------------------------------------------//

Transition::Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, map<string,Interval> const& clocks_interv,
           unordered_set<string> const& clock_to_reset) : origine(ori), destination(dest),
           allocations(allocs), frees(freez), clocks_to_reset(clock_to_reset){
  clocks_constraints = map<string, Interval>(clocks_interv);
  merge_constraints(clocks_constraints, ori->clocks_constraints);
  /**If the clock is reset there is no need to verify the destination's
   * constraints before we reset the clock.
   */
  map<string,Interval> dest_constraints(dest->clocks_constraints);
  for(auto clk_reseted : clock_to_reset){
    dest_constraints.erase(clk_reseted);
  }
  merge_constraints(clocks_constraints, dest_constraints);
}
Transition::Transition(State* const& ori, State* const& dest)
                               :origine(ori),destination(dest){
  merge_constraints(clocks_constraints, ori->clocks_constraints);
  merge_constraints(clocks_constraints, dest->clocks_constraints);
}
Transition::~Transition() = default;

pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Transition::accept_epsilon(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, double &increm, int &incl_increm){
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
}
pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Transition::accept_event(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
    string event){
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
}
pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Transition::accept_constant(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
    string constant){
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
}

string Transition::to_string(){
  stringstream res;
  if(!clocks_constraints.empty()){
    res << "<br/>";
    for(auto interv : clocks_constraints){
      res << "{" << interv.first << "&isin;[" << interv.second.borne_inf ;
      res << "," << interv.second.borne_sup  << "]}";
    }
  }
  if(!clocks_to_reset.empty()){
    res << "<br/>";
    bool first = true;
    for(string clk2Rst : clocks_to_reset){
      if(!first){
        res << ",";
      }
      res << clk2Rst << ":=0";
      first = false;
    }
  }
  return res.str();
}

Transition* Transition::copy(){
  return new Transition(*this);
}

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, map<string,Interval> const& clocks_interv,
           unordered_set<string> const& clock_to_reset)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset){}
Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest)
    : Transition(ori,dest){}
Epsilon_Transition::~Epsilon_Transition() = default;
string Epsilon_Transition::to_string(){
  stringstream res;
  if(!allocations.empty()){
    res << "&nu;{" << allocations[0] << "}";
    for(uint i = 1 ; i < allocations.size(); i++){
      res << ",&nu;{" << allocations[i] << "}";
    }
  }
  if(!frees.empty()){
    if(!allocations.empty()){
      res << ",";
    }
    res << "<O>&nu;</O>{" << frees[0] << "}";
    for(uint i=1; i < frees.size(); i++){
      res << ",<O>&nu;</O>{" << frees[i] << "}";
    }
  }
  return res.str() + Transition::to_string();
}
Transition* Epsilon_Transition::copy(){
  return new Epsilon_Transition(*this);
}

//The Epsilon_Transition are the only transition
//possibly triggered after a time elapse.
pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Epsilon_Transition::accept_epsilon(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, double & increm, int & incl_increm){
  //Time management
  vector<Clock> clock_res;
  if( ! value.first.empty()){
    double need_increm_val = 0;
    int need_increm_inc = 0;
    for(auto & clk : value.first){
      try{
        Interval const& constraint = this->clocks_constraints.at(clk.name);
        if(clk.value > constraint.borne_sup
          || (clk.value == constraint.borne_sup && clk.include > constraint.include_sup)){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
        if(need_increm_val < (constraint.borne_inf - clk.value)
          || (need_increm_val == (constraint.borne_inf - clk.value)
             && need_increm_inc < (constraint.include_inf - clk.include))){
            need_increm_val = (constraint.borne_inf - clk.value);
            need_increm_inc = (constraint.include_inf - clk.include);
        }
      }catch(const out_of_range & oor){}
    }
    if(need_increm_val > increm
      || (need_increm_val == increm && need_increm_inc > incl_increm)){
        return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
    }
    increm -= need_increm_val;
    incl_increm -= need_increm_inc;
    for(auto & clk : value.first){
      Clock new_clock(clk.name);
      new_clock.value = clk.value + need_increm_val;
      new_clock.include = clk.include + need_increm_inc;
      try{
        Interval const& constraint = this->clocks_constraints.at(clk.name);
        if(clk.value > constraint.borne_sup
          || (new_clock.value == constraint.borne_sup
            && new_clock.include > constraint.include_sup)){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
      }catch(const out_of_range& oor){}
      for(string clock_to_reset : clocks_to_reset){
        if(new_clock.name == clock_to_reset){
          try{
            Interval const& constraint =  destination->clocks_constraints.at(new_clock.name);
            if(constraint.borne_inf > 0 || (constraint.borne_inf == 0 && constraint.include_inf > 0)){
              return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
            }
          }catch(const out_of_range & oor){}
          new_clock.reset();
          break;
        }
      }
      clock_res.push_back(new_clock);
    }
  }
  //Memory management
  vector<pair<bool,unordered_set<string>>> memory_res = value.second;
  if( ! value.second.empty()){
    alloc(this->allocations, memory_res);
    desalloc(this->frees, memory_res);
  }
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>(clock_res,memory_res);
}


Event_Transition::Event_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, map<string,Interval> const& clocks_interv,
           unordered_set<string> const& clock_to_reset, int p_variable)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), variable(p_variable){}
Event_Transition::Event_Transition(State* const& ori, State* const& dest, int p_variable)
    : Transition(ori,dest), variable(p_variable){}
Event_Transition::~Event_Transition() = default;
string Event_Transition::to_string(){
  stringstream res;
  if(!allocations.empty()){
    for(uint i = 0 ; i < allocations.size(); i++){
      res << "&nu;{" << allocations[i] << "},";
    }
  }
  res <<"<B>{"<< variable << "}</B>";
  if(!frees.empty()){
    for(uint i=0; i < frees.size(); i++){
      res << ",<O>&nu;</O>{" << frees[i] << "}";
    }
  }
  return res.str() + Transition::to_string();
}

Transition* Event_Transition::copy(){
  return new Event_Transition(*this);
}

pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Event_Transition::accept_event(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
    string p_event){
  //Time management
  vector<Clock> clock_res;
  if( ! value.first.empty()){
    for(Clock & clk : value.first){
      Clock new_clock(clk);
      try{
        Interval const& constraint = this->clocks_constraints.at(clk.name);
        if(clk.value > constraint.borne_sup
          || (clk.value == constraint.borne_sup && clk.include > constraint.include_sup)
          ||clk.value < constraint.borne_inf
          || (clk.value == constraint.borne_inf && clk.include < constraint.include_inf)){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
      }catch(const out_of_range& oor){}
      for(string clk2Reset : clocks_to_reset){
        if(clk.name == clk2Reset){
          try{
            Interval const& dest_constraint = destination->clocks_constraints.at(clk2Reset);
            if(dest_constraint.borne_inf > 0
              || (dest_constraint.borne_inf == 0
                && dest_constraint.include_inf > 0)){
                  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
            }
          }catch(out_of_range const& oor){}
          new_clock.reset();
        }
        break;
      }
      clock_res.push_back(new_clock);
    }
  }
  //Memory management
  vector<pair<bool,unordered_set<string>>> memory_res(value.second);
  alloc(this->allocations, memory_res);
  if(!use(this->variable, memory_res, p_event)){
    return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
  }
  desalloc(this->frees, memory_res);
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>(clock_res, memory_res);
}

Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, map<string,Interval> const& clocks_interv,
           unordered_set<string> const& clock_to_reset, string p_constant)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), constant(p_constant){}
Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, string p_constant)
    : Transition(ori,dest), constant(p_constant){}
Constant_Transition::~Constant_Transition() = default;
string Constant_Transition::to_string(){
  stringstream res;
  res  << constant;
  if(!allocations.empty()){
    res << "<br/>&nu;{" << allocations[0] << "}";
    for(uint i = 1 ; i < allocations.size(); i++){
      res << ",&nu;{" << allocations[i] << "}";
    }
  }
  if(!frees.empty()){
    if(!allocations.empty()){
      res << ",";
    }else{
      res << "<br/>";
    }
    res << "<O>&nu;</O>{" << frees[0] << "}";
    for(uint i=1; i < frees.size(); i++){
      res << ",<O>&nu;</O>{" << frees[i] << "}";
    }
  }
  return res.str() + Transition::to_string();
}
Transition* Constant_Transition::copy(){
  return new Constant_Transition(*this);
}

pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Constant_Transition::accept_constant(
      pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
      string constant){
  if(constant != this->constant){
    return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
  }
  //Time management
  vector<Clock> clock_res;
  if( ! value.first.empty()){
    for(auto & clk : value.first){
      Clock new_clock(clk);
      try{
        Interval const& constraint = this->clocks_constraints.at(clk.name);
        if(clk.value > constraint.borne_sup
          || (clk.value == constraint.borne_sup && clk.include > constraint.include_sup)
          ||clk.value < constraint.borne_inf
          || (clk.value == constraint.borne_inf && clk.include < constraint.include_inf)){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
      }catch(const out_of_range& oor){}
      for(string clk2Reset : clocks_to_reset){
        if(clk.name == clk2Reset){
          try{
            Interval const& dest_constraint = destination->clocks_constraints.at(clk2Reset);
            if(dest_constraint.borne_inf > 0
              || (dest_constraint.borne_inf == 0
                && dest_constraint.include_inf > 0)){
                  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
                }
          }catch(out_of_range const& oor){}
          new_clock.reset();
          break;
        }
      }
      clock_res.push_back(new_clock);
    }
  }
  //Memory management
  vector<pair<bool,unordered_set<string>>> memory_res(value.second);
  alloc(this->allocations, memory_res);
  desalloc(this->frees, memory_res);
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>(clock_res, memory_res);
}

Automate::Automate(): ressources(0), states(), clocks(), transitions(), constants(),
                      start(), endStates() {};

Automate::Automate(int p_ressources,
         list<State> p_states,
         vector<Clock> p_clocks,
         map<State*, vector<Transition*>> p_transitions,
         unordered_set<string> p_constants,
         State* p_start,
         vector<State*> p_endStates) :
         ressources(p_ressources), states(p_states), clocks(p_clocks),
         transitions(p_transitions), constants(p_constants),
         start(p_start), endStates(p_endStates) {}

Automate::~Automate() {
   for(auto & pair : transitions){
     for(Transition * trans : pair.second){
       delete trans;
     }
   }
 }

Clock* Automate::find_or_create_clock(string name){
  for(Clock & existing_clock : this->clocks){
    if(existing_clock.name == name)
      return &existing_clock;
  }
  Clock new_clock(name);
  this->clocks.push_back(new_clock);
  vector<Clock>::iterator ite(this->clocks.end());
  ite--;
  return &(*ite);
}
State* Automate::getState(string state_name){
  for(State& state : this->states){
    if(state.id == state_name){
      return &state;
    }
  }
  return nullptr;
}
