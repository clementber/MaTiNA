#include "parsingDriver.hpp"
#include "yacc.tab.hpp"

autom_driver::autom_driver ()
  : trace_scanning (false), trace_parsing (false)
{
}

autom_driver::~autom_driver ()
{
}

int autom_driver::parse (automate::Automate* automate, const std::string &f)
{
  file = f;
  scan_begin ();
  yy::parser parser (automate,*this);
  parser.set_debug_level (trace_parsing);

  map_variable.clear();
  variable_cpt=0;

  int res = parser.parse ();
  scan_end ();
  if(automate->start == nullptr){
    cerr << "The automata of " << f << "is invalide : No initial state.\n";
  }
  if(automate->endStates.empty()){
    cerr << "The automata of " << f << "is invalide : No final state.\n";
  }
  return res;
}

void
autom_driver::error (const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

void
autom_driver::error (const std::string& m)
{
  std::cerr << m << std::endl;
}
