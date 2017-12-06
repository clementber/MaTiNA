#include "grammar.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
using namespace automate;
using namespace std;

Bound::Bound(): value(numeric_limits<double>::max()),inclusion(LESS){}
Bound::Bound(double value): value(value), inclusion(LESSEQ){}
Bound::Bound(double value, bool included): value(value), inclusion(included){}

Bound Bound::operator+(Bound const& bound2) const{
  if(this->value == numeric_limits<double>::max() || bound2.value == numeric_limits<double>::max()){
    return Bound();
  }
  return Bound(this->value + bound2.value, ((this->inclusion == LESS ||bound2.inclusion == LESS)?LESS:LESSEQ));
}

Bound Bound::intersect(Bound const& bound2) const{
  if(*this < bound2.value)
    return *this;
  else
    return bound2;
}

bool Bound::operator<(Bound const& bound2) const{
  return (this->value < bound2.value)
      || ((this->value == bound2.value)&& (this->inclusion < bound2.inclusion));
}

bool Bound::operator<=(Bound const& bound2) const{
  return (this->value < bound2.value)
      || ((this->value == bound2.value)&& (this->inclusion <= bound2.inclusion));
}

DBM::DBM():DBM(0){}
DBM::DBM(int clocks_number):length(clocks_number+1){
  for(int i = 0; i<= clocks_number; i++){
    matrice.push_back(std::vector<Bound>(clocks_number+1));
    for(int j=0; j<= clocks_number; j++){
      matrice[i][j]=(i==j?Bound(0,LESSEQ):Bound());
    }
  }
}
DBM::DBM(Automate automate):DBM(automate.clocks.size()){}

static DBM DBM::fail(){
  DBM invalid_dbm = DBM();
  invalid_dbm.value[0][0]=-1;
  return invalid_dbm;
}

int DBM::getClocks_number(){return length-1;}

//Time modification operators
void DBM::increment(double time_delay){
  for(int i=1; i<length;i++){
    matrice[0][i].value -= time_delay;
    matrice[i][0].value += time_delay;
  }
}

void DBM::reset(vector<Clock*> clks){
  for(Clock* clk : clks){
    int id = clk->getId()
    matrice[id][0]=Bound(0);
    matrice[0][id]=Bound(0);
    //TODO : Optimizable?
    for(int i = 1; i<length; i++){
      if(i==id) {
        matrice[i][id] = Bound();
        matrice[id][i] = Bound();
      }
    }
  }
  reduce();
}

//Validation operator.
bool DBM::isValid() const{
  //Check if the clocks individual interval are consistent.
  for(int i = 0; i< length; i++){
    //Verification of the diagonal values.
    if(matrice[i][i].value != 0 || matrice[i][i].inclusion != LESSEQ)
      return false;
    //Both bound of clocks values are positive
    if ((-1*matrice[0][i].value) < 0 || matrice[i][0].value < 0
       || (matrice[i][0].value==0 && matrice[i][0].inclusion== LESS)
       || (matrice[0][i].value==0 && matrice[0][i].inclusion== LESS))
      return false;
    //The maximum bound is greater than the minimal one.
    if ((-1*matrice[0][i].value) > matrice[i][0].value
    || ((-1*matrice[0][i].value) == matrice[i][0].value
        && (matrice[0][i].inclusion == LESS
        ||  matrice[i][0].inclusion == LESS)))
      return false;
  }
  //Check if the relation between clocks is consistent.
  //(ci-cj ~ value, where ~ in {<,<=})
  for(int i=1; i<length; i++){
    for(int j = i+1; j<length; j++){
      //The maximum bound is greater than the minimal one.
      if ((-1*matrice[j][i].value) > matrice[i][j].value
      || ((-1*matrice[j][i].value) == matrice[i][j].value
          && (matrice[j][i].inclusion == LESS
          ||  matrice[i][j].inclusion == LESS)))
        return false;
      //Check if the clocks values are consistent with the relation.
      if((matrice[i][0].value+matrice[0][j].value) < (-1*matrice[j][i].value)
      || ((matrice[i][0].value+matrice[0][j].value) == (-1*matrice[j][i].value)
         && (matrice[j][i].inclusion==LESS
         ||matrice[i][0].inclusion==LESS
         ||matrice[0][j].inclusion==LESS))
      ||((-1*matrice[0][i].value)-matrice[j][0].value) > (matrice[i][j].value)
      || (((-1*matrice[0][i].value)-matrice[j][0].value) == (matrice[i][j].value)
         && (matrice[i][j].inclusion==LESS
         ||matrice[0][i].inclusion==LESS
         ||matrice[j][0].inclusion==LESS)))
      return false;
    }
  }
  return true;
}

//Reduction operator. Refine the dbm by reducing the interval
//which are lager than needed.
//PRECONDITION : this->isValid()==true.
bool DBM::reduce(){
  for(int i=1; i<length;i++){
    for(int j=1; i<length;j++){
      if(i==j) continue;
      //Reduce the interval of each clocks depending on the relation on it
      if(matrice[0][i].value - matrice[0][j].value > matrice[j][i].value)
        ||((matrice[0][i].value - matrice[0][j].value == matrice[j][i].value)
          &&(matrice[0][i].inclusion == LESS
          || matrice[0][j].inclusion == LESS
          || matrice[j][i].inclusion == LESS))){
        matrice[0][i]=Bound(matrice[0][j].value+matrice[j][i].value, matrice[j][i].inclusion);
      }
      if((matrice[i][0].value - matrice[j][0].value > matrice[i][j].value)
        ||((matrice[i][0].value - matrice[j][0].value == matrice[i][j].value)
          &&(matrice[i][0].inclusion == LESS
          || matrice[j][0].inclusion == LESS
          || matrice[i][j].inclusion == LESS))){
        matrice[i][0]=Bound(matrice[j][0].value+matrice[i][j].value, matrice[i][j].inclusion);
      //Reduce a relation if too far from the clocks values.
      }else{
        matrice[i][j].inclusion = ((matrice[i][0].value + matrice[0][j].value)<=matrice[i][j].value
                                  ? min(matrice[i][j].inclusion,min(matrice[i][0].inclusion, matrice[0][j].inclusion))
                                  : matrice[i][j].inclusion);
        matrice[i][j].value = min(matrice[i][j].value,matrice[i][0].value + matrice[0][j].value);
      }
    }
  }
}

//The intersection of a DBM is the matrix where each element is the smallest
//from both the DBM in parameter.
DBM DBM::intersect(DBM const& dbm2) const{
  if(dbm2.length != this->length){
    return DBM::fail();
  }
  DBM intersection = DBM(this->length);
  for(int i = 0; i < this->length;i++)
    for(int j = 0; j < this->length; j++){
      intersection.matrice[i][j] = (matrice[i][j]<dbm2.matrice[i][j]?matrice[i][j]:dbm2.matrice[i][j]);
    }
  return intersection;
}

DBM DBM::operator+(DBM const& dbm2) const;
//Subset operators
bool DBM::operator<(DBM const& dbm2) const{
  if(dbm2.length != this->length){
    return false;
  }
  for(int i = 0; i < length; i++)
    for(int j = 0; j < length; j++)
      if(dbm2.matrice[i][j] <= matrice[i][j])
        return false;
  return true;
}
bool DBM::operator<=(DBM const& dbm2) const{
  if(dbm2.length != this->length){
    return false;
  }
  for(int i = 0; i < length; i++)
    for(int j = 0; j < length; j++)
      if(dbm2.matrice[i][j] < matrice[i][j])
        return false;
  return true;
}

Clock::Clock(string const& p_name, int id): name(p_name), id(id){}
Clock::~Clock() = default;

int Clock::getId(){
  return id;
}

void Clock::print() const{
  //TODO
}

State::State(string identifiant,
             DBM clock_constraints) :
               id(identifiant), clocks_constraints(clock_constraints){ }
State::State(string identifiant):id(identifiant),clocks_constraints(DBM()) { }
State::State():id(""),clocks_constraints(DBM()) { }
State::~State() = default;

DBM State::accept(DBM const& clocks_status) const{
  if(clocks_constraints.getClocks_number() == 0){
    return clocks_status;
  }
  return clocks_constraints.intersection(clocks_status).reduce();
}

void merge_constraints(DBM & dbm1, DBM const& dbm2){
  //TODO penser au cas où dbm1 et/ou dbm2 est vide.
}

Transition::Transition(State* const& ori, State* const& dest, vector<string> events,
           DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset) : origine(ori), destination(dest),
           triggers(events), clocks_to_reset(clock_to_reset){
  clocks_constraints = DBM(clocks_interv);
  merge_constraints(clocks_constraints, ori->clocks_constraints);
  /**If the clock is reset there is no need to verify the destination's
   * constraints before we reset the clock.
   */
  DBM dest_constraints(dest->clocks_constraints);
  for(auto clk_reseted : clock_to_reset){
    //TODO redo the erase for a matrice
    dest_constraints.erase(clk_reseted);
  }
  merge_constraints(clocks_constraints, dest_constraints);
}
Transition::Transition(State* const& ori, State* const& dest)
                             :origine(ori), destination(dest){
  merge_constraints(clocks_constraints, ori->clocks_constraints);
  merge_constraints(clocks_constraints, dest->clocks_constraints);
}
Transition::~Transition() = default;
bool Transition::epsilon() const{
  return triggers.empty();
}

DBM Transition::accept(string const& event,
                                 DBM const& clocks_status) const{
  bool triggered = false;
  for(auto accepted_event : this->triggers){
    if(accepted_event == event){
      triggered = true;
      break;
    }
  }
  if(!triggered){
    return DBM::fail();
  }
  //TODO
}

DBM Transition::accept(DBM const& clocks_status){
  if(! this->triggers.empty()){
    return DBM::fail();
  }
  //TODO
  //Last check on the token in it's final state.
  return this->destination->accept(res);
}

Automate::Automate() = default;

Automate::Automate(list<State> p_states,vector<Clock> p_clocks,
         map<State*, vector<Transition>> p_transitions,
         set<string> p_alphabet,State* p_start,vector<State*> p_endStates) :

         states(p_states), clocks(p_clocks), transitions(p_transitions), alphabet(p_alphabet),
         start(p_start), endStates(p_endStates) {}
Automate::~Automate() = default;
Clock* Automate::find_or_create_clock(string name){
  for(Clock & existing_clock : this->clocks){
    if(existing_clock.name == name)
      return &existing_clock;
  }
  Clock new_clock(name,this->clocks.size()+1);
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
