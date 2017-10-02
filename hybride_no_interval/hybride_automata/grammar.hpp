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
class Interval{
public:
  double borne_inf, borne_sup;
  int include_inf, include_sup;

  Interval();
  Interval(double constant);
  Interval(double born_inf, double born_sup);
  Interval(double born_inf, bool incl_inf, double born_sup, bool incl_sup);
  Interval(double born_inf, int incl_inf, double born_sup, int incl_sup);

  //SUBSET!!
  bool operator<=(Interval const& interv);
  void operator=(double const val);
  bool operator==(Interval const& interv) const;
  Interval operator+(Interval const& interv) const;
  void operator+=(Interval const& interv);
  void operator+=(double const val);
  void validate();
  bool isValid();

  static Interval intersect(Interval interv1, Interval interv2){
    double borne_inf, borne_sup;
    int include_inf, include_sup;
    interv1.validate();
    interv2.validate();

    if(interv1.borne_inf > interv2.borne_inf){
      borne_inf = interv1.borne_inf;
      include_inf = interv1.include_inf;
    }else if(interv1.borne_inf < interv2.borne_inf){
      borne_inf = interv2.borne_inf;
      include_inf = interv2.include_inf;
    }else{
      borne_inf = interv1.borne_inf;
      include_inf = max(interv1.include_inf, interv2.include_inf);
    }

    if(interv1.borne_sup < interv2.borne_sup){
      borne_sup = interv1.borne_sup;
      include_sup = interv1.include_sup;
    }else if(interv1.borne_sup > interv2.borne_sup){
      borne_sup = interv2.borne_sup;
      include_sup = interv2.include_sup;
    }else{
      borne_sup = interv1.borne_sup;
      include_sup = min(interv1.include_sup , interv2.include_sup);
    }

    Interval res(borne_inf,include_inf,borne_sup,include_sup);
    return res;
  }

  static Interval validate(Interval const& interv){
    Interval res(interv);
    res.validate();
    return res;
  }
};

  class Clock{
  public:
    string name;
    double value;
    int include;

    Clock(string const& p_name);
    ~Clock();

    void reset();
    bool operator==(Clock const& c2);
    void print() const;
  };

  class State{
    public:
      string id;
      map<string,Interval> clocks_constraints;

      State();
      State(string identifiant);
      State(string identifiant, map<string,Interval> clocks_constraints);
      ~State();

      vector<Clock> accept(vector<Clock> const& clocks_status);
  };

  class Transition{
  public:
    State* origine;
    State* destination;
    vector<int> allocations;
    vector<int> frees;
    map<string,Interval> clocks_constraints;
    unordered_set<string> clocks_to_reset;

    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, map<string,Interval> const& clocks_interv,
               unordered_set<string> const& clock_to_reset);
    Transition(State* const& ori, State* const& dest);
    virtual ~Transition();
    virtual string to_string();
    virtual bool epsilon(){return false;}
    virtual int dynamique(){return -1;}
    virtual string statique(){return "";}
    virtual Transition* copy();
    virtual pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, double & increm, int & incl_increm);
    virtual pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_event(
        pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
        string event);
    virtual pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_constant(
      pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
      string constant);
  };

  class Epsilon_Transition : public Transition{
  public:
    Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, map<string,Interval> const& clocks_interv,
               unordered_set<string> const& clock_to_reset);
    Epsilon_Transition(State* const& ori, State* const& dest);
    ~Epsilon_Transition();
    string to_string();
    Transition* copy();
    bool epsilon(){return true;}
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value, double &increm, int &incl_increm);
  };

  class Event_Transition : public Transition{
  public:
    int variable;

    Event_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, map<string,Interval> const& clocks_interv,
               unordered_set<string> const& clock_to_reset, int variable);
    Event_Transition(State* const& ori, State* const& dest, int variable);
    ~Event_Transition();
    string to_string();
    Transition* copy();
    int dynamique(){ return variable; }
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_event(
        pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
        string event);
  };

  class Constant_Transition : public Transition{
  public:
    string constant;

    Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, map<string,Interval> const& clocks_interv,
               unordered_set<string> const& clock_to_reset,
               string constant);
    Constant_Transition(State* const& ori, State* const& dest, string constant);
    ~Constant_Transition();
    string to_string();
    Transition* copy();
    string statique(){ return constant;}
    pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> accept_constant(
        pair<vector<Clock>,vector<pair<bool,unordered_set<string>>>> value,
        string constant);
  };

  class Automate{
  public:
    int ressources;
    list<State> states;
    vector<Clock> clocks;
    map<State*,vector<Transition*>> transitions;
    unordered_set<string> constants;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(int p_ressources,
               list<State> p_states,
               vector<Clock> p_clocks,
               map<State*, vector<Transition*>> p_transitions,
               unordered_set<string> p_constants,
               State* p_start,
               vector<State*> p_endStates);

      ~Automate();
      Clock* find_or_create_clock(string name);
      State* getState(string state_name);
  };
};

#endif
