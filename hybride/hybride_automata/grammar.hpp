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
  class Automate;
  class Clock;
  class Bound{
  public:
    double value;
    int inclusion; //Inclusion level : 0 is <= ; -1 is <

    Bound();
    Bound(double value);
    Bound(double value, int included);

    Bound operator+(Bound const& bound2) const;
    bool operator==(Bound const& bound2) const;
    bool operator!=(Bound const& bound2) const;
    bool operator<(Bound const& bound2) const;
    bool operator<=(Bound const& bound2) const;
    Bound min(Bound const& bound2) const;
  };
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

    DBM(); //Create a DBM of length 0.
    DBM(DBM const& dbm);
    DBM(int clocks_number);
    DBM(Automate const& autom);

    //Return a DBM describing no zone.
    static DBM fail(){
      DBM invalid_dbm = DBM();
      invalid_dbm.matrice[0][0]=-1;
      return invalid_dbm;
    }
    int getClocks_number() const;
    void addClock();

    //Time modification operators
    void increment(double time_delay);
    //The value of the given clocks are set to zero.
    void reset(unordered_set<Clock*> clk);
    //The value of the given clocks became the interval [0,+inf].
    void maximize(unordered_set<Clock*> clks);

    //Emptiness predicat. Return false when the clocks zone described is empty.
    bool empty() const;

    //Reductions operator.
    void normalize();

    //Projection operator : Generate the zone containing every reachable values
    //between the current DBM and the destination.
    DBM project(DBM const& destination) const;

    //Intersection operator.
    DBM intersect(DBM const& dbm2) const;
    //Subset operators
    bool operator<(DBM const& dbm2) const;
    bool operator<=(DBM const& dbm2) const;
  };


  class Clock{
  private:
    int id;
  public:
    string name;

    Clock(string const& p_name, int id);
    ~Clock();

    int getId() const;
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

      DBM accept(DBM const& clocks_status);
  };

  class Transition{
  public:
    State* origine;
    State* destination;
    vector<int> allocations;
    vector<int> frees;
    DBM clocks_constraints;
    unordered_set<Clock*> clocks_to_reset;

    /**
    * with the first constructor, the constraints of the transitions will be
    * the intersection of clocks_interv, origine->clocks_constraints and
    * destination->clocks_constraints.
    */
    Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, DBM const& clocks_interv,
               unordered_set<Clock*> const& clock_to_reset);
    Transition(State* const& ori, State* const& dest);
    virtual ~Transition();
    virtual string to_string();
    virtual pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        DBM initial_clocks_status, DBM current_clocks_status,
        DBM final_clocks_status,vector<pair<bool,unordered_set<string>>> memory);
    virtual pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory);
    /**The parameter 'event' isn't present in the unordered set 'alphabet' of 
    * the automaton containing this transition.
    */
    virtual pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_event(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
        string event);
    virtual pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_constant(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
        string constant);
  };

  class Epsilon_Transition : public Transition{
  public:
    Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, DBM const& clocks_interv,
               unordered_set<Clock*> const& clock_to_reset);
    Epsilon_Transition(State* const& ori, State* const& dest);
    ~Epsilon_Transition();
    string to_string();
    pair<vector<DBM>,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        DBM initial_clocks_status, DBM current_clocks_status,
        DBM final_clocks_status,vector<pair<bool,unordered_set<string>>> memory);
    pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_epsilon(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory);
  };

  class Event_Transition : public Transition{
  public:
    int variable;

    Event_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, DBM const& clocks_interv,
               unordered_set<Clock*> const& clock_to_reset, int variable);
    Event_Transition(State* const& ori, State* const& dest, int variable);
    ~Event_Transition();
    string to_string();
    pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_event(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
        string event);
  };

  class Constant_Transition : public Transition{
  public:
    string constant;

    Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
               vector<int> freez, DBM const& clocks_interv,
               unordered_set<Clock*> const& clock_to_reset,
               string constant);
    Constant_Transition(State* const& ori, State* const& dest, string constant);
    ~Constant_Transition();
    string to_string();
    pair<DBM,vector<pair<bool,unordered_set<string>>>> accept_constant(
        DBM clocks_status ,vector<pair<bool,unordered_set<string>>> memory,
        string constant);
  };

  class Automate{
  public:
    int ressources;
    list<State> states;
    vector<Clock*> clocks;
    map<State*,vector<Transition*>> transitions;
    unordered_set<string> alphabet;
    State* start;
    vector<State*> endStates;

      Automate();
      Automate(int p_ressources,
               list<State> p_states,
               vector<Clock*> p_clocks,
               map<State*, vector<Transition*>> p_transitions,
               unordered_set<string> p_alphabet,
               State* p_start,
               vector<State*> p_endStates);

      ~Automate();
      Clock* find_or_create_clock(string name);
      State* getState(string state_name);
  };
};

#endif
