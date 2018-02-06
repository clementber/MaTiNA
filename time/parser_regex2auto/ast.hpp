#ifndef AST_TIMEREGEXP

#define AST_TIMEREGEXP

#include "../time_automata/grammar.hpp"
#include <string>

using namespace automate;
using namespace std;

namespace tre_ast{

  class AST_node{
  public:
    AST_node();
    Automate * convert();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state)=0;
  };

  class AST_OR : AST_node{
  public:
    AST_node *case1, *case2;

    AST_OR(AST_node * n1, AST_node * n1);
    ~AST_OR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_AND : AST_node{
  public:
    AST_node *pattern1, *pattern2;
    AST_AND(AST_node * n1, AST_node * n1);
    ~AST_AND();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_CONCAT : AST_node{
  public:
    AST_node *begin, *end;
    AST_CONCAT(AST_node *n1, AST_node *n1);
    ~AST_CONCAT();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_DELAY : AST_node{
  public:
    AST_node * pattern;
    Bound inf, sup;

    AST_DELAY(AST_node * n1, Bound inf, Bound sup);
    ~AST_DELAY();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_KSTAR : AST_node{
  public:
    AST_node * pattern;
    AST_KSTAR(AST_node * pattern);
    ~AST_KSTAR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  // Zero or one occurence
  class AST_OPTIONAL : AST_node{
  public:
    AST_node * pattern;
    AST_OPTIONAL(AST_node * pattern);
    ~AST_OPTIONAL();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_PLUS : AST_node{
  public:
    AST_node *pattern;
    AST_PLUS(AST_node *pattern);
    ~AST_PLUS();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_EVENT : AST_node{
  public:
    string event;
    AST_EVENT(string event);
    ~AST_EVENT();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };

  class AST_SHUFFLE : AST_node{
  public:
    AST_node *pattern1, *pattern2;
    AST_SHUFFLE(AST_node *pattern1, AST_node *pattern2);
    ~AST_SHUFFLE();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state);
  };
};

#endif
