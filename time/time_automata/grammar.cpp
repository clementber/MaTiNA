#include "grammar.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
using namespace automate;
using namespace std;

Bound::Bound(): value(numeric_limits<double>::max()),inclusion(-1){}
Bound::Bound(double value): value(value), inclusion(0){}
Bound::Bound(double value, bool included): value(value), inclusion(included){}

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
DBM::DBM(Automate autom):length(autom.clocks.size()+1){
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

void DBM::reset(vector<Clock*> clks){
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
void DBM::maximize(vector<Clock*> clks){
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
    if(matrice[i][i] != zero)
      return true;
    //Bounds of clocks values are positive. Don't need to test maximum bound.
    //(The minimal bound of a clock c_i is -1*matrice[0][i])
    if (zero < matrice[0][i])
      return true;
    //The maximum bound is greater than the minimal one.
    if (matrice[i][0] + matrice[0][i] < zero)
      return true;
  }
  //Check if the relation between clocks is consistent.
  //(ci-cj ~ matrice_ij, where ~ in {<,<=})
  for(int i=1; i<length; i++){
    for(int j = i+1; j<length; j++){
      //The maximum bound is greater than the minimal one.
      if (matrice[i][j] + matrice[j][i] < zero)
        return true;
      //Check if the clocks values are consistent with the relation.
      if((matrice[i][0]+matrice[0][j]+matrice[j][i] < zero)
      || (matrice[0][i]+matrice[j][0]+matrice[i][j] < zero))
        return true;
    }
  }
  return false;
}

//Reduction operator. Refine the dbm by reducing the interval
//which are lager than needed.
//PRECONDITION : this->isValid()==true.
void DBM::normalize(){
  for(int i=1; i<length;i++){
    for(int j=1; j<length;j++){
      if(i==j) continue;
      //Reduce the interval of each clocks depending on the relation on it
      matrice[0][i]=matrice[0][i].min(matrice[0][j]+matrice[j][i]);
      matrice[i][0]=matrice[i][0].min(matrice[j][0]+matrice[i][j]);
      //Reduce a relation if too far from the clocks values.
      matrice[i][j] = matrice[i][j].min(matrice[i][0] + matrice[0][j]);
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
  DBM res = clocks_constraints.intersect(clocks_status);
  res.normalize();
  return res;
}

Transition::Transition(State* const& ori, State* const& dest, vector<string> events,
           DBM const& clocks_interv,
           vector<Clock*> const& clock_to_reset) : origine(ori), destination(dest),
           triggers(events), clocks_to_reset(clock_to_reset){
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
                             :origine(ori), destination(dest){
  clocks_constraints = ori->clocks_constraints.intersect(dest->clocks_constraints);
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
  DBM accepted_values = clocks_status.intersect(clocks_constraints);
  if(accepted_values.empty()) {
    return DBM::fail();
  }
  accepted_values.reset(clocks_to_reset);
  //Last check on the token in it's final state.
  DBM output_values = accepted_values.intersect(destination->clocks_constraints);
  output_values.normalize();
  return output_values;
}

vector<DBM> Transition::accept(DBM initial_clocks_status,
                               DBM current_clocks_status,
                               DBM final_clocks_status)const{
  if(! this->triggers.empty()){
    return {};
  }
  //Check if a constraint shall be respected
  if(clocks_constraints.getClocks_number() != 0){
    //First : correct the initial and final clocks values by removing the
    //unreachable from the clocks_constraints.
    for(int i =1; i< clocks_constraints.length; i++){
      if(clocks_constraints.matrice[i][0] < initial_clocks_status.matrice[i][0]){
        final_clocks_status.matrice[i][0] = Bound(final_clocks_status.matrice[i][0].value - initial_clocks_status.matrice[i][0].value + clocks_constraints.matrice[i][0].value , clocks_constraints.matrice[i][0].inclusion);
        initial_clocks_status.matrice[i][0] = clocks_constraints.matrice[i][0];
      }
      if(clocks_constraints.matrice[0][i] < final_clocks_status.matrice[0][i]){
        if(initial_clocks_status.matrice[i][0] != Bound(0)){
          initial_clocks_status.matrice[0][i] = Bound(initial_clocks_status.matrice[0][i].value - final_clocks_status.matrice[0][i].value + clocks_constraints.matrice[0][i].value , clocks_constraints.matrice[0][i].inclusion);
        }
        final_clocks_status.matrice[0][i] = clocks_constraints.matrice[0][i];
      }
      current_clocks_status.matrice[i][0] = current_clocks_status.matrice[i][0].min(clocks_constraints.matrice[i][0]);
    }
    if(current_clocks_status.empty()||final_clocks_status.empty()||initial_clocks_status.empty()){
      return {};
    }
    current_clocks_status.normalize();
    final_clocks_status.normalize();
    //Generate the projection DBM (all reachable value) from the current clocks_status to the final_clocks_status.
    DBM projection = DBM(current_clocks_status.getClocks_number());
    for(int i = 1; i < projection.length; i++){
      projection.matrice[i][0] = final_clocks_status.matrice[i][0];
      projection.matrice[0][i] = current_clocks_status.matrice[0][i];
      for(int j = i+1; j < projection.length; j++){
        projection.matrice[i][j] = current_clocks_status.matrice[i][j].min(final_clocks_status.matrice[i][j]);
        projection.matrice[j][i] = current_clocks_status.matrice[j][i].min(final_clocks_status.matrice[j][i]);
      }
    }
    //The accepted values are the intersection of the projection DBM
    // and the constraint DBM.
    current_clocks_status = clocks_constraints.intersect(projection);
    if(current_clocks_status.empty()){
      return {};
    }
    current_clocks_status.normalize();
    //The final_clocks_status is restricted by the relation of the
    //current_clocks_values.
    for(int i=1; i<final_clocks_status.length; i++){
      for(int j=i+1; j<final_clocks_status.length;j++){
        final_clocks_status.matrice[i][j] = final_clocks_status.matrice[i][j].min(current_clocks_status.matrice[i][j]);
      }
    }
    //The unreachable values in the final_clocks_status should be removed from the
    // initial_clocks_status
    Bound minVals[final_clocks_status.length], maxVals[final_clocks_status.length];
    for(int i=0; i<final_clocks_status.length;i++){
      minVals[i] = final_clocks_status.matrice[0][i];
      maxVals[i] = final_clocks_status.matrice[i][0];
    }
    final_clocks_status.normalize();
    for(int i=0; i<final_clocks_status.length;i++){
      if(initial_clocks_status.matrice[i][0] != Bound(0)){
        initial_clocks_status.matrice[0][i].value = initial_clocks_status.matrice[0][i].value - (minVals[i].value - final_clocks_status.matrice[0][i].value);
        initial_clocks_status.matrice[0][i].inclusion = final_clocks_status.matrice[0][i].inclusion;
        initial_clocks_status.matrice[i][0] = initial_clocks_status.matrice[i][0].value - (minVals[i].value - final_clocks_status.matrice[i][0].value);
        initial_clocks_status.matrice[i][0].inclusion = final_clocks_status.matrice[i][0].inclusion;
      }
    }
  }

  //Reset management
  if(!clocks_to_reset.empty()){
    //Calculate the final values for the reseted clocks.
    Bound max_available_time = Bound(final_clocks_status.matrice[1][0].value - initial_clocks_status.matrice[1][0].value, final_clocks_status.matrice[1][0].inclusion - initial_clocks_status.matrice[1][0].inclusion);
    Bound max_remaining_time = Bound(final_clocks_status.matrice[1][0].value - current_clocks_status.matrice[0][1].value, final_clocks_status.matrice[1][0].inclusion - current_clocks_status.matrice[0][1].inclusion);
    Bound min_remaining_time = Bound(final_clocks_status.matrice[0][1].value - current_clocks_status.matrice[1][0].value, final_clocks_status.matrice[0][1].inclusion - current_clocks_status.matrice[1][0].inclusion);
    for(int i=2; i< final_clocks_status.length; i++){
      max_available_time = max_available_time.min(Bound(final_clocks_status.matrice[i][0].value - initial_clocks_status.matrice[i][0].value, final_clocks_status.matrice[i][0].inclusion - initial_clocks_status.matrice[i][0].inclusion));
      max_remaining_time = max_remaining_time.min(final_clocks_status.matrice[i][0] + current_clocks_status.matrice[0][i]);
      Bound candidate_min_remaining_time = Bound((-1*final_clocks_status.matrice[0][1].value) - current_clocks_status.matrice[1][0].value, (-1*final_clocks_status.matrice[0][1].inclusion) - current_clocks_status.matrice[1][0].inclusion);
      min_remaining_time = min_remaining_time<candidate_min_remaining_time?candidate_min_remaining_time:min_remaining_time;
    }
    max_remaining_time = max_remaining_time.min(max_available_time);
    if(min_remaining_time < Bound(0)){
      min_remaining_time = Bound(0);
    }

    //Reset the clocks
    current_clocks_status.reset(clocks_to_reset);

    //Adjust the final and initial clocks bounds.
    for(Clock* clk: clocks_to_reset){
      initial_clocks_status.matrice[clk->getId()][0] = Bound(0);
      initial_clocks_status.matrice[0][clk->getId()] = Bound(0);
      final_clocks_status.matrice[clk->getId()][0] = max_remaining_time;
      final_clocks_status.matrice[0][clk->getId()] = min_remaining_time;
    }
    //Calculate the relation of the final zone.
    for(int i=1; i<current_clocks_status.length; i++){
      for(int j=1; j<current_clocks_status.length; j++){
        if(i!=j){
          final_clocks_status.matrice[i][j]=current_clocks_status.matrice[i][j];
        }
      }
    }

    if(final_clocks_status.empty()) return {};

    //And report the modification on the initial zone.
    Bound minVals[final_clocks_status.length], maxVals[final_clocks_status.length];
    for(int i=0; i<final_clocks_status.length;i++){
      minVals[i] = final_clocks_status.matrice[0][i];
      maxVals[i] = final_clocks_status.matrice[i][0];
    }
    final_clocks_status.normalize();
    for(int i=0; i<final_clocks_status.length;i++){
      if(initial_clocks_status.matrice[i][0] != Bound(0)){
        initial_clocks_status.matrice[0][i].value = initial_clocks_status.matrice[0][i].value - (minVals[i].value - final_clocks_status.matrice[0][i].value);
        initial_clocks_status.matrice[0][i].inclusion = final_clocks_status.matrice[0][i].inclusion;
        initial_clocks_status.matrice[i][0] = initial_clocks_status.matrice[i][0].value - (minVals[i].value - final_clocks_status.matrice[i][0].value);
        initial_clocks_status.matrice[i][0].inclusion = final_clocks_status.matrice[i][0].inclusion;
      }
    }
    //Finally apply the raltion of the final zone on the current zone.
    for(int i=1; i<current_clocks_status.length; i++){
      for(int j=1; j<current_clocks_status.length; j++){
        if(i!=j){
          current_clocks_status.matrice[i][j]=final_clocks_status.matrice[i][j];
        }
      }
    }
    if(current_clocks_status.empty()) return {};
    current_clocks_status.normalize();
  }

  //Check if the outgoing clocks_values are accepted in the final state.
  current_clocks_status = destination->accept(current_clocks_status);
  if(current_clocks_status.empty()) return {};

  //Recalculate the final zone.
  Bound minVals[final_clocks_status.length], maxVals[final_clocks_status.length];
  for(int i=0; i<final_clocks_status.length;i++){
    minVals[i] = final_clocks_status.matrice[0][i];
    maxVals[i] = final_clocks_status.matrice[i][0];
  }
  if(final_clocks_status.empty()) return {};
  final_clocks_status.normalize();
  for(int i=0; i<final_clocks_status.length;i++){
    if(initial_clocks_status.matrice[i][0] != Bound(0)){
      initial_clocks_status.matrice[0][i].value = initial_clocks_status.matrice[0][i].value - (minVals[i].value - final_clocks_status.matrice[0][i].value);
      initial_clocks_status.matrice[0][i].inclusion = final_clocks_status.matrice[0][i].inclusion;
      initial_clocks_status.matrice[i][0] = initial_clocks_status.matrice[i][0].value - (minVals[i].value - final_clocks_status.matrice[i][0].value);
      initial_clocks_status.matrice[i][0].inclusion = final_clocks_status.matrice[i][0].inclusion;
    }
  }

  return {initial_clocks_status,current_clocks_status,final_clocks_status};
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
