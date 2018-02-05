#include "ast.hpp"

using namespace std;
using namespace automate;
using namespace tre_ast;


AST_node::AST_node(int number_clocks):number_clocks(number_clocks){}

AST_node::convert(){
  vector<Clock*> clks = vector<Clocks*>(number_clocks);
  
}

AST_OR::AST_OR(AST_node * n1, AST_node * n1):
          AST_node(n1->number_clocks+n2->number_clocks),
          case1(n1), case2(n2){}

AST_OR::~AST_OR(){
  delete case1;
  delete case2;
}

Automate * AST_OR::convert(vector<Clock*> clocks){
  //TODO
}


AST_node pattern1, pattern2;
AST_AND::AST_AND(AST_node *n1, AST_node *n1):
            AST_node(n1->number_clocks+n2->number_clocks),
            pattern1(n1),pattern2(n2){}

AST_AND::~AST_AND(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_AND::convert(vector<Clock*> clocks){
  //TODO
}

AST_CONCAT::AST_CONCAT(AST_node* n1, AST_node* n1):
            AST_node(n1->number_clocks+n2->number_clocks),
            begin(n1),end(n2){}

AST_CONCAT::~AST_CONCAT(){
  delete begin;
  delete end;
}

Automate * AST_CONCAT::convert(vector<Clock*> clocks){
  //TODO
}

AST_DELAY::AST_DELAY(AST_node* n1, Bound inf, Bound sup):
          AST_node(n1->clocks_number+1), pattern(n1), inf(inf), sup(sup){}

AST_DELAY::~AST_DELAY(){
  delete pattern;
}

Automate * AST_DELAY::convert(vector<Clock*> clocks){
  //TODO
}

AST_KSTAR::AST_KSTAR(AST_node * pattern): AST_node(pattern->number_clocks),
                                          pattern(pattern){}

AST_KSTAR::~AST_KSTAR(){
  delete pattern;
}

Automate * AST_KSTAR::convert(vector<Clock*> clocks){
  //TODO
}


AST_OPTIONAL::AST_OPTIONAL(AST_node * pattern):AST_node(pattern->number_clocks),
                                 pattern(pattern){}

AST_OPTIONAL::~AST_OPTIONAL(){
 delete pattern;
}

Automate * AST_OPTIONAL::convert(vector<Clock*> clocks){
  //TODO
}

AST_PLUS::AST_PLUS(AST_node * pattern):AST_node(pattern->number_clocks),
                                       pattern(pattern){}

AST_PLUS::~AST_PLUS(){
 delete pattern;
}

Automate * AST_PLUS::convert(vector<Clock*> clocks){
  //TODO
}

AST_EVENT::AST_EVENT(string event):AST_node(0), event(event){}

AST_EVENT::~AST_EVENT(vector<Clock*> clocks)=default;
Automate * AST_EVENT::convert(){
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
                AST_node(pattern1->number_clocks+pattern2->number_clocks),
                pattern1(pattern1),pattern2(pattern2){}

AST_SHUFFLE::~AST_SHUFFLE(){
  delete pattern1;
  delete pattern2;
}

Automate * AST_SHUFFLE::convert(vector<Clock*> clocks){
  //TODO
}
