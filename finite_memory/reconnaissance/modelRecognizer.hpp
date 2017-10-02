#ifndef __AUTOMATE_RECOGNIZER__
#define __AUTOMATE_RECOGNIZER__

#include "../time_automata/grammar.hpp"

using namespace std;
using namespace automate;

namespace recognizer{

  class Token{
  public:
    vector<string> registre;
    State* state;

    //--------------------------------------------------------------------------//
    //---------------------------Constructor------------------------------------//
    //--------------------------------------------------------------------------//
    Token(Automate* automate);
    Token(Token const& token);
    ~Token();

    //--------------------------------------------------------------------------//
    //--------------------------------Methods-----------------------------------//
    //--------------------------------------------------------------------------//
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

    void input(string event);

    void print_state(string str);
  };
}

#endif
