#include "ast.hpp"

using namespace std;
using namespace automate;
using namespace tre_ast;


AST_node::AST_node(){}

Automate * AST_node::convert(){
  vector<Clock*> clks = vector<Clocks*>(number_clocks,nullptr);
  int cpt_state = 0;
  Automate * res = convert(clks, cpt_state);
  res->clocks = clks;
  return res;
}

AST_OR::AST_OR(AST_node * n1, AST_node * n1):AST_node(),case1(n1), case2(n2){}

AST_OR::~AST_OR(){
  delete case1;
  delete case2;
}

Automate * AST_OR::convert(vector<Clock*> & clocks, int & cpt_state){
  Automate * autom1 = case1.convert(clocks, cpt_state);
  Automate * autom2 = case2.convert(clocks, cpt_state);

  autom1->constants.insert(autom2->constants.begin(), autom2->constants.end());

  map<State*, State*> dictionnary;

  for(State & oldState : autom2->states){
    autom1->states.push_back(oldState);
    dictionnary[&oldState]= &*(--autom1->states.end());
  }
  for(State & oldState : autom2->states){
    State * newState = dictionnary[&oldState];
    for(Transition * transition : autom2->transitions[&oldState]){
      Transition * newTransition = transition->copy();
      newTransition->origine = newState;
      newTransition->destination = dictionnary[transition->destination];
      autom1->transitions[newState].push_back(newTransition);
    }
  }
  for(State * oldEndState : autom2->endStates){
    autom1->endStates.push_back(dictionnary[oldEndState]);
  }

  autom1->states.push_back(State("s"+to_string(cpt_state++)));
  State * new_initial_state = &(*(--autom2->states.end()));
  Transition * epsi_trans = new Epsilon_Transition(new_initial_state, autom1->start);
  autom1->transitions[new_initial_state].push_back(epsi_trans);
  autom1->start = new_initial_state;
  epsi_trans = new Epsilon_Transition(autom1->start, dictionnary[autom2->start]);
  autom1->transitions[autom1->start].push_back(epsi_trans);

  delete autom2;
  return autom1;
}

AST_AND::AST_AND(AST_node *n1, AST_node *n1):AST_node(),pattern1(n1),pattern2(n2){}

AST_AND::~AST_AND(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_AND::convert(vector<Clock*> & clocks, int & cpt_state){
  Automate * autom1 = pattern1->convert(clocks, cpt_state);
  //TODO
  //TODO Probleme gestion des horloges. Il ne doit pas y avoir de reset
  //TODO entre les deux patterns!!
  //TODO
  Automate * autom2 = pattern2->convert(clocks, cpt_state);
  Automate *a = new Automate();
  map<State *,map<State*,State*>> dictionnary;

  for(State & oldState1 : $1->states){
    for(State & oldState3 : $3->states){
      a->states.push_back(State(oldState1.id+"-"+oldState3.id));
      State & newState = *(--(a->states.end()));
      newState.clocks_constraints = oldState1.clocks_constraints;
      newState.clocks_constraints.insert(oldState3.clocks_constraints.begin(),oldState3.clocks_constraints.end());
      dictionnary[&oldState1][&oldState3] = &newState;
    }
  }

  for(auto const& element1 : $1->transitions){
    for(Transition * trans1 : element1.second){
      if(trans1->epsilon()){ continue; }
      for(auto const& element3 : $3->transitions){
        for(Transition * trans3 : element3.second){
          if(!trans3->epsilon() && trans1->statique()==trans3->statique()
            && trans1->dynamique()==trans3->dynamique()){
              map<string,Interval> constraints(trans1->clocks_constraints);
              constraints.insert(trans3->clocks_constraints.begin(), trans3->clocks_constraints.end());
              unordered_set<string> reseted(trans1->clocks_to_reset);
              reseted.insert(trans3->clocks_to_reset.begin(),trans3->clocks_to_reset.end());
              Transition * newTransition = trans1->copy();
              newTransition->origine = dictionnary[trans1->origine][trans3->origine];
              newTransition->destination = dictionnary[trans1->destination][trans3->destination];
              newTransition->clocks_constraints = constraints;
              newTransition->clocks_to_reset = reseted;
              a->transitions[dictionnary[trans1->origine][trans3->origine]].push_back(newTransition);
          }
        }
      }
    }
  }

  for(auto const& element1 : $1->transitions){
    for(Transition * trans1 : element1.second){
      if(!trans1->epsilon()){ continue; }
      for(State & state : $3->states){
        Transition * newTransition = trans1->copy();
        newTransition->origine = dictionnary[trans1->origine][&state];
        newTransition->destination = dictionnary[trans1->destination][&state];
        a->transitions[dictionnary[trans1->origine][&state]].push_back(newTransition);
      }
    }
  }

  for(auto const& element3 : $3->transitions){
    for(Transition * trans3 : element3.second){
      if(!trans3->epsilon()){ continue; }
      for(State & state : $1->states){
        Transition * newTransition=trans3->copy();
        newTransition->origine = dictionnary[&state][trans3->origine];
        newTransition->destination = dictionnary[&state][trans3->destination];
        a->transitions[dictionnary[&state][trans3->origine]].push_back(newTransition);
      }
    }
  }

  for(State * endState1 : $1->endStates){
    for(State * endState3 : $3->endStates){
      a->endStates.push_back(dictionnary[endState1][endState3]);
    }
  }

  a->clocks = $1->clocks;
  a->clocks.insert(a->clocks.end(), $3->clocks.begin(), $3->clocks.end());
  a->constants = $1->constants;
  a->constants.insert($3->constants.begin(), $3->constants.end());
  a->start = dictionnary[$1->start][$3->start];
  delete ($1);
  delete ($3);
  $$=a;
}

AST_CONCAT::AST_CONCAT(AST_node* n1, AST_node* n1):AST_node(),begin(n1),end(n2){}

AST_CONCAT::~AST_CONCAT(){
  delete begin;
  delete end;
}

Automate * AST_CONCAT::convert(vector<Clock*> & clocks, int & cpt_state){
  Automate * autom_end = end->convert(clocks,cpt_state);
  Automate * autom_begin = begin->convert(clocks,cpt_state);

  autom_end->constants.insert(autom_begin->constants.begin(), autom_begin->constants.end());

  map<State*, State*> dictionnary;

  for(State & oldState : autom_begin->states){
    bool present = false;
    for(State* endS : autom_begin->endStates){
      if(endS == &oldState){
        present = true;
        break;
      }
    }
    if(!present){
      autom_end->states.push_back(oldState);
      State & new_state = *(--(autom_end->states.end()));
      dictionnary[&oldState]= &new_state;
    }
  }

  for(State* oldEndState : autom_begin->endStates){
    dictionnary[oldEndState] = autom_end->start;
  }

  for(State & oldState : autom_begin->states){
    State * newState = dictionnary[&oldState];
    for(Transition * transition : autom_begin->transitions[&oldState]){
      Transition*  newTransition = transition->copy();
      newTransition->origine = newState;
      newTransition->destination = dictionnary[transition->destination];
      a->transitions[newState].push_back(newTransition);
    }
  }

  autom_end->start = dictionnary[autom_begin->start];
  delete(autom_begin);
  return autom_end;
}

AST_DELAY::AST_DELAY(AST_node* n1, Bound inf, Bound sup):
          AST_node(), pattern(n1), inf(inf), sup(sup){}

AST_DELAY::~AST_DELAY(){
  delete pattern;
}

Automate * AST_DELAY::convert(vector<Clock*> & clocks, int & cpt_state){
  //TODO
}

AST_KSTAR::AST_KSTAR(AST_node * pattern): AST_node(),pattern(pattern){}

AST_KSTAR::~AST_KSTAR(){
  delete pattern;
}

Automate * AST_KSTAR::convert(vector<Clock*> & clocks, int & cpt_state){
  //TODO
}


AST_OPTIONAL::AST_OPTIONAL(AST_node * pattern):AST_node(),pattern(pattern){}

AST_OPTIONAL::~AST_OPTIONAL(){
 delete pattern;
}

Automate * AST_OPTIONAL::convert(vector<Clock*> & clocks, int & cpt_state){
  //TODO
}

AST_PLUS::AST_PLUS(AST_node * pattern):AST_node(),pattern(pattern){}

AST_PLUS::~AST_PLUS(){
 delete pattern;
}

Automate * AST_PLUS::convert(vector<Clock*> & clocks, int & cpt_state){
  //TODO
}

AST_EVENT::AST_EVENT(string event):AST_node(), event(event){}

AST_EVENT::~AST_EVENT()=default;

Automate * AST_EVENT::convert(vector<Clock*> & clocks, int & cpt_state){
  Automate *a = new Automate();
  a->states.push_back(State("s"+to_string(cpt_state++)));
  a->states.push_back(State("s"+to_string(cpt_state++)));
  std::list<State>::iterator ite = a->states.begin();
  a->start = &(*ite);
  ite++;
  a->endStates.push_back(&(*(ite)));
  Transition trans(a->start, a->endStates[0], {event}, DBM(), {});
  a->transitions[a->start].push_back(trans);
  a->alphabet.insert(event);

  return a;
}


AST_SHUFFLE::AST_SHUFFLE(AST_node * pattern1, AST_node * pattern2):
                AST_node(),pattern1(pattern1),pattern2(pattern2){}

AST_SHUFFLE::~AST_SHUFFLE(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_SHUFFLE::convert(vector<Clock*> & clocks, int & cpt_state){
  //TODO
}
