#include "grammar.hpp"
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace automate;
using namespace std;


State::State(string identifiant,
             DBM clock_constraints) :
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

Clock::Clock(string const& p_name): name(p_name), value(0), increm(0){}
Clock::~Clock() = default;

void Clock::reset() {
  this->value=0;
}

void Clock::increment(double n){
  this->increm+= n;
}

void Clock::apply_increment(){
    this->value += this->increm;
    this->increm = 0;
}

bool Clock::operator==(Clock const& c2){
  return this->name==c2.name;
}

bool Clock::operator<=(Clock const& c2){
  if (this->name!=c2.name){
    cout << "The clock didn't have the same name : \"" << this->name;
    cout << "\" and \"" << c2.name <<"\"\n";
  }
  return (c2.increm.borne_sup > this->increm.borne_sup || (c2.increm.borne_sup == this->increm.borne_sup && c2.increm.include_sup >= this->increm.include_sup))
      && (this->value + this->increm) <= (c2.value + c2.increm)
      && (this->value.borne_inf > c2.value.borne_inf || (this->value.borne_inf == c2.value.borne_inf && this->value.include_inf >= c2.value.include_inf));
}

void Clock::print() const{
  cout << " " << (value.include_inf<=0?"[":"]") << (value.include_inf<0?"-":(value.include_inf>1?"+":""));
  cout << value.borne_inf << "," << value.borne_sup;
  cout << (value.include_sup<-1?"-":(value.include_sup>0?"+":"")) <<(value.include_sup>=0?"]":"[");
}

void merge_constraints(map<string,Interval> & map1, map<string,Interval> const& map2){
  for(auto map2_element : map2){
    if(! map1.insert(map2_element).second){
      map1[map2_element.first] = Interval::intersect(map1[map2_element.first], map2_element.second);
    }
  }
}

Transition::Transition(State* const& ori, State* const& dest, vector<string> events,
           map<string,Interval> const& clocks_interv,
           vector<string> const& clock_to_reset) : origine(ori), destination(dest),
           triggers(events), clocks_to_reset(clock_to_reset){
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
                             :origine(ori), destination(dest){
  merge_constraints(clocks_constraints, ori->clocks_constraints);
  merge_constraints(clocks_constraints, dest->clocks_constraints);
}
Transition::~Transition() = default;
bool Transition::epsilon(){
  return triggers.empty();
}

vector<Clock> Transition::accept(string const& event,
                                 vector<Clock> const& clocks_status){
  bool triggered = false;
  for(auto accepted_event : this->triggers){
    if(accepted_event == event){
      triggered = true;
      break;
    }
  }
  if(!triggered){
    return {};
  }
  vector<Clock> res;
  for(auto clk : clocks_status){
    Clock new_clock(clk.name);
    try{
      Interval & constraint = this->clocks_constraints.at(clk.name);
      Interval intersection = Interval::intersect(clk.value, constraint);
      if(intersection.borne_inf == -1){
        return {};
      }
      new_clock.value = intersection;
    }catch(const out_of_range& oor){new_clock = clk;}
    for(auto clock_to_reset : this->clocks_to_reset){
      if(clock_to_reset == new_clock.name){
        new_clock.reset();
        break;
      }
    }
    res.push_back(new_clock);
  }
  return this->destination->accept(res);
}

vector<Clock> Transition::accept(vector<Clock> const& clocks_status){
  if(! this->triggers.empty()){
    return {};
  }
  vector<Clock> res;
  for(auto clk : clocks_status){
    try{
      Interval & constraint = this->clocks_constraints.at(clk.name);
      Interval intersection = Interval::intersect(clk.value, constraint);
      if(intersection.borne_inf == -1){
        int increment_needed = constraint.borne_inf-clk.value.borne_sup;

        if(clk.value.borne_inf>=constraint.borne_sup //The clock's value is over the constraint.
           || clk.increm.borne_sup < increment_needed //There isn't enough increment to valide the constraint.
            //It lack an infinitly small increment.
           ||(clk.increm.borne_sup == increment_needed
             && (clk.value.include_sup + clk.increm.include_sup < constraint.include_inf))){
          return {};
        }
        intersection = constraint.borne_inf;
        intersection.include_inf = constraint.include_inf;
        intersection.include_sup = constraint.include_inf;
      }
      //Création de la nouvelle horloge
      Clock new_clock(clk.name);
      new_clock.value = intersection;
      //Increment management.
      new_clock.increm.borne_inf = max(0.0,clk.increm.borne_inf - (intersection.borne_inf - clk.value.borne_inf));
      if(intersection.borne_inf - clk.value.borne_inf > clk.increm.borne_inf){
        new_clock.increm.include_inf = 0;
      }else if(intersection.borne_inf-clk.value.borne_inf == clk.increm.borne_inf){
        new_clock.increm.include_inf = max(0,clk.increm.include_inf - (intersection.include_inf - clk.value.include_inf));
      }else{
        new_clock.increm.include_inf = clk.increm.include_inf - (intersection.include_inf - clk.value.include_inf);
      }

      new_clock.increm.borne_sup = clk.increm.borne_sup - max(0.0,(intersection.borne_sup - clk.value.borne_sup));
      if(new_clock.value.borne_sup > clk.value.borne_sup || (new_clock.value.borne_sup == clk.value.borne_sup && new_clock.value.include_sup >= clk.value.include_sup)){
        new_clock.increm.include_sup = clk.increm.include_sup - (intersection.include_sup - clk.value.include_sup);
      }else{
        new_clock.increm.include_sup = clk.increm.include_sup;
      }
      //Reset management
      for(auto clock_to_reset : this->clocks_to_reset){
        if(clock_to_reset == new_clock.name){
          if( new_clock.increm.borne_inf < (constraint.borne_sup - new_clock.value.borne_inf)){
            new_clock.increm.include_inf = 0;
          }else if(new_clock.increm.borne_inf == (constraint.borne_sup - new_clock.value.borne_inf)){
            new_clock.increm.include_inf =  max(0,new_clock.increm.include_inf - (constraint.include_sup - new_clock.value.include_inf));
          }else{
            new_clock.increm.include_inf = new_clock.increm.include_inf - (constraint.include_sup - new_clock.value.include_inf);
          }
          new_clock.increm.borne_inf = max(0.0, new_clock.increm.borne_inf - (constraint.borne_sup - new_clock.value.borne_inf));
          new_clock.reset();
          break;
        }
      }
      res.push_back(new_clock);

    //No constraint on this clock.
    }catch(const out_of_range& oor){
      Clock new_clock(clk);
      for(auto clock_to_reset : this->clocks_to_reset){
        if(clock_to_reset == new_clock.name){
          new_clock.increm.borne_inf = 0;
          new_clock.increm.include_inf = 0;
          new_clock.reset();
          break;
        }
      }
      res.push_back(new_clock);
    }
  }
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
