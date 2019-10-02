#ifndef AST_TIMEREGEXP

#define AST_TIMEREGEXP

#include "../hybride_automata/grammar.hpp"
#include <string>
#include <vector>

using namespace automate;
using namespace std;

namespace tnure_ast{

  class AST_node{
  public:
    const unsigned int number_clocks;

    AST_node(int number_clocks);
    virtual Automate * convert();
    virtual Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk)=0;
    virtual ~AST_node();
  };

  class AST_operator : public AST_node{
  public:
    AST_operator(int number_clocks);
    virtual Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk)=0;
    virtual ~AST_operator();
  };

  class AST_OR : public AST_operator{
  public:
    AST_node *case1, *case2;

    AST_OR(AST_node * n1, AST_node * n2);
    ~AST_OR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_CONCAT : public AST_operator{
  public:
    AST_node *begin, *end;

    AST_CONCAT(AST_node *n1, AST_node *n2);
    ~AST_CONCAT();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_AND : public AST_operator{
  public:
    AST_node *pattern1, *pattern2;

    AST_AND(AST_node * n1, AST_node * n2);
    ~AST_AND();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_DELAY : public AST_operator{
  public:
    AST_node * pattern;
    Bound inf, sup;

    AST_DELAY(AST_node * n1, Bound inf, Bound sup);
    ~AST_DELAY();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_KSTAR : public AST_operator{
  public:
    AST_node * pattern;

    AST_KSTAR(AST_node * pattern);
    ~AST_KSTAR();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  // Zero or one occurence
  class AST_OPTIONAL : public AST_operator{
  public:
    AST_node * pattern;

    AST_OPTIONAL(AST_node * pattern);
    ~AST_OPTIONAL();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  //One ore more occurence
  class AST_PLUS : public AST_operator{
  public:
    AST_node *pattern;

    AST_PLUS(AST_node *pattern);
    ~AST_PLUS();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  //Zero or more occurence
  class AST_SHUFFLE : public AST_operator{
  public:
    AST_node *pattern1, *pattern2;

    AST_SHUFFLE(AST_node *pattern1, AST_node *pattern2);
    ~AST_SHUFFLE();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_ALLOCS : public AST_operator{
  public:
    AST_node *pattern;
    vector<int> vars;

    AST_ALLOCS(AST_node *pattern, vector<int> vars);
    ~AST_ALLOCS();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_FREES : public AST_operator{
  public:
    AST_node *pattern;
    vector<int> vars;

    AST_FREES(AST_node *pattern, vector<int> vars);
    ~AST_FREES();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_terminals : public AST_node{
  public:
    AST_terminals();
    virtual Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk)=0;
    virtual ~AST_terminals();
  };

  class AST_CONST : public AST_terminals{
  public:
    string event;

    AST_CONST(string event);
    ~AST_CONST();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };
  
  class AST_LINK : public AST_terminals{
  public:
    AST_terminals *pattern1, *pattern2;

    AST_LINK(AST_terminals *pattern1, AST_terminals *pattern2);
    ~AST_LINK();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };

  class AST_USE : public AST_terminals{
  public:
    int var;
    bool fresh, free;

    AST_USE(int var, bool fresh, bool free);
    ~AST_USE();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };
  
  class AST_ALPHA : public AST_terminals{
  public:
    AST_ALPHA();
    ~AST_ALPHA();
    Automate * convert(vector<Clock*> & clocks, int & cpt_state, int & init_clk);
  };
};
#endif
