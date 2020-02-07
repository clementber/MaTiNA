#ifndef __AUTOMATE_RECOGNIZER__
#define __AUTOMATE_RECOGNIZER__

#include "../lama_automata/grammar.hpp"

using namespace std;
using namespace automate;

namespace recognizer{
//----------------------------------TOKEN-------------------------------------//
  class Token{
  public:
    Valuation value;

    Token(Token const& token);
    Token(Valuation const& val);
    Token(Automate * autom);
    ~Token();

    bool operator==(Token const& tok2) const;
    string to_string() const;

    static bool included(Token const& tok, list<Token> const& list){
      for(Token const& elmt : list){
        if(tok == elmt){
          return true;
        }
      }
      return false;
    }
  };

//----------------------------------CHECKER-----------------------------------//
  class Checker{
  public:
    Automate* modele;
    map<State*,list<Token>> map_tokens;
    
    Checker(Automate* modele_automate);
    ~Checker();
    
    bool check();
    void epsi_closure();
    void input(string event);
    void print_state(string str);
  };
}

#endif
