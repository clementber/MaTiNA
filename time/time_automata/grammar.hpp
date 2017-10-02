#ifndef __GRAMMAR_AUTOMATE__
#define __GRAMMAR_AUTOMATE__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>

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
  bool isValid();

  static Interval intersect(Interval const& interv1, Interval const& interv2){
    double borne_inf, borne_sup;
    int include_inf, include_sup;

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
    //DEBUG
    //cout << "In include : "<< include_inf << "|" << borne_inf <<","<<borne_sup<<"|"<<include_sup<<"\n";

    Interval res(borne_inf,include_inf,borne_sup,include_sup);
    return res;
  }
};

  class Clock{
  public:
    string name;
    Interval value;
    Interval increm;

    Clock(string const& p_name);
    ~Clock();

    void reset();
    void increment(double n);
    void apply_increment();
    bool operator==(Clock const& c2);
    //Kind of subset with management of increment.
    bool operator<=(Clock const& c2);
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
    vector<string> triggers;
    map<string,Interval> clocks_constraints;
    vector<string> clocks_to_reset;

    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<string> events,
               map<string,Interval> const& clocks_interv,
               vector<string> const& clock_to_reset);
    Transition(State* const& ori, State* const& dest);
    ~Transition();

    bool epsilon();
    /**
    * Accept return an empty vector of clock if clocks_status don't respect the
    * constraints. Else, it return a new vector of clock whose the possible
    * values of the clock at the end of the transition.
    * If an event is given as paramter, the transition will return an empty
    * vector if the event isn't one o it's triggers. However, if no events are
    * given as parameter, only the transition with no triggers can be crossed.
    */
    vector<Clock> accept(string const& event, vector<Clock> const& clocks_status);
    vector<Clock> accept(vector<Clock> const& clocks_status);
  };

  class Automate{
  public:
    list<State> states;
    vector<Clock> clocks;
    map<State*,vector<Transition>> transitions;
    set<string> alphabet;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(list<State> p_states,
               vector<Clock> p_clocks,
               map<State*, vector<Transition>> p_transitions,
               set<string> p_alphabet,
               State* p_start,
               vector<State*> p_endStates);

      ~Automate();
      Clock* find_or_create_clock(string name);
      State* getState(string state_name);
  };
};

#endif
