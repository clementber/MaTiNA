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
  vector<Clock> res;
  for(Clock const& clk : clocks_status){
    try{
      Interval & constraint = this->clocks_constraints.at(clk.name);
      Interval intersection = Interval::intersect(clk.value, constraint);
      if(!intersection.isValid()){
        return {};
      }
      Clock new_clock(clk.name);
      new_clock.value = intersection;
      new_clock.increm = clk.increm;
      res.push_back(new_clock);
    }catch(const out_of_range& oor){res.push_back(clk);}
  }
  return res;
}

Clock::Clock(string const& p_name): name(p_name), value(0){}
Clock::~Clock() = default;

void Clock::reset() {
  this->value=0;
}

void Clock::increment(Interval increm){
  this->value += increm;
}

void Clock::validate(){this->increm.validate();}

bool Clock::operator==(Clock const& c2){
  return this->name==c2.name;
}

void Clock::print() const{
  cout << " " << (value.include_inf<=0?"[":"]") << (value.include_inf<0?"-":(value.include_inf>1?"+":""));
  cout << value.borne_inf << "," << value.borne_sup;
  cout << (value.include_sup<-1?"-":(value.include_sup>0?"+":"")) <<(value.include_sup>=0?"]":"[");
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
           vector<string> const& clock_to_reset) : origine(ori), destination(dest),
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
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, Interval const& increm){
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
    res << clocks_to_reset[0] << ":=0";
    for(unsigned int i =1; i<clocks_to_reset.size(); i++){
      res << "," << clocks_to_reset[i] << ":=0";
    }
  }
  return res.str();
}

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, map<string,Interval> const& clocks_interv,
           vector<string> const& clock_to_reset)
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

//The Epsilon_Transition are the only transition
//possibly triggered after a time elapse.
pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Epsilon_Transition::accept_epsilon(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, Interval const& increm){
  //Time management
  vector<Clock> clock_res;
  if(!value.first.empty()){
    Interval inf_increm(0,numeric_limits<double>::max()), sup_increm(0,numeric_limits<double>::max());
    for(Clock const& clk : value.first){
      //Calcul des incréments possibles pour chaque bornes.
      try{
        Interval const& constraint = this->clocks_constraints.at(clk.name);
        if(constraint.borne_inf - clk.value.borne_inf > inf_increm.borne_inf){
          inf_increm.borne_inf = constraint.borne_inf - clk.value.borne_inf;
          inf_increm.include_inf = constraint.include_inf - clk.value.include_inf;
        }else{
          if(constraint.borne_inf - clk.value.borne_inf == inf_increm.borne_inf){
            inf_increm.include_inf = max(constraint.include_inf - clk.value.include_inf,inf_increm.include_inf);
          }
        }
        if(constraint.borne_sup - clk.value.borne_inf < inf_increm.borne_sup){
          inf_increm.borne_sup = constraint.borne_sup - clk.value.borne_inf;
          inf_increm.include_sup = constraint.include_sup - clk.value.include_inf;
        }else{
          if(constraint.borne_sup - clk.value.borne_inf == inf_increm.borne_sup){
            inf_increm.include_sup = min(constraint.include_sup - clk.value.include_inf,inf_increm.include_sup);
          }
        }
        if(constraint.borne_inf - clk.value.borne_sup > inf_increm.borne_inf){
          sup_increm.borne_inf = constraint.borne_inf - clk.value.borne_sup;
          sup_increm.include_inf = constraint.include_inf - clk.value.include_sup;
        }else{
          if(constraint.borne_inf - clk.value.borne_sup == sup_increm.borne_inf){
            sup_increm.include_inf = max(constraint.include_inf - clk.value.include_sup,sup_increm.include_inf);
          }
        }
        if(constraint.borne_sup - clk.value.borne_sup < sup_increm.borne_sup){
          sup_increm.borne_sup = constraint.borne_sup - clk.value.borne_sup;
          sup_increm.include_sup = constraint.include_sup - clk.value.include_sup;
        }else{
          if(constraint.borne_sup - clk.value.borne_sup == sup_increm.borne_sup){
            sup_increm.include_sup = min(constraint.include_sup - clk.value.include_sup,sup_increm.include_sup);
          }
        }
      }catch(out_of_range const& oor){}
    }
    //Last check on the token in it's final state.
    clock_res = this->destination->accept(clock_res);
    if(clock_res.empty()){
      return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
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
           vector<string> const& clock_to_reset, int p_variable)
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

pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Event_Transition::accept_event(
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
    string p_event){
  //Time management
  vector<Clock> clock_res;
  if( ! value.first.empty()){
    for(auto & clk : value.first){
      Clock new_clock(clk.name);
      try{
        Interval & constraint = this->clocks_constraints.at(clk.name);
        Interval intersection = Interval::intersect(clk.value, constraint);
        if(intersection.borne_inf == -1){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
        new_clock.value = intersection;
      }catch(const out_of_range& oor){new_clock = clk;}
      for(auto clock_to_reset : this->clocks_to_reset){
        if(clock_to_reset == new_clock.name){
          new_clock.reset();
          break;
        }
      }
      clock_res.push_back(new_clock);
    }
    clock_res = this->destination->accept(clock_res);
    if(clock_res.empty()){
      return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
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
           vector<string> const& clock_to_reset, string p_constant)
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
pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> Constant_Transition::accept_constant(
      pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
      string constant){
  if(constant != this->constant){
    return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
  }
  //Time management
  vector<Clock> clock_res;
  if(! value.first.empty()){
    for(auto & clk : value.first){
      Clock new_clock(clk.name);
      try{
        Interval & constraint = this->clocks_constraints.at(clk.name);
        Interval intersection = Interval::intersect(clk.value, constraint);
        if(intersection.borne_inf == -1){
          return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
        }
        new_clock.value = intersection;
      }catch(const out_of_range& oor){new_clock = clk;}
      for(auto clock_to_reset : this->clocks_to_reset){
        if(clock_to_reset == new_clock.name){
          new_clock.reset();
          break;
        }
      }
      clock_res.push_back(new_clock);
    }
    clock_res = this->destination->accept(clock_res);
    if(clock_res.empty()){
      return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>({},{});
    }
  }
  //Memory management
  vector<pair<bool,unordered_set<string>>> memory_res(value.second);
  alloc(this->allocations, memory_res);
  desalloc(this->frees, memory_res);
  return pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>>(clock_res, memory_res);
}

Automate::Automate() = default;

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
