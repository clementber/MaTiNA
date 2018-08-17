#include "grammar.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <limits>
using namespace automate;
using namespace std;

Bound::Bound(): value(numeric_limits<double>::max()),inclusion(-1){}
Bound::Bound(double value): value(value), inclusion(0){}
Bound::Bound(double value, int included): value(value), inclusion(included){}

Bound Bound::operator+(Bound const& bound2) const{
  if(this->value == numeric_limits<double>::max()
  || bound2.value == numeric_limits<double>::max()){
    return Bound();
  }
  return Bound(this->value + bound2.value, this->inclusion + bound2.inclusion);
}

bool Bound::operator==(Bound const& bound2)const{
  return value==bound2.value && inclusion==bound2.inclusion;
}

bool Bound::operator!=(Bound const& bound2)const{
  return !(*this==bound2);
}

bool Bound::operator<(Bound const& bound2) const{
  return (this->value < bound2.value)
      || ((this->value == bound2.value)&& (this->inclusion < bound2.inclusion));
}

bool Bound::operator<=(Bound const& bound2) const{
  return (this->value < bound2.value)
      || ((this->value == bound2.value)&& (this->inclusion <= bound2.inclusion));
}

Bound Bound::min(Bound const& bound2) const{
  if(*this < bound2.value)
    return *this;
  else
    return bound2;
}

DBM::DBM():DBM(0){}
DBM::DBM(DBM const& source):length(source.length){
  matrice = vector<vector<Bound>>(source.matrice);
}
DBM::DBM(int clocks_number):length(clocks_number+1){
  for(int i = 0; i<= clocks_number; i++){
    matrice.push_back(std::vector<Bound>());
    for(int j=0; j<= clocks_number; j++){
      matrice[i].push_back((i==j||i==0)?Bound(0):Bound());
    }
  }
}
DBM::DBM(Automate const& autom):length(autom.clocks.size()+1){
  for(int i=0;i<length;i++){
    matrice.push_back(vector<Bound>(length,Bound(0)));
  }
}

int DBM::getClocks_number() const {return length-1;}

void DBM::addClock(){
  matrice[0].push_back(Bound(0));
  for(int i = 1; i < length; i++){
    matrice[i].push_back(Bound());
  }
  length++;
  matrice.push_back(vector<Bound>(length,Bound()));
  matrice[length-1][length-1] = Bound(0);
}

//Time modification operators
void DBM::increment(double time_delay){
  for(int i=1; i<length;i++){
    matrice[0][i].value -= time_delay;
    matrice[i][0].value += time_delay;
  }
}

void DBM::reset(unordered_set<Clock*> clks){
  for(Clock* clk : clks){
    int id = clk->getId();
    matrice[id][0]=Bound(0);
    matrice[0][id]=Bound(0);
    for(int i = 1; i<length; i++){
      if(i!=id) {
        matrice[i][id] = matrice[i][0];
        matrice[id][i] = matrice[0][i];
      }
    }
  }
}

//The value of the given clocks became the interval [0,+inf].
void DBM::maximize(unordered_set<Clock*> clks){
  for(Clock* clk : clks){
    int id = clk->getId();
    matrice[id][0]=Bound();
    matrice[0][id]=Bound(0);
    for(int i = 1; i<length; i++){
      if(i!=id) {
        matrice[i][id] = matrice[i][0];
        matrice[id][i] = Bound();
      }
    }
  }
}

//Return false if the DBM represent an
bool DBM::empty() const{
  //Check if the clocks individual interval are consistent.
  Bound zero = Bound(0);
  for(int i = 0; i< length; i++){
    //Verification of the diagonal values.
    if(matrice[i][i] != zero){
      return true;
    }
    //Bounds of clocks values are positive. Don't need to test maximum bound.
    //(The minimal bound of a clock c_i is -1*matrice[0][i])
    if (zero < matrice[0][i]){
      return true;
    }
    //The maximum bound is greater than the minimal one.
    if (matrice[i][0] + matrice[0][i] < zero){
      return true;
    }
  }
  //Check if the relation between clocks is consistent.
  //(ci-cj ~ matrice_ij, where ~ in {<,<=})
  for(int i=1; i<length; i++){
    for(int j = i+1; j<length; j++){
      //The maximum bound is greater than the minimal one.
      if (matrice[i][j] + matrice[j][i] < zero){
        return true;
      }
      //Check if the clocks values are consistent with the relation.
      if((matrice[i][0]+matrice[0][j]+matrice[j][i] < zero)
      || (matrice[0][i]+matrice[j][0]+matrice[i][j] < zero)){
        return true;
      }
    }
  }
  return false;
}

//Reduction operator. Refine the dbm by reducing the interval
//which are lager than needed.
void DBM::normalize(){
  for(int k = 0; k<length; k++){
    for(int i=0; i<length;i++){
      for(int j=0; j<length;j++){
        matrice[i][j] = matrice[i][j].min(matrice[i][k]+matrice[k][j]);
      }
    }
  }
}

//The intersection of a DBM is the matrix where each element is the smallest
//from both the DBM in parameter.
DBM DBM::intersect(DBM const& dbm2) const{
  if(this->length == 1){
    return dbm2;
  }
  if(dbm2.length == 1)
    return *this;
  if(this->length != dbm2.length){
    cout << "Erreur d'intersection. Les deux DBM n'ont pas le même nombre d'horloge!";
    return DBM::fail();
  }
  DBM intersection;
  if(dbm2.length > this->length){
    intersection = DBM(dbm2.length-1).intersect(*this);
  }else{
    intersection = DBM(*this);
  }

  for(int i = 0; i < dbm2.length;i++){
    for(int j = 0; j < dbm2.length; j++){
      intersection.matrice[i][j] = intersection.matrice[i][j].min(dbm2.matrice[i][j]);
    }
  }
  return intersection;
}

DBM DBM::project(DBM const& destination) const{
  DBM projection = DBM(this->getClocks_number());
  for(int i = 1; i < projection.length; i++){
    projection.matrice[i][0] = destination.matrice[i][0];
    projection.matrice[0][i] = this->matrice[0][i];
    for(int j = i+1; j < projection.length; j++){
      projection.matrice[i][j] = this->matrice[i][j].min(destination.matrice[i][j]);
      projection.matrice[j][i] = this->matrice[j][i].min(destination.matrice[j][i]);
    }
  }
  return projection;
}

//Subset operators
bool DBM::operator<(DBM const& dbm2) const{
  if(dbm2.length==1) return true;
  if(dbm2.length != this->length){
    cout << "Erreur Subset. Les deux DBM n'ont pas le même nombre d'horloge!";
    return false;
  }
  for(int i = 0; i < length; i++)
    for(int j = 0; j < length; j++)
      if(dbm2.matrice[i][j] <= matrice[i][j])
        return false;
  return true;
}
bool DBM::operator<=(DBM const& dbm2) const{
  if(dbm2.length==1) return true;
  if(dbm2.length != this->length){
    cout << "Erreur Subset. Les deux DBM n'ont pas le même nombre d'horloge!";
    return false;
  }
  for(int i = 0; i < length; i++)
    for(int j = 0; j < length; j++)
      if(dbm2.matrice[i][j] < matrice[i][j])
        return false;
  return true;
}

Clock::Clock(string const& p_name, int id): id(id),name(p_name){}
Clock::~Clock() = default;

int Clock::getId() const{
  return id;
}

void Clock::print() const{
  cout << this->name << " : " << this->id;
}

State::State(string identifiant, DBM clock_constraints) :
   id(identifiant), clocks_constraints(clock_constraints){ }
State::State(string identifiant):id(identifiant),clocks_constraints(DBM()) { }
State::State():id(""),clocks_constraints(DBM()) { }
State::~State() = default;

DBM State::accept(DBM const& clocks_status){
  if(clocks_constraints.getClocks_number() == 0){
    return clocks_status;
  }
  DBM res = clocks_constraints.intersect(clocks_status);
  res.normalize();
  return res;
}


//----------------------------------------------------------------------------//
//-----------------------Utilitaire-------------------------------------------//
//----------------------------------------------------------------------------//
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
           vector<int> freez, DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset) : origine(ori), destination(dest),
           allocations(allocs), frees(freez), clocks_to_reset(clock_to_reset){
   clocks_constraints = clocks_interv.intersect(ori->clocks_constraints);
   /**If the clock is reset there is no need to verify the destination's
    * constraints before we reset the clock.
    */
   DBM dest_constraints(dest->clocks_constraints);
   if(dest_constraints.getClocks_number()>0){
     dest_constraints.maximize(clocks_to_reset);
   }
   clocks_constraints = clocks_constraints.intersect(dest_constraints);
   clocks_constraints.normalize();
}

Transition::Transition(State* const& ori, State* const& dest)
                               :origine(ori),destination(dest){
  clocks_constraints = ori->clocks_constraints.intersect(dest->clocks_constraints);
}
Transition::~Transition() = default;

pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
      DBM initial_clocks_status, DBM current_clocks_status,
      DBM final_clocks_status,vector<pair<bool,unordered_set<string>>> memory){
  return pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>>({},{});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
      DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(), {});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_event(
    DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
    string event){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_constant(
    DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
    string constant){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
}

//TODO
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
           vector<int> freez, DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset){}
Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest)
    : Transition(ori,dest){}
Epsilon_Transition::~Epsilon_Transition() = default;
//TODO
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


Event_Transition::Event_Transition(State* const& ori, State* const& dest,
           vector<int> allocs, vector<int> freez, DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset, int p_variable)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), variable(p_variable){}
Event_Transition::Event_Transition(State* const& ori, State* const& dest, int p_variable)
    : Transition(ori,dest), variable(p_variable){}
Event_Transition::~Event_Transition() = default;
//TODO
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
           vector<int> freez, DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset, string p_constant)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), constant(p_constant){}
Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, string p_constant)
    : Transition(ori,dest), constant(p_constant){}
Constant_Transition::~Constant_Transition() = default;
//TODO
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
         vector<Clock*> p_clocks,
         map<State*, vector<Transition*>> p_transitions,
         unordered_set<string> p_alphabet,
         State* p_start,
         vector<State*> p_endStates) :
         ressources(p_ressources), states(p_states), clocks(p_clocks),
         transitions(p_transitions), alphabet(p_constants),
         start(p_start), endStates(p_endStates) {}

Automate::~Automate() {
   for(auto & pair : transitions){
     for(Transition * trans : pair.second){
       delete trans;
     }
   }
   for(Clock* clk : clocks){
     delete clk;
   }
 }

Clock* Automate::find_or_create_clock(string name){
  for(Clock* existing_clock : this->clocks){
    if(existing_clock->name == name)
      return existing_clock;
  }
  Clock* new_clock = new Clock(name,this->clocks.size()+1);
  this->clocks.push_back(new_clock);

  //Increase all non-empty DBM-size
  for(State & state : states){
    if(state.clocks_constraints.getClocks_number()!=0){
      state.clocks_constraints.addClock();
    }
    for(Transition & trans : transitions[&state]){
      if(trans.clocks_constraints.getClocks_number()!=0){
        trans.clocks_constraints.addClock();
      }
    }
  }

  return new_clock;
}
State* Automate::getState(string state_name){
  for(State& state : this->states){
    if(state.id == state_name){
      return &state;
    }
  }
  return nullptr;
}
