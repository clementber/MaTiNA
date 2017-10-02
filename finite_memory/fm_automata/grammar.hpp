#ifndef __GRAMMAR_AUTOMATE__
#define __GRAMMAR_AUTOMATE__

#include <string>
#include <vector>
#include <map>
#include <list>

//DEBUG
//#include <iostream>

using namespace std;

namespace automate{
  class State{
    public:
      string id;
      vector<int> assignable;

      State();
      State(string identifiant);
      State(string identifiant, vector<int> assignable_register);
      ~State();
  };

  class Transition{
  public:
    State* origine;
    State* destination;
    vector<int> triggers;

    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<int> events);
    Transition(State* const& ori, State* const& dest);
    ~Transition();
  };

  class Automate{
  public:
    list<State> states;
    map<State*,vector<Transition>> transitions;
    vector<string> initial_config;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(list<State> p_states,
               map<State*, vector<Transition>> p_transitions,
               vector<string> p_initial_conf,
               State* p_start,
               vector<State*> p_endStates);

      ~Automate();
      State* getState(string state_name);
  };
};

#endif
