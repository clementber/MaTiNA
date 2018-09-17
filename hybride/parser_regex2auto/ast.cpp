#include "ast.hpp"
#include <unordered_set>
#include <iostream>

using namespace std;
using namespace automate;
using namespace tnure_ast;


AST_node::AST_node(int number_clocks):number_clocks(number_clocks) {}

Automate * AST_node::convert(){
  vector<Clock*> clks = vector<Clock*>(number_clocks+1,nullptr);
  clks[0] = new Clock("c_epsi", 1);
  int cpt_state = 0;
  int init_clk = 1;
  Automate * res = convert(clks, cpt_state, init_clk);
  res->clocks = clks;
  return res;
}

AST_node::~AST_node()=default;

AST_OR::AST_OR(AST_node * n1, AST_node * n2):
            AST_node(n1->number_clocks+n2->number_clocks),case1(n1), case2(n2){}

AST_OR::~AST_OR(){
  delete case1;
  delete case2;
}

Automate * AST_OR::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate * autom1 = case1->convert(clocks, cpt_state,init_clk);
  Automate * autom2 = case2->convert(clocks, cpt_state,init_clk);

  autom1->alphabet.insert(autom2->alphabet.begin(), autom2->alphabet.end());

  map<State*, State*> dictionnary;

  for(State & oldState : autom2->states){
    autom1->states.push_back(oldState);
    dictionnary[&oldState]= &*(--autom1->states.end());
  }
  for(State & oldState : autom2->states){
    State * newState = dictionnary[&oldState];
    for(Transition* transition : autom2->transitions[&oldState]){
      transition->origine = newState;
      transition->destination = dictionnary[transition->destination];
      autom1->transitions[newState].push_back(transition);
    }
  }
  for(State * oldEndState : autom2->endStates){
    autom1->endStates.push_back(dictionnary[oldEndState]);
  }

  autom1->states.push_back(State("s"+to_string(cpt_state++)));
  State * new_initial_state = &(*(--autom1->states.end()));
  DBM eps_constraint(clocks.size());
  eps_constraint.matrice[1][0] = Bound(0);
  eps_constraint.matrice[0][1] = Bound(0);
  autom1->transitions[new_initial_state].push_back(new
      Epsilon_Transition(new_initial_state, autom1->start, {}, {}, eps_constraint, {}));
  autom1->transitions[new_initial_state].push_back(new
      Epsilon_Transition(new_initial_state, dictionnary[autom2->start], {}, {}, eps_constraint, {}));
  autom1->start = new_initial_state;

  autom2->transitions = map<State*,vector<Transition*>>();
  autom1->start = new_initial_state;
  delete autom2;
  return autom1;
}

AST_CONCAT::AST_CONCAT(AST_node* n1, AST_node* n2):
               AST_node(n1->number_clocks+n2->number_clocks),begin(n1),end(n2){}

AST_CONCAT::~AST_CONCAT(){
  delete begin;
  delete end;
}

Automate * AST_CONCAT::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate * autom_end = end->convert(clocks,cpt_state,init_clk);
  vector<Clock*> to_reset = clocks;
  Automate * autom_begin = begin->convert(clocks,cpt_state,init_clk);

  autom_end->alphabet.insert(autom_begin->alphabet.begin(), autom_begin->alphabet.end());

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
    for(Transition* transition : autom_begin->transitions[&oldState]){
      transition->origine = newState;
      transition->destination = dictionnary[transition->destination];
      if(transition->destination == autom_end->start){
        transition->clocks_to_reset = unordered_set<Clock*>(to_reset.begin(),to_reset.end());
      }
      autom_end->transitions[newState].push_back(transition);
    }
  }

  autom_end->start = dictionnary[autom_begin->start];
  autom_begin->transitions = map<State*,vector<Transition*>>();
  delete autom_begin;
  return autom_end;
}

AST_AND::AST_AND(AST_node *n1, AST_node *n2):
       AST_node(n1->number_clocks+n2->number_clocks),pattern1(n1),pattern2(n2){}

AST_AND::~AST_AND(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_AND::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  /*TODO*/ /*// As both patterns occures simultaneously the clocks of each pattern
  // shall not be reset by the transition of other one.
  int initial_clock_number = init_clk;
  Automate * autom1 = pattern1->convert(clocks, cpt_state,init_clk);
  vector<Clock*> new_clocks;
  for(unsigned int i = initial_clock_number; i< init_clk; i++){
    new_clocks.push_back(clocks[i]);
    clocks[i]=nullptr;
  }
  Automate * autom2 = pattern2->convert(clocks, cpt_state,init_clk);
  for(unsigned int i = 0; i< new_clocks.size(); i++){
    clocks[initial_clock_number+i] = new_clocks[i];
  }
  Automate *a = new Automate();
  map<State *,map<State*,State*>> dictionnary;

  for(State & oldState1 : autom1->states){
      for(State & oldState2 : autom2->states){
      a->states.push_back(State(oldState1.id+"-"+oldState2.id));
      State & newState = *(--(a->states.end()));
      newState.clocks_constraints = oldState1.clocks_constraints.intersect(oldState2.clocks_constraints);
      dictionnary[&oldState1][&oldState2] = &newState;
    }
  }

  for(auto const& element1 : autom1->transitions){
    for(Transition const& trans1 : element1.second){
      if(trans1.epsilon()){ continue; }
      for(auto const& element2 : autom2->transitions){
        for(Transition trans2 : element2.second){
          if(trans2.epsilon()){continue;}
          if(trans1.triggers[0] != trans2.triggers[0]){continue;}
          trans2.origine= dictionnary[trans1.origine][trans2.origine];
          trans2.destination = dictionnary[trans1.destination][trans2.destination];
          trans2.clocks_constraints = trans1.clocks_constraints.intersect(trans2.clocks_constraints);
          trans2.clocks_to_reset.insert(trans1.clocks_to_reset.begin(), trans1.clocks_to_reset.end());
          a->transitions[trans2.origine].push_back(trans2);
        }
      }
    }
  }

  for(auto const& element1 : autom1->transitions){
    for(Transition trans1 : element1.second){
      if(!trans1.epsilon()){ continue; }
      State * t1ori = trans1.origine;
      State * t1dest = trans1.destination;
      for(State & state : autom2->states){
        trans1.origine = dictionnary[t1ori][&state];
        trans1.destination = dictionnary[t1dest][&state];
        a->transitions[trans1.origine].push_back(trans1);
      }
    }
  }

  for(auto const& element2 : autom2->transitions){
    for(Transition trans2 : element2.second){
      if(!trans2.epsilon()){ continue; }
      State * t2ori = trans2.origine;
      State * t2dest = trans2.destination;
      for(State & state : autom1->states){
        trans2.origine = dictionnary[&state][t2ori];
        trans2.destination = dictionnary[&state][t2dest];
        a->transitions[trans2.origine].push_back(trans2);
      }
    }
  }

  for(State * endState1 : autom1->endStates){
    for(State * endState3 : autom2->endStates){
      a->endStates.push_back(dictionnary[endState1][endState3]);
    }
  }
  a->alphabet = autom1->alphabet;
  a->alphabet.insert(autom2->alphabet.begin(), autom2->alphabet.end());
  a->start = dictionnary[autom1->start][autom2->start];
  delete autom1;
  delete autom2;
  return a;*/
  return pattern1->convert(clocks, cpt_state,init_clk);
}

AST_DELAY::AST_DELAY(AST_node* n1, Bound inf, Bound sup):
          AST_node(n1->number_clocks+1), pattern(n1), inf(inf), sup(sup){}

AST_DELAY::~AST_DELAY(){
  delete pattern;
}

Automate * AST_DELAY::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate *a = pattern->convert(clocks,cpt_state,init_clk);
  clocks [init_clk] = new Clock("c"+init_clk, init_clk+1);
  init_clk++;
  for(State & state : a->states){
    if(state.clocks_constraints.getClocks_number() == 0){
      state.clocks_constraints = DBM(clocks.size());
    }
    state.clocks_constraints.matrice[init_clk][0]=sup;
    for(Transition * trans : a->transitions[&state]){
      if(trans->clocks_constraints.getClocks_number() == 0){
        trans->clocks_constraints = DBM(clocks.size());
      }
      trans->clocks_constraints.matrice[init_clk][0]=sup;
      for(State * endState : a->endStates){
        if(endState == trans->destination){
          trans->clocks_constraints.matrice[0][init_clk]=inf;
          break;
        }
      }
    }
  }
  for(State * endState : a->endStates){
    endState->clocks_constraints.matrice[init_clk][0]=Bound();
  }
  return a;
}

AST_KSTAR::AST_KSTAR(AST_node * pattern): AST_node(pattern->number_clocks),
                                          pattern(pattern){}

AST_KSTAR::~AST_KSTAR(){
  delete pattern;
}

Automate * AST_KSTAR::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate *a = pattern->convert(clocks, cpt_state, init_clk);

  for(pair<State *,vector<Transition*>> element : a->transitions){
    for(Transition * trans : element.second){
      for(State * endState : a->endStates){
        if(endState == trans->destination){
          Transition * new_trans = trans->clone();
          new_trans->destination = a->start;
          for(unsigned int i = 1; i < clocks.size(); i++){
            if(clocks[i]!= nullptr){
              new_trans->clocks_to_reset.insert(clocks[i]);
            }
          }
          if(trans->triggerable()){
            new_trans->clocks_to_reset.insert(clocks[0]);
          }
          a->transitions[element.first].push_back(new_trans);
          break;
        }
      }
    }
  }

  State * oldStart = a->start;
  a->states.push_back(State("s"+to_string(cpt_state++)));
  a->states.push_back(State("s"+to_string(cpt_state++)));
  list<State>::iterator it = a->states.end();
  it--;
  a->start = &(*(it));
  it--;
  State * new_end = &(*(it));
  Transition * epsi_trans = new Epsilon_Transition(a->start, oldStart);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0]=Bound(0);
  a->transitions[a->start].push_back(epsi_trans);
  epsi_trans = new Epsilon_Transition(a->start, new_end);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0]=Bound(0);
  a->transitions[a->start].push_back(epsi_trans);
  a->endStates.push_back(new_end);
  return a;
}


AST_OPTIONAL::AST_OPTIONAL(AST_node *pattern):AST_node(pattern->number_clocks),
                                              pattern(pattern){}

AST_OPTIONAL::~AST_OPTIONAL(){
 delete pattern;
}

Automate * AST_OPTIONAL::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate *a = pattern->convert(clocks, cpt_state, init_clk);
  State * oldStart = a->start;
  a->states.push_back(State("s"+to_string(cpt_state++)));
  a->states.push_back(State("s"+to_string(cpt_state++)));
  list<State>::iterator it = a->states.end();
  it--;
  a->start = &(*(it));
  it--;
  State * new_end = &(*(it));
  Transition * epsi_trans = new Epsilon_Transition(a->start, oldStart);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0]=Bound(0);
  a->transitions[a->start].push_back(epsi_trans);
  epsi_trans = new Epsilon_Transition(a->start, new_end);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0]=Bound(0);
  a->transitions[a->start].push_back(epsi_trans);
  a->endStates.push_back(new_end);
  return a;
}

AST_PLUS::AST_PLUS(AST_node * pattern):AST_node(pattern->number_clocks),
                                       pattern(pattern){}

AST_PLUS::~AST_PLUS(){
 delete pattern;
}

Automate * AST_PLUS::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate *a = pattern->convert(clocks,cpt_state,init_clk);
  for(pair<State *,vector<Transition*>> element : a->transitions){
    for(Transition * trans : element.second){
      for(State * endState : a->endStates){
        if(endState == trans->destination){
          Transition * new_trans = trans->clone();
          new_trans->destination = a->start;
          for(unsigned int i = 1; i < clocks.size(); i++){
            if(clocks[i]!= nullptr){
              new_trans->clocks_to_reset.insert(clocks[i]);
            }
          }
          if(trans->triggerable()){
            new_trans->clocks_to_reset.insert(clocks[0]);
          }
          a->transitions[element.first].push_back(new_trans);
          break;
        }
      }
    }
  }
  return a;
}

AST_SHUFFLE::AST_SHUFFLE(AST_node * pattern1, AST_node * pattern2):
                AST_node(pattern1->number_clocks+pattern2->number_clocks),
                pattern1(pattern1),pattern2(pattern2){}

AST_SHUFFLE::~AST_SHUFFLE(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_SHUFFLE::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  // As both patterns occures simultaneously the clocks of each pattern
  // shall not be reset by the transition of other one.
  int initial_clock_number = init_clk;
  Automate * autom1 = pattern1->convert(clocks, cpt_state,init_clk);
  vector<Clock*> new_clocks;
  for(int i = initial_clock_number; i< init_clk; i++){
    new_clocks.push_back(clocks[i]);
    clocks[i]=nullptr;
  }
  Automate * autom2 = pattern2->convert(clocks, cpt_state,init_clk);
  for(unsigned int i = 0; i< new_clocks.size(); i++){
    clocks[initial_clock_number+i] = new_clocks[i];
  }
  Automate *a = new Automate();
  map<State *,map<State*,State*>> dictionnary;

  for(State & oldState1 : autom1->states){
    for(State & oldState2 : autom2->states){
      a->states.push_back(State(oldState1.id+""+oldState2.id));
      State & newState = *(--(a->states.end()));
      newState.clocks_constraints = oldState1.clocks_constraints.intersect(oldState2.clocks_constraints);
      dictionnary[&oldState1][&oldState2] = &newState;
    }
  }

  for(auto const& element1 : autom1->transitions){
    for(Transition *trans1 : element1.second){
      State * t1ori = trans1->origine;
      State * t1dest = trans1->destination;
      for(State & state2 : autom2->states){
        Transition * new_trans = trans1->clone();
        new_trans->origine = dictionnary[t1ori][&state2];
        new_trans->destination=dictionnary[t1dest][&state2];
        a->transitions[new_trans->origine].push_back(new_trans);
      }
    }
  }

  for(auto const& element2 : autom2->transitions){
    for(Transition * trans2 : element2.second){
      State * t2ori = trans2->origine;
      State * t2dest = trans2->destination;
      for(State & state1 : autom1->states){
        Transition * new_trans = trans2->clone();
        new_trans->origine = dictionnary[&state1][t2ori];
        new_trans->destination = dictionnary[&state1][t2dest];
        a->transitions[trans2->origine].push_back(new_trans);
      }
    }
  }

  for(State * endState1 : autom1->endStates){
    for(State * endState2 : autom2->endStates){
      a->endStates.push_back(dictionnary[endState1][endState2]);
    }
  }

  a->alphabet = autom1->alphabet;
  a->alphabet.insert(autom2->alphabet.begin(), autom2->alphabet.end());
  a->start = dictionnary[autom1->start][autom2->start];
  delete autom1;
  delete autom2;
  return a;
}

AST_ALLOCS::AST_ALLOCS(AST_node *pattern, vector<int> vars)
                      :AST_node(pattern->number_clocks), vars(vars){}

AST_ALLOCS::~AST_ALLOCS(){delete pattern;}

Automate * AST_ALLOCS::convert(vector<Clock*> & clocks, int& cpt_state, int& init_clk){
  Automate *a = pattern->convert(clocks,cpt_state,init_clk);
  a->states.push_back(State("s"+to_string(cpt_state++)));
  std::list<State>::iterator ite = a->states.end();
  ite--;
  Transition * epsi_trans = new Epsilon_Transition(&(*ite), a->start);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0] = Bound(0);
  epsi_trans->allocations = vars;
  a->transitions[&(*ite)].push_back(epsi_trans);
  a->start = &(*(ite));
  return a;
}

AST_FREES::AST_FREES(AST_node *pattern, vector<int> vars)
                    :AST_node(pattern->number_clocks), vars(vars) {}

AST_FREES::~AST_FREES(){delete pattern;}

Automate * AST_FREES::convert(vector<Clock*> & clocks, int& cpt_state, int& init_clk){
  Automate *a = pattern->convert(clocks,cpt_state,init_clk);
  a->states.push_back(State("s"+to_string(cpt_state++)));
  std::list<State>::iterator ite = a->states.end();
  ite--;
  Transition * epsi_trans = new Epsilon_Transition(&(*ite), a->start);
  epsi_trans->clocks_constraints = DBM(clocks.size());
  epsi_trans->clocks_constraints.matrice[1][0] = Bound(0);
  epsi_trans->frees = vars;
  a->transitions[&(*ite)].push_back(epsi_trans);
  a->start = &(*(ite));
  return a;
}

AST_CONST::AST_CONST(string event):AST_node(0), event(event){}

AST_CONST::~AST_CONST()=default;

Automate * AST_CONST::convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk){
  Automate *a = new Automate();
  a->states.push_back(State("s"+to_string(cpt_state++)));
  a->states.push_back(State("s"+to_string(cpt_state++)));
  std::list<State>::iterator ite = a->states.begin();
  a->start = &(*ite);
  ite++;
  a->endStates.push_back(&(*(ite)));
  Transition * trans = new Constant_Transition(a->start, a->endStates[0], {}, {}, DBM(), {clocks[0]}, event);
  a->transitions[a->start].push_back(trans);
  a->alphabet.insert(event);

  return a;
}

AST_USE::AST_USE(int var, bool fresh, bool free):AST_node(0),var(var),
                                                 fresh(fresh), free(free) {}

AST_USE::~AST_USE()=default;

Automate * AST_USE::convert(vector<Clock*> & clocks, int& cpt_state, int & init_clk){
  Automate *a = new Automate();
  a->states.push_back(State("s"+to_string(cpt_state++)));
  a->states.push_back(State("s"+to_string(cpt_state++)));
  std::list<State>::iterator ite = a->states.begin();
  a->start = &(*ite);
  ite++;
  a->endStates.push_back(&(*(ite)));
  Transition * trans = new Event_Transition(a->start, a->endStates[0], {}, {}, DBM(), {clocks[0]}, var);
  if(fresh){
    trans->allocations.push_back(var);
  }
  if(free){
    trans->frees.push_back(var);
  }
  a->transitions[a->start].push_back(trans);

  return a;
}
