#ifndef __GRAMMAR_AUTOMATE__
#define __GRAMMAR_AUTOMATE__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <unordered_set>

//DEBUG
//#include <iostream>

using namespace std;

namespace automate{
  class State{
    public:
      string id;

      State();
      State(string identifiant);
      ~State();
  };

  class Transition{
  public:
    State* origine;
    State* destination;

    vector<int> allocations;
    vector<int> frees;
    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez);
    Transition(State* const& ori, State* const& dest);
    virtual ~Transition();
    virtual vector<pair<bool,unordered_set<string>>> accept_epsilon(
        vector<pair<bool,unordered_set<string>>> memory);
    virtual vector<pair<bool,unordered_set<string>>> accept_event(
        vector<pair<bool,unordered_set<string>>> memory,string event);
    virtual vector<pair<bool,unordered_set<string>>> accept_constant(
        vector<pair<bool,unordered_set<string>>> memory,string constant);
    virtual string to_string()=0;
  };

  class Epsilon_Transition : public Transition{
  public:
    Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez);
    Epsilon_Transition(State* const& ori, State* const& dest);
    ~Epsilon_Transition();
    vector<pair<bool,unordered_set<string>>> accept_epsilon(
        vector<pair<bool,unordered_set<string>>> memory);
    string to_string();
  };

  class Event_Transition : public Transition{
  public:
    int variable;

    Event_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, int variable);
    Event_Transition(State* const& ori, State* const& dest, int variable);
    ~Event_Transition();
    vector<pair<bool,unordered_set<string>>> accept_event(
        vector<pair<bool,unordered_set<string>>> memory,string event);
    string to_string();
  };

  class Constant_Transition : public Transition{
  public:
    string constant;

    Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, string constant);
    Constant_Transition(State* const& ori, State* const& dest, string constant);
    ~Constant_Transition();
    vector<pair<bool,unordered_set<string>>> accept_constant(
        vector<pair<bool,unordered_set<string>>> memory,string constant);
    string to_string();
  };

  class Automate{
  public:
    int ressources;
    unordered_set<string> constants;
    list<State> states;
    map<State*,vector<Transition*>> transitions;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(int p_ressources, unordered_set<string> p_constants,
            list<State> p_states, map<State*, vector<Transition*>> p_transitions,
            State* p_start, vector<State*> p_endStates);

      ~Automate();
      State* getState(string state_name);
  };
};

#endif
