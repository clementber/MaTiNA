#ifndef __AUTOMATE_RECOGNIZER__
#define __AUTOMATE_RECOGNIZER__

#include "../time_automata/grammar.hpp"

using namespace std;
using namespace automate;

namespace recognizer{

  class Token{
  public:
    DBM initial_values;
    DBM current_values;
    DBM final_values;

    //------------------------------------------------------------------------//
    //---------------------------Constructor----------------------------------//
    //------------------------------------------------------------------------//
    Token(Automate* automate);
    Token(DBM initial_v,DBM current_v,DBM final_v);
    Token(Token const& token);
    ~Token();

    //------------------------------------------------------------------------//
    //--------------------------------Methods---------------------------------//
    //------------------------------------------------------------------------//
    void increment(double time_elapse);

    void apply_increment();

    bool operator<=(Token const& tok2);

    static bool included(Token tok, list<Token> list){
      for(auto tok_of_list : list){
        if(tok <= tok_of_list){
          return true;
        }
      }
      return false;
    }
  };


  class Checker{
  private:
    Automate* modele;
    map<State*,list<Token>> map_tokens;

  public:
    Checker(Automate* modele_automate);
    ~Checker();

    bool check();

    void input(double time_elapse);

    void input(string event);

    void print_state(string str);
  };
}

#endif
