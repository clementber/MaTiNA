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

  class Bound{
  public:
    double value;
    int inclusion; //Inclusion level : 0 is <= ; -1 is <

    Bound();
    Bound(double value);
    Bound(double value, bool included);

    Bound operator+(Bound const& bound2) const;
    bool operator==(Bound const& bound2) const;
    bool operator!=(Bound const& bound2) const;
    bool operator<(Bound const& bound2) const;
    bool operator<=(Bound const& bound2) const;
    Bound min(Bound const& bound2) const;
  }
  /**Differences Bounds Matrice
  * This class is a matrice which contain for each pair of (row,column) the
  * maximal bound of the clocks values substraction row_clockId - cloumn_clockId.
  * Each bound is a pair (double value, {LESS, LESSEQ} inclusion).
  * The first row and column of the matrice is for the clock_0 which value is
  * always 0. This clock is used to know the current interval of value for all
  * the other clock.
  *
  * If matrice[row][column] = (val,~) Then, there exist clocks c1 and c2
  * where c1.id == row and c2.id == column and c1-c2 ~ val. (~ in {<, <=}).
  **/
  class DBM{
  public:
    vector<vector<Bound>> matrice;
    int length;
    DBM();
    DBM(DBM dbm);
    DBM(int clocks_number);
    DBM(Automate automate);

    //Return a DBM describing no zone.
    static DBM fail();
    int getClocks_number();

    //Time modification operators
    void increment(double time_delay);
    //The value of the given clocks are set to zero.
    void reset(vector<Clock*> clk);
    //The value of the given clocks became the interval [0,+inf].
    void maximize(vector<Clock*> clks);

    //Emptiness predicat. Return false when the clocks zone described is empty.
    bool empty() const;

    //Reductions operator.
    void normalize();

    //Intersection operator.
    DBM intersect(DBM const& dbm2) const;
    //Subset operators
    bool operator<(DBM const& dbm2) const;
    bool operator<=(DBM const& dbm2) const;

  }


  class Clock{
  private:
    int id;
  public:
    string name;

    Clock(string const& p_name, int id);
    ~Clock();

    int getId();
    void print() const;
  };

  class State{
    public:
      string id;
      DBM clocks_constraints;

      State();
      State(string identifiant);
      State(string identifiant, DBM clocks_constraints);
      ~State();

      DBM accept(DBM const& clocks_status) const;
  };

  class Transition{
  public:
    State* origine;
    State* destination;
    vector<string> triggers;
    DBM clocks_constraints;
    vector<Clock*> clocks_to_reset;

    /**
    * with the first constructor, the constraints of the transitions will be the
    * intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<string> events,
               DBM const& clocks_interv,
               vector<Clock*> const& clock_to_reset);
    Transition(State* const& ori, State* const& dest);
    ~Transition();

    bool epsilon() const;
    /**
    * Accept return an empty vector of clock if clocks_status don't respect the
    * constraints. Else, it return a new vector of clock whose the possible
    * values of the clock at the end of the transition.
    * If an event is given as paramter, the transition will return an empty
    * vector if the event isn't one o it's triggers. However, if no events are
    * given as parameter, only the transition with no triggers can be crossed.
    */
    DBM accept(string const& event, DBM const& clocks_status) const;
    vector<DBM> accept(DBM initial_clocks_status,
                       DBM current_clocks_status,
                       DBM final_clocks_status) const;
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
