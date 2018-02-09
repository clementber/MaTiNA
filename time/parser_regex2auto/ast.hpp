#ifndef AST_TIMEREGEXP

#define AST_TIMEREGEXP

#include "../time_automata/grammar.hpp"
#include <string>
#include <vector>

using namespace automate;
using namespace std;

namespace tre_ast{

  class AST_node{
  public:
    const unsigned int number_clocks;
    AST_node(int number_clocks);
    virtual Automate * convert();
    virtual Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk)=0;
    virtual ~AST_node();
  };

  class AST_OR : public AST_node{
  public:
    AST_node *case1, *case2;

    AST_OR(AST_node * n1, AST_node * n2);
    ~AST_OR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_AND : public AST_node{
  public:
    AST_node *pattern1, *pattern2;
    AST_AND(AST_node * n1, AST_node * n2);
    ~AST_AND();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_CONCAT : public AST_node{
  public:
    AST_node *begin, *end;
    AST_CONCAT(AST_node *n1, AST_node *n2);
    ~AST_CONCAT();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_DELAY : public AST_node{
  public:
    AST_node * pattern;
    Bound inf, sup;

    AST_DELAY(AST_node * n1, Bound inf, Bound sup);
    ~AST_DELAY();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_KSTAR : public AST_node{
  public:
    AST_node * pattern;
    AST_KSTAR(AST_node * pattern);
    ~AST_KSTAR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  // Zero or one occurence
  class AST_OPTIONAL : public AST_node{
  public:
    AST_node * pattern;
    AST_OPTIONAL(AST_node * pattern);
    ~AST_OPTIONAL();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_PLUS : public AST_node{
  public:
    AST_node *pattern;
    AST_PLUS(AST_node *pattern);
    ~AST_PLUS();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_EVENT : public AST_node{
  public:
    string event;
    AST_EVENT(string event);
    ~AST_EVENT();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_SHUFFLE : public AST_node{
  public:
    AST_node *pattern1, *pattern2;
    AST_SHUFFLE(AST_node *pattern1, AST_node *pattern2);
    ~AST_SHUFFLE();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };
};

#endif
