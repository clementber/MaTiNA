#include "grammar.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
using namespace automate;
using namespace std;

Bound::Bound(): value(numeric_limits<double>.max()),inclusion(LESS){}
Bound::Bound(double value): value(value), inclusion(LESSEQ){}
Bound::Bound(double value, bool included): value(value), inclusion(included){}

Bound Bound::operator+(Bound const& bound2) const{
  if(this->value == numeric_limits<double>.max() || bound2.value == numeric_limits<double>.max()){
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

DBM::DBM():matrice({{Bound(0,LESSEQ)}}){}
DBM::DBM(int clocks_number){
  for(int i = 0; i<= clocks_number; i++){
    matrice.push_back(std::vector<Bound>(clocks_number+1));
    for(int j=0; j<= clocks_number; j++){
      matrice[i][j]=(i==j?Bound(0,LESSEQ):Bound());
    }
  }
}
DBM::DBM(Automate automate):DBM(automate.clocks.size()){}

static DBM DBM::fail(){
  return {{Bound(-1,LESS)}};
}

//Time modification operators
void DBM::increment(double time_delay){
  for(int i=1; i<matrice.size();i++){
    matrice[0][i].value -= time_delay;
    matrice[i][0].value += time_delay;
  }
}

void DBM::reset(Clock* clk){
  //TODO
}

//Reductions and validation operators.
bool DBM::isValid() const{
  for(int i = 0; i< matrice.length; i++){
    if(matrice[i][i].value != 0 || matrice[i][i].inclusion != LESSEQ)
      return false;
    if ((-1*matrice[0][i].value) < 0 || matrice[i][0].value < 0
       || (matrice[i][0].value==0 && matrice[i][0].inclusion== LESS)
       || (matrice[0][i].value==0 && matrice[0][i].inclusion== LESS))
      return false;
    if()
  }

}
bool DBM::reduce();

DBM DBM::intersect(DBM const& dbm2) const;
DBM DBM::operator+(DBM const& dbm2) const;
//Subset operators
bool DBM::operator<(DBM const& dbm2) const;
bool DBM::operator<=(DBM const& dbm2) const;

State::State(string identifiant,
             DBM clock_constraints) :
               id(identifiant), clocks_constraints(clock_constraints){ }
//TODO que faire si aucun DBM n'est fourni.
State::State(string identifiant):id(identifiant),clocks_constraints() { }
State::State() = default;
State::~State() = default;

DBM State::accept(DBM const& clocks_status) const{
  //TODO
  return vector<Clock<>>;
}

Clock::Clock(string const& p_name, int id): name(p_name), id(id){}
Clock::~Clock() = default;

int Clock::getId(){
  return id;
}

void Clock::print() const{
  //TODO
}

void merge_constraints(DBM & dbm1, DBM const& dbm2){
  //TODO penser au cas où this est vide.
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
    return DBM.fail();
  }
  //TODO
}

DBM Transition::accept(DBM const& clocks_status){
  if(! this->triggers.empty()){
    return DBM.fail();
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
