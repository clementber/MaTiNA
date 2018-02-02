#include <string>
#include <iostream>
#include "modelRecognizer.hpp"
#include <limits>
#include "../time_automata/grammar.hpp"

using namespace automate;
using namespace std;
using namespace recognizer;

//--------------------------------------------------------------------------//
//---------------------Token Constructor------------------------------------//
//--------------------------------------------------------------------------//
Token::Token(Automate* automate){
  initial_values = DBM(*automate);
  current_values = initial_values;
  final_values = initial_values;
}
Token::Token(DBM initial_v, DBM current_v, DBM final_v):
    initial_values(initial_v), current_values(current_v), final_values(final_v){}
Token::Token(Token const& token) = default;
Token::~Token() = default;

//--------------------------------------------------------------------------//
//--------------------------------Methods-----------------------------------//
//--------------------------------------------------------------------------//
void Token::increment(double time_elapse){
  final_values.increment(time_elapse);
}

void Token::apply_increment(){
  initial_values = final_values;
  current_values = final_values;
}


bool Token::operator<=(Token const& tok2){
  if(!(this->final_values <= tok2.final_values)){
    return false;
  }
  if(!(this->current_values.project(this->final_values)<=tok2.current_values.project(tok2.final_values))) {
    return false;
  }
  return true;
}

//---------------------------End of class Token-------------------------------//

Checker::Checker(Automate* modele_automate) : modele(modele_automate) {
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

void Checker::input(double time_elapse){
  map<State*,list<Token>> new_map_tokens;
  list<pair<State*,Token>> to_evaluate;
  //First step : prepare the tokens
  for(auto tokens : this->map_tokens){
    for(Token token : tokens.second){
      token.increment(time_elapse);
      to_evaluate.push_back(pair<State*,Token>(tokens.first,token));
    }
  }
  //Optional : Put a new initial token when there is no more.
  /*bool present=false;
  if(time_elapse == 0){
    for(auto const& tok :this->map_tokens[modele->start]){
      bool zero = true;
      for(Clock const& clk : tok.clocks_values){
        if(clk.value.borne_inf != 0 || clk.value.include_inf != 0){
          zero = false;
          break;
        }
      }
      if(zero){
        present = true;
        break;
      }
    }
  }
  if(!present){
    to_evaluate.push_back(pair<State*,Token>(modele->start, Token(modele)));
  }*/
  //Second step: move new token as long as they can move.
  while(!to_evaluate.empty()){
    State* state = to_evaluate.front().first;
    Token token = to_evaluate.front().second;
    to_evaluate.pop_front();
    if(!Token::included(token,new_map_tokens[state])){
      new_map_tokens[state].remove_if([token](Token k){return k <= token;});
      new_map_tokens[state].push_back(token);
      for(auto transition : modele->transitions[state]){
        vector<DBM> outgoing_3dbm = transition.accept(token.initial_values,
                                                      token.current_values,
                                                      token.final_values);
        if(!outgoing_3dbm.empty()){
          Token new_token(outgoing_3dbm[0], outgoing_3dbm[1],outgoing_3dbm[2]);
          to_evaluate.push_back(pair<State*,Token>(transition.destination,new_token));
        }
      }
    }
  }
  //Final step: stabilize the tokens values.
  map<State*,list<Token>> final_map_tokens;
  for(auto tokens : new_map_tokens){
    for(auto token : tokens.second){
      token.final_values = tokens.first->accept(token.final_values);
      if(!token.final_values.empty()){
        token.apply_increment();
        final_map_tokens[tokens.first].push_back(token);
      }
    }
  }
  this->map_tokens = final_map_tokens;
}

void Checker::input(string event){
  map<State*,list<Token>> new_map_tokens;
  for(State & state : modele->states){
    for(Token & token : map_tokens[&state]){
      //TODO opti : ne pas traiter les transition qui ne peuvent pas accepter l'event
      for(Transition & transition : modele->transitions[&state]){
        DBM accepted_values=transition.accept(event,token.final_values);
        if(!accepted_values.empty()){
          Token new_token(accepted_values,accepted_values,accepted_values);
          State* dest = transition.destination;
          new_map_tokens[dest].push_back(new_token);
        }
      }
    }
  }

  this->map_tokens = new_map_tokens;
  input(0);
}

void Checker::print_state(string str){
  if(str == ""){
    cout << "----------------current state---------------\n";
  }else{
    cout << "----------------current state : "<<str<<"-----------\n";
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
      cout <<"\n";
    }
  }
}
