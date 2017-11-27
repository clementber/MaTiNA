#include <string>
#include <iostream>
#include "modelRecognizer.hpp"
#include "../fm_automata/grammar.hpp"

using namespace automate;
using namespace std;
using namespace recognizer;

Token::Token(Automate* automate, State* position){
  state = position;

}

Token;;Token(Token const& token);
Token::~Token() = default;

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
  if(modele->clocks.empty()){
    modele->find_or_create_clock("c");
  }
  map_tokens[modele->start].push_back(Token(modele));
  this->input(0.);
}
Checker::~Checker() = default;

bool Checker::check(){
  for(auto end_state : modele->endStates){
    list<Token> tokens = map_tokens[end_state];
    if(! tokens.empty()){
      return  true;
    }
  }
  return false;
}

void Checker::input(string event){
  map<State*,list<Token>> new_map_tokens;
  for(State & state : modele->states){
    for(Token & token : map_tokens[&state]){
      for(Transition & transition : modele->transitions[&state]){
        vector<Clock> new_clocks=transition.accept(event,token.clocks_values);
        if(!new_clocks.empty()){
          Token new_token(new_clocks);
          State* dest = transition.destination;
          new_map_tokens[dest].push_back(new_token);
        }
      }
    }
  }
  this->map_tokens = new_map_tokens;
}

void Checker::print_state(string str){
  if(str == ""){
    cout << "----------------current state---------------\n";
  }else{
    cout << "----------------current state : "<<str<<"-----------\n";
  }
  for(State& state : modele->states){
    cout << state.id << " : " << this->map_tokens[&state].size() << "\n";
    for(auto token : this->map_tokens[&state]){
      cout << "\t";
      cout << "token :";
      for(auto clk : token.clocks_values){
        clk.print();
      }
      cout <<"\n";
    }
  }
}
