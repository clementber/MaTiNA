#include <iostream>
#include "modelRecognizer.hpp"
#include <limits>
#include <sstream>

using namespace automate;
using namespace std;
using namespace recognizer;

typedef pair<DBM, vector<pair<bool,unordered_set<string>>>> token_content;

Token::Token(Automate* automate){
  initial_values = DBM(*automate);
  current_values = initial_values;
  final_values = initial_values;
  for(int i = 0; i< automate->ressources; i++){
    variables.push_back(pair<bool,unordered_set<string>>(false,{}));
  }
}

Token::Token(DBM initial_v,DBM current_v,DBM final_v,
            vector<pair<bool,unordered_set<string>>> & memory)
      : initial_values(initial_v), current_values(current_v),
        final_values(final_v), variables(memory){}
Token::Token(DBM clocks_v, vector<pair<bool,unordered_set<string>>> & memory):
        initial_values(clocks_v), current_values(clocks_v),
        final_values(clocks_v), variables(memory){}
Token::Token(Token const& token) = default;
Token::~Token() = default;
void Token::increment(double time_elapse){
  final_values.increment(time_elapse);
}

bool Token::operator<=(Token const& tok2) const{
  if(!(this->final_values <= tok2.final_values)){
    return false;
  }
  if(!(this->current_values.project(this->final_values)<=tok2.current_values.project(tok2.final_values))) {
    return false;
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
    if(this->variables[i].second != tok2.variables[i].second){
      return false;
    }
  }
  return true;
}

string Token::to_string() const{
  stringstream sstr;
  sstr <<  "token :\n";
  for(vector<Bound> const& line : current_values.matrice){
    for(Bound const& bound : line){
      sstr << "(";
      sstr <<  (bound.value==numeric_limits<double>::max()? "\\inf":std::to_string(bound.value));
      sstr <<  " , " << bound.inclusion << ") ";
    }
    sstr << "\n";
  }
  for(unsigned int i =0; i<variables.size(); i++){
    sstr <<  " "<< i << " : {";
    for(string str : variables[i].second){
      sstr <<  str << "; ";
    }
    sstr <<  "}";
  }
  sstr <<  "\n";
  return sstr.str();
}
//---------------------------End of class Token-------------------------------//

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
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
      new_map_tokens[evaluate_state].remove_if([evaluate_token](Token k){return k <= evaluate_token;});
      new_map_tokens[evaluate_state].push_back(evaluate_token);
      for(Transition * trans : modele->transitions[evaluate_state]){
        if (incremented){
          pair<vector<DBM>, vector<pair<bool,unordered_set<string>>>> new_token_value;
          new_token_value = trans->accept_epsilon(evaluate_token.initial_values,
                                                  evaluate_token.current_values,
                                                  evaluate_token.final_values,
                                                  evaluate_token.variables);
          if(!(new_token_value.first.empty() && new_token_value.second.empty())){
            to_evaluate.push_back(pair<State*,Token>(trans->destination,
                          Token(new_token_value.first[0], new_token_value.first[1],
                                new_token_value.first[2], new_token_value.second)));
          }
        }else{
          token_content new_token_value = trans->accept_epsilon(evaluate_token.current_values, evaluate_token.variables);
          if(!(new_token_value.first.empty() && new_token_value.second.empty())){
            to_evaluate.push_back(pair<State*,Token>(trans->destination,Token(new_token_value.first, new_token_value.second)));
          }
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
        DBM accepted_clocks = tokens.first->accept(token.final_values);
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
  bool isConstant = modele->alphabet.find(event) != modele->alphabet.end();

  //Second step : make the tokens consume the event.
  map<State*,list<Token>> new_map_tokens;
  for(State & state : modele->states){
    for(Token const& token : map_tokens[&state]){
      for(Transition * transition : modele->transitions[&state]){
        token_content new_token_value;
        if(isConstant){
          new_token_value = transition->accept_constant(token.current_values,token.variables,event);
        }else{
          new_token_value = transition->accept_event(token.current_values, token.variables,event);
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
  cout << "Clocks name and line number : \n";
  for(Clock* const& clk : modele->clocks){
    cout << "\t";
    clk->print();
    cout << "\n";
  }
  for(State& state : modele->states){
    cout << state.id << " : " << this->map_tokens[&state].size() << "\n";
    for(auto token : this->map_tokens[&state]){
      cout << "\t";
      cout << "token :\n";
      for(vector<Bound> const& line : token.current_values.matrice){
        cout << "\t\t";
        for(Bound const& bound : line){
          cout << "(";
          cout << (bound.value==numeric_limits<double>::max()? "\\inf":to_string(bound.value));
          cout << " , " << bound.inclusion << ") ";
        }
        cout <<"\n";
      }
      cout << "\t\t";
      for(unsigned int i =0; i<token.variables.size(); i++){
        cout << " "<< i << " : {";
        for(string str : token.variables[i].second){
          cout <<str << "; ";
        }
        cout << "}";
      }
      cout << "\n";
    }
  }
  cout << "\n";

}
