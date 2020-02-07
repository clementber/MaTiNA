#include <iostream>
#include <sstream>
#include "modelRecognizer.hpp"

using namespace automate;
using namespace std;
using namespace recognizer;

//-----------------------------TOKEN------------------------------------------//
Token::Token(Automate* automate): value(automate->initial_valuation){}
Token::Token(Token const& token) = default;
Token::Token(Valuation const& val): value(val){}

Token::~Token() = default;


bool Token::operator==(Token const& tok2) const{
  return (this->value == tok2.value);
}

string Token::to_string() const{
  return value.to_string();
}

//---------------------------Checker-----------------------------------------//

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
  map_tokens[modele->start].push_back(Token(modele));
  this->epsi_closure();
}
Checker::~Checker() = default;

bool Checker::check(){
  for(auto end_state : modele->endStates){
    if(! map_tokens[end_state].empty()){
      return  true;
    }
  }
  return false;
}

void Checker::epsi_closure(){
  vector<pair<State*,Token>> to_evaluate;
  //First step : collect every token and their positions.
  for(State & state : modele->states){
    for(Token const& token : map_tokens[&state]){
      to_evaluate.push_back(pair<State*,Token>(&state, token));
    }
  }
  map<State*,list<Token>> new_map_tokens;
  //Second step : propagate the token through transitions
  while(!to_evaluate.empty()){
    State * evaluate_state = to_evaluate.back().first;
    Token evaluate_token = to_evaluate.back().second;
    to_evaluate.pop_back();
    if(!Token::included(evaluate_token, new_map_tokens[evaluate_state])){
      new_map_tokens[evaluate_state].push_back(evaluate_token);
      for(Transition * trans : modele->transitions[evaluate_state]){
        Valuation new_token_value = trans->accept_epsilon(evaluate_token.value);
        if( new_token_value.nb_layers() != 0){
          Token new_token(new_token_value);
          to_evaluate.push_back(pair<State*,Token>(trans->destination,new_token));
        }
      }
    }
  }
  //Final step : save the new system state.
  this->map_tokens = new_map_tokens;
}

void Checker::input(string event){
  //First step : Find if the event is a constant.
  bool isConstant = modele->constants.find(event) != modele->constants.end();

  //Second step : make the tokens consume the event.
  map<State*,list<Token>> new_map_tokens;
  for(State & state : modele->states){
    for(Token const& token : map_tokens[&state]){
      for(Transition * transition : modele->transitions[&state]){
        Valuation new_token_value;
        if(isConstant){
          new_token_value = transition->accept_constant(token.value,event);
        }else{
          new_token_value = transition->accept_value(token.value,event);
        }
        if( new_token_value.nb_layers() != 0 ){
          new_map_tokens[transition->destination].push_back(Token(new_token_value));
        }
      }
    }
  }
  this->map_tokens = new_map_tokens;
  //Third step : epsi_closure the new tokens.
  epsi_closure();

}

void Checker::print_state(string state_name){
  if(state_name == ""){
    cout << "----------------current state---------------\n";
  }else{
    cout << "----------------current state : "<<state_name<<"-----------\n";
  }
  for(State& state : modele->states){
    cout << state.id << " : " << this->map_tokens[&state].size() << "\n";
    for(auto token : this->map_tokens[&state]){
      cout << "\t";
      cout << "token : ";

      cout << token.to_string();
    }
  }
}
