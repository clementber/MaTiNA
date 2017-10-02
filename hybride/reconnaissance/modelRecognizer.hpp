#ifndef __AUTOMATE_RECOGNIZER__
#define __AUTOMATE_RECOGNIZER__

#include "../hybride_automata/grammar.hpp"

using namespace std;
using namespace automate;

namespace recognizer{
  class Token{
  public:
    vector<Clock> clocks;
    Interval increm;
    vector<pair<bool,unordered_set<string>>> variables;

  //--------------------------------------------------------------------------//
  //---------------------------Constructor------------------------------------//
  //--------------------------------------------------------------------------//
    Token(Automate* automate);
    Token(Token const& token);
    Token(vector<Clock> clocks, vector<pair<bool,unordered_set<string>>> & memory);
    ~Token();

  //--------------------------------------------------------------------------//
  //--------------------------------Methods-----------------------------------//
  //--------------------------------------------------------------------------//
    bool operator<=(Token const& tok2) const;
    void increment(double time_elapse);
    void validate();

    static bool included(Token const& tok, list<Token> const& list){
      for(Token const& elmt : list){
        if(tok <= elmt){
          return true;
        }
      }
      return false;
    }

    static Token validate(Token tok){
      tok.validate;
      return tok;
    }
  };

  class Checker{
  public:
    Automate* modele;
    map<State*,list<Token>> map_tokens;
    Checker(Automate* modele_automate);
    ~Checker();

    bool check();

    void propagate(bool incremented);
    void input(double time_elapse);
    void input(string event);

    void print_state(string str);
  };
}

#endif
