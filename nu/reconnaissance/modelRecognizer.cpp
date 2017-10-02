#include <iostream>
#include "modelRecognizer.hpp"
#include "../nu_automata/grammar.hpp"

using namespace automate;
using namespace std;
using namespace recognizer;

Token::Token(Automate* automate){
  for(int i = 0; i< automate->ressources; i++){
    variables.push_back(pair<bool,unordered_set<string>>(false,{}));
  }
}

Token::Token(vector<pair<bool,unordered_set<string>>> & memory): variables(memory){}
Token::Token(Token const& token) = default;
Token::~Token() = default;
bool Token::operator==(Token const& tok2) const{
  if(this->variables.size() != tok2.variables.size()){
    return false;
  }
  //Memory state aren't an order, the only comparison possible is the equality.
  for(int i=0; i<tok2.variables.size();i++){
    if(this->variables[i].first != tok2.variables[i].first
     ||this->variables[i].second.size() != tok2.variables[i].second.size()){
      return false;
    }
  }
  for(int i=0; i<tok2.variables.size();i++){
    if(this->variables[i] != tok2.variables[i]){
      return false;
    }
  }
  return true;
}

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
  map_tokens[modele->start].push_back(Token(modele));
  this->propagate();
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

void Checker::propagate(){
  vector<pair<State*,Token>> to_evaluate;
  //First step : collect every token and their positions.
  for(State & state : modele->states){
    for(Token const& token : map_tokens[&state]){
      to_evaluate.push_back(pair<State*,Token>(&state, token));
    }
  }

  map<State*,list<Token>> new_map_tokens;
  //Second step : put the token and calculate their sons
  while(!to_evaluate.empty()){
    State * evaluate_state = to_evaluate.back().first;
    Token evaluate_token = to_evaluate.back().second;
    to_evaluate.pop_back();
    if(!Token::included(evaluate_token, new_map_tokens[evaluate_state])){
      new_map_tokens[evaluate_state].push_back(evaluate_token);
      for(Transition * trans : modele->transitions[evaluate_state]){
        vector<pair<bool,unordered_set<string>>> new_memory_state = trans->accept_epsilon(evaluate_token.variables);
        if(!new_memory_state.empty()){
          to_evaluate.push_back(pair<State*,Token>(trans->destination,Token(new_memory_state)));
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
        vector<pair<bool,unordered_set<string>>> new_memory_state;
        if(isConstant){
          new_memory_state = transition->accept_constant(token.variables,event);
        }else{
          new_memory_state = transition->accept_event(token.variables,event);
        }
        if( ! new_memory_state.empty()){
          new_map_tokens[transition->destination].push_back(Token(new_memory_state));
        }
      }
    }
  }
  this->map_tokens = new_map_tokens;

  //Third step : propagate the new tokens.
  propagate();

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
      for(int i =0; i<token.variables.size(); i++){
        cout << " "<< i << " : {";
        for(string str : token.variables[i].second){
          cout <<str << "; ";
        }
        cout << "}";
      }
      cout <<"\n";
    }
  }
}
