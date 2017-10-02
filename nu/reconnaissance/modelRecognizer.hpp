#ifndef __AUTOMATE_RECOGNIZER__
#define __AUTOMATE_RECOGNIZER__

#include "../nu_automata/grammar.hpp"

using namespace std;
using namespace automate;

namespace recognizer{
  class Token{
  public:
    vector<pair<bool,unordered_set<string>>> variables;

    //--------------------------------------------------------------------------//
    //---------------------------Constructor------------------------------------//
    //--------------------------------------------------------------------------//
    Token(Automate* automate);
    Token(Token const& token);
    Token(vector<pair<bool,unordered_set<string>>> & memory);
    ~Token();

    //--------------------------------------------------------------------------//
    //--------------------------------Methods-----------------------------------//
    //--------------------------------------------------------------------------//
    bool operator==(Token const& tok2) const;

    static bool included(Token tok, list<Token> list){
      for(Token const& elmt : list){
        if(tok == elmt){
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

    void propagate();
    void input(string event);

    void print_state(string str);
  };
}

#endif
