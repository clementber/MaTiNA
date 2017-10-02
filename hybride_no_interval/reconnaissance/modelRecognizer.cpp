#include <iostream>
#include <sstream>
#include "modelRecognizer.hpp"

using namespace automate;
using namespace std;
using namespace recognizer;

typedef pair<vector<Clock>, vector<pair<bool,unordered_set<string>>>> token_content;

Token::Token(Automate* automate): increm_val(0),increm_inc(0){
  for(Clock clk : automate->clocks){
    this->clocks.push_back(clk);
  }
  for(int i = 0; i< automate->ressources; i++){
    variables.push_back(pair<bool,unordered_set<string>>(false,{}));
  }
}

Token::Token(vector<Clock> p_clocks, vector<pair<bool,unordered_set<string>>> & memory)
      : clocks(p_clocks), increm_val(0), increm_inc(0), variables(memory){}
Token::Token(Token const& token) = default;
Token::~Token() = default;
void Token::increment(double time_elapse){
  increm_val = time_elapse;
}

bool Token::operator<=(Token const& tok2) const{
  if(this->clocks.size() != tok2.clocks.size()){
    return false;
  }
  if(this->increm_val != tok2.increm_val || this->increm_inc != tok2.increm_inc){
    return false;
  }
  for(int i =0; i< this->clocks.size(); i++){
    if(this->clocks[i].value != tok2.clocks[i].value || this->clocks[i].include != tok2.clocks[i].include){
      return false;
    }
  }
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

void Token::validate(){
  for(Clock & clock : clocks){
    clock.value += this->increm_val;
    clock.include += this->increm_inc;
  }
  this->increm_val = 0;
  this->increm_inc = 0;
}

string Token::toString() const{
  stringstream res;
  for(unsigned int i=0; i<this->clocks.size(); i++){
    res <<  this->clocks[i].name << " : ";
    res << "{" << this->clocks[i].value;
    res << (this->clocks[i].include > 0? "+" : (this->clocks[i].include < 0? "-" :"")) << "}";
    res <<  " ";
  }
  res <<  "\n        ";
  for(unsigned int i =0; i<this->variables.size(); i++){
    res <<  " "<< i << " : {";
    for(string str : this->variables[i].second){
      res << str << "; ";
    }
    res <<  "}";
  }
  res << "\n";
  return res.str();
}

//---------------------------End of class Token-------------------------------//

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
  if(modele->clocks.empty() && modele->ressources == 0){
    modele->ressources++;
  }
  map_tokens[modele->start].push_back(Token(modele));
  this->propagate(false);
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

void Checker::propagate(bool incremented){
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
        double increm_val = evaluate_token.increm_val;
        int increm_inc = evaluate_token.increm_inc;
        token_content new_token_value = trans->accept_epsilon(token_content(evaluate_token.clocks, evaluate_token.variables), increm_val, increm_inc);
        if(!(new_token_value.first.empty() && new_token_value.second.empty())){
          Token new_token(new_token_value.first, new_token_value.second);
          new_token.increm_val = increm_val;
          new_token.increm_inc = increm_inc;
          to_evaluate.push_back(pair<State*,Token>(trans->destination,new_token));
        }
      }
    }
  }

  //Third step : only if clock were incremented, finish to  apply the increment
  // and check if the token still respects their state constraints.
  if(incremented){
    map<State*,list<Token>> final_map_tokens;
    for(auto tokens : new_map_tokens){
      for(Token & token : tokens.second){
        token.validate();
        vector<Clock> accepted_clocks = tokens.first->accept(token.clocks);
        if( ! accepted_clocks.empty()){
          final_map_tokens[tokens.first].push_back(Token(accepted_clocks, token.variables));
        }
      }
    }
    // Avoid to copy the map more than necessary.
    this->map_tokens = final_map_tokens;
    return;
  }

  //Final step : save the new system state.
  this->map_tokens = new_map_tokens;
}

void Checker::input(double time_elapse){
  for(State & state : modele->states){
    for(Token & token : map_tokens[&state]){
      token.increment(time_elapse);
    }
  }

  propagate(true);
}

void Checker::input(string event){
  //First step : Find if the event is a constant.
  bool isConstant = modele->constants.find(event) != modele->constants.end();

  //Second step : make the tokens consume the event.
  map<State*,list<Token>> new_map_tokens;
  for(State & state : modele->states){
    for(Token const& token : map_tokens[&state]){
      for(Transition * transition : modele->transitions[&state]){
        token_content new_token_value;
        if(isConstant){
          new_token_value = transition->accept_constant(token_content(token.clocks,token.variables),event);
        }else{
          new_token_value = transition->accept_event(token_content(token.clocks, token.variables),event);
        }
        if( ! (new_token_value.first.empty() && new_token_value.second.empty())){
          new_map_tokens[transition->destination].push_back(Token(new_token_value.first, new_token_value.second));
        }
      }
    }
  }
  this->map_tokens = new_map_tokens;

  //Third step : propagate the new tokens.
  propagate(false);

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

      cout << token.toString();
    }
  }
}
