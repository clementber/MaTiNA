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
  if(this->getClocks_number() == 0){
    return dbm2;
  }
  if(dbm2.getClocks_number() == 0)
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
           unordered_set<Clock*> const& clock_to_reset) : origine(ori), destination(dest),
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

pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>> Transition::accept_epsilon(
      DBM initial_clocks_status, DBM current_clocks_status,
      DBM final_clocks_status,vector<pair<bool,unordered_set<string>>> memory){
  return pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>>({},{});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> Transition::accept_epsilon(
      DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(), {});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> Transition::accept_event(
    DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
    string event){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
}
pair<DBM,vector<pair<bool,unordered_set<string>>>> Transition::accept_constant(
    DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
    string constant){
  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
}


string Transition::to_string(){
  return "";
}

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, DBM const& clocks_interv,
           unordered_set<Clock*> const& clock_to_reset)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset){}

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest)
    : Transition(ori,dest){}

Transition* Epsilon_Transition::clone(){
  return new Epsilon_Transition(*this);
}

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
  return res.str();
}

bool Epsilon_Transition::epsilon(){
  return true;
}

//The Epsilon_Transition are the only transition
//possibly triggered after a time elapse.
pair<DBM,vector<pair<bool,unordered_set<string>>>> Epsilon_Transition::accept_epsilon(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory){
  //Time management
  DBM accepted_values = clocks_status.intersect(clocks_constraints);
  if(accepted_values.empty()) {
    return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
  }
  accepted_values.reset(clocks_to_reset);
  //Last check on the token in it's final state.
  DBM output_values = accepted_values.intersect(destination->clocks_constraints);
  output_values.normalize();

  //Memory management
  if( ! memory.empty()){
    alloc(this->allocations, memory);
    desalloc(this->frees, memory);
  }

  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(output_values,memory);
}

pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>> Epsilon_Transition::accept_epsilon(
        DBM initial_clocks_status, DBM current_clocks_status,
        DBM final_clocks_status,vector<pair<bool,unordered_set<string>>> memory){
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
    DBM projection = current_clocks_status.project(final_clocks_status);
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
        final_clocks_status.matrice[i][j] = current_clocks_status.matrice[i][j];
        final_clocks_status.matrice[j][i] = current_clocks_status.matrice[j][i];
      }
    }

    if(final_clocks_status.empty()) {
      return {};
    }

    //The unreachable values in the final_clocks_status should be removed from the
    // initial_clocks_status
    Bound minVals[final_clocks_status.length], maxVals[final_clocks_status.length];
    for(int i=0; i<final_clocks_status.length;i++){
      minVals[i] = final_clocks_status.matrice[0][i];
      maxVals[i] = final_clocks_status.matrice[i][0];
    }
    //Reduce the individual clocks valuation after capturate them.
    final_clocks_status.normalize();
    //Recalculate the allowed initial valuation by removing the one that
    //cannot reach a final valuation.
    for(int i=0; i<final_clocks_status.length;i++){
      if(initial_clocks_status.matrice[i][0] != Bound(0)){
        initial_clocks_status.matrice[0][i].value = initial_clocks_status.matrice[0][i].value - (minVals[i].value - final_clocks_status.matrice[0][i].value);
        initial_clocks_status.matrice[0][i].inclusion = final_clocks_status.matrice[0][i].inclusion;
        initial_clocks_status.matrice[i][0] = initial_clocks_status.matrice[i][0].value - (maxVals[i].value - final_clocks_status.matrice[i][0].value);
        initial_clocks_status.matrice[i][0].inclusion = final_clocks_status.matrice[i][0].inclusion;
      }
    }
  }

  //Reset management
  if(!clocks_to_reset.empty()){
    //Calculate the final values for the reseted clocks.
    Bound max_available_time = Bound(final_clocks_status.matrice[1][0].value - initial_clocks_status.matrice[1][0].value, final_clocks_status.matrice[1][0].inclusion - initial_clocks_status.matrice[1][0].inclusion);
    Bound max_remaining_time = final_clocks_status.matrice[1][0] + current_clocks_status.matrice[0][1];
    Bound min_remaining_time = Bound((-1*final_clocks_status.matrice[0][1].value) - current_clocks_status.matrice[1][0].value, (-1*final_clocks_status.matrice[0][1].inclusion) - current_clocks_status.matrice[1][0].inclusion);
    for(int i=2; i< final_clocks_status.length; i++){
      max_available_time = max_available_time.min(Bound(final_clocks_status.matrice[i][0].value - initial_clocks_status.matrice[i][0].value, final_clocks_status.matrice[i][0].inclusion - initial_clocks_status.matrice[i][0].inclusion));
      max_remaining_time = max_remaining_time.min(final_clocks_status.matrice[i][0] + current_clocks_status.matrice[0][i]);
      Bound candidate_min_remaining_time = Bound((-1*final_clocks_status.matrice[0][1].value) - current_clocks_status.matrice[1][0].value, (-1*final_clocks_status.matrice[0][1].inclusion) - current_clocks_status.matrice[1][0].inclusion);
      min_remaining_time = min_remaining_time<candidate_min_remaining_time?candidate_min_remaining_time:min_remaining_time;
    }

    max_remaining_time = max_remaining_time.min(max_available_time);
    if(min_remaining_time < Bound(0)){
      min_remaining_time = Bound(0);
    }else{
      min_remaining_time.value *= -1;
      min_remaining_time.inclusion *= -1;
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
  }

  //Check if the outgoing clocks_values are accepted in the final state.
  current_clocks_status = destination->accept(current_clocks_status);
  if(current_clocks_status.empty()) {
    return {};
  }
  //Finally apply the relation of the final zone on the current zone.
  for(int i=1; i<current_clocks_status.length; i++){
    for(int j=1; j<current_clocks_status.length; j++){
      final_clocks_status.matrice[i][j]=current_clocks_status.matrice[i][j];
    }
  }
  //Recalculate the final zone.
  Bound minVals[final_clocks_status.length], maxVals[final_clocks_status.length];
  for(int i=0; i<final_clocks_status.length;i++){
    minVals[i] = final_clocks_status.matrice[0][i];
    maxVals[i] = final_clocks_status.matrice[i][0];
  }
  if(final_clocks_status.empty()) {
    return {};
  }
  final_clocks_status.normalize();
  for(int i=0; i<final_clocks_status.length;i++){
    if(initial_clocks_status.matrice[i][0] != Bound(0)){
      initial_clocks_status.matrice[0][i].value = initial_clocks_status.matrice[0][i].value - (minVals[i].value - final_clocks_status.matrice[0][i].value);
      initial_clocks_status.matrice[0][i].inclusion = final_clocks_status.matrice[0][i].inclusion;
      initial_clocks_status.matrice[i][0].value = initial_clocks_status.matrice[i][0].value - (maxVals[i].value - final_clocks_status.matrice[i][0].value);
      initial_clocks_status.matrice[i][0].inclusion = final_clocks_status.matrice[i][0].inclusion;
    }
  }

  //Memory management
  if( ! memory.empty()){
    alloc(this->allocations, memory);
    desalloc(this->frees, memory);
  }

  return pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>>({initial_clocks_status,current_clocks_status,final_clocks_status},memory);
}

Event_Transition::Event_Transition(State* const& ori, State* const& dest,
           vector<int> allocs, vector<int> freez, DBM const& clocks_interv,
           unordered_set<Clock*> const& clock_to_reset, int p_variable)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), variable(p_variable){}

Event_Transition::Event_Transition(State* const& ori, State* const& dest, int p_variable)
    : Transition(ori,dest), variable(p_variable){}

Transition* Event_Transition::clone(){
  return new Event_Transition(*this);
}

Event_Transition::~Event_Transition() = default;

string Event_Transition::to_string(){
  stringstream res;
  if(!allocations.empty()){
    for(uint i = 0 ; i < allocations.size(); i++){
      res << "&nu;{" << allocations[i] << "},";
    }
  }
  res <<"<B>"<< variable << "</B>";
  if(!frees.empty()){
    for(uint i=0; i < frees.size(); i++){
      res << ",<O>&nu;</O>{" << frees[i] << "}";
    }
  }
  return res.str() ;
}

bool Event_Transition::epsilon(){
  return false;
}

pair<DBM,vector<pair<bool,unordered_set<string>>>> Event_Transition::accept_event(
    DBM clocks_status, vector<pair<bool,unordered_set<string>>> memory,
    string p_event){
  //Memory management
  alloc(this->allocations, memory);
  if(!use(this->variable, memory, p_event)){
    return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
  }
  desalloc(this->frees, memory);

  //Time management
  DBM accepted_values = clocks_status.intersect(clocks_constraints);
  if(accepted_values.empty()) {
    return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
  }
  accepted_values.reset(clocks_to_reset);
  //Last check on the clocks_values in the destination location.
  DBM output_values = accepted_values.intersect(destination->clocks_constraints);
  output_values.normalize();

  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(output_values, memory);
}

Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, DBM const& clocks_interv,
           unordered_set<Clock*> const& clock_to_reset, string p_constant)
    : Transition(ori,dest,allocs,freez,clocks_interv, clock_to_reset), constant(p_constant){}

Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, string p_constant)
    : Transition(ori,dest), constant(p_constant){}

Transition* Constant_Transition::clone(){
  return new Constant_Transition(*this);
}

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
  return res.str();
}

bool Constant_Transition::epsilon(){
  return false;
}

pair<DBM,vector<pair<bool,unordered_set<string>>>> Constant_Transition::accept_constant(
      DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
      string constant){
  if(constant != this->constant){
    return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
  }
  //Memory management
  alloc(this->allocations, memory);
  desalloc(this->frees, memory);

  //Time management
  DBM accepted_values = clocks_status.intersect(clocks_constraints);
  if(accepted_values.empty()) {
    return pair<DBM,vector<pair<bool,unordered_set<string>>>>(DBM::fail(),{});
  }
  accepted_values.reset(clocks_to_reset);
  //Last check on the token in it's final state.
  DBM output_values = accepted_values.intersect(destination->clocks_constraints);
  output_values.normalize();

  return pair<DBM,vector<pair<bool,unordered_set<string>>>>(output_values, memory);
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
         transitions(p_transitions), alphabet(p_alphabet),
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
    for(Transition* trans : transitions[&state]){
      if(trans->clocks_constraints.getClocks_number()!=0){
        trans->clocks_constraints.addClock();
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
