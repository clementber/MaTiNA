#ifndef __GRAMMAR_AUTOMATE__
#define __GRAMMAR_AUTOMATE__

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <list>
#include <unordered_set>

//DEBUG
//#include <iostream>

using namespace std;

namespace automate{
  class Variable{
    public:
     int name, layer;
     
     Variable(int name, int layer);
     string to_string() const;
  }; 

  class Valuation{
    private:
      unsigned int nb_layers, nb_variables;
      vector<vector<pair<bool,unordered_set<string>>>> value;
      
      bool can_use(vector<Variable> const& to_use, string const& event);
    public:
      Valuation();
      Valuation(unsigned int nb_layers, unsigned int nb_variables);
      Valuation(Valuation const& original);
      Valuation(vector<vector<vector<string>>> const& source, int nb_variables);
      ~Valuation();
      
      int get_nb_layers();
      int get_nb_variables();
      bool isFresh(int const& layer, string const& event) const;
      void alloc(vector<Variable> const& to_alloc);
      void desalloc(vector<Variable> const& to_free);
      bool use(vector<Variable> const& to_use, string const& event);
      string to_string() const;
  };

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

    vector<Variable> allocations;
    vector<Variable> frees;
    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, 
               vector<Variable> allocs, vector<Variable> freez);
    Transition(State* const& ori, State* const& dest);
    virtual ~Transition();
    virtual Valuation accept_epsilon(Valuation memory);
    virtual Valuation accept_value(Valuation memory,string event);
    virtual string to_string() const =0;
  };

  class Epsilon_Transition : public Transition{
  public:
    Epsilon_Transition(State* const& ori, State* const& dest, 
               vector<Variable> allocs, vector<Variable> freez);
    Epsilon_Transition(State* const& ori, State* const& dest);
    ~Epsilon_Transition();
    Valuation accept_epsilon(Valuation memory);
    string to_string() const;
  };

  class Var_Transition : public Transition{
  public:
    vector<Variable> triggers;

    Var_Transition(State* const& ori, State* const& dest, 
               vector<Variable> allocs, vector<Variable> freez, 
               vector<Variable> var);
    Var_Transition(State* const& ori, State* const& dest, 
                     vector<Variable> variable);
    ~Var_Transition();
    Valuation accept_value(Valuation memory,string event);
    string to_string() const;
  };

  class Constant_Transition : public Transition{
  public:
    string constant;
    
    Constant_Transition(State* const& ori, State* const& dest, 
               vector<Variable> allocs, vector<Variable> freez,string constant);
    Constant_Transition(State* const& ori, State* const& dest, string constant);
    ~Constant_Transition();
    Valuation accept_value(Valuation memory,string event);
    string to_string() const;
  };
  
  class Universal_Transition : public Transition{
  public:

    Universal_Transition(State* const& ori, State* const& dest, 
               vector<Variable> allocs, vector<Variable> freez);
    Universal_Transition(State* const& ori, State* const& dest);
    ~Universal_Transition();
    Valuation accept_value(Valuation memory,string constant);
    string to_string() const;
  };

  class Automate{
  public:
    Valuation initial_valuation;
    list<State> states;
    map<State*,vector<Transition*>> transitions;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(Valuation initial_valuation, list<State> p_states, 
               map<State*, vector<Transition*>> p_transitions, State* p_start, 
               vector<State*> p_endStates);

      ~Automate();
      State* getState(string state_name);
  };
};

#endif
