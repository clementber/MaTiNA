#include "parsingDriver.hpp"
#include "yacc.tab.hpp"

regex_driver::regex_driver ()
  : trace_scanning (false), trace_parsing (false)
{
}

regex_driver::~regex_driver ()
{
}

int
regex_driver::parse (automate::Automate** automate, const std::string &f)
{
  file = f;
  scan_begin ();
  tre_ast::AST_node * ast;
  yy::parser parser (& ast, *this);
  parser.set_debug_level (trace_parsing);
  int res = parser.parse ();
  scan_end ();
  if(res == 0){
    *automate = ast->convert();
    delete ast;
  }

  return res;
}

void
regex_driver::error (const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

void
regex_driver::error (const std::string& m)
{
  std::cerr << m << std::endl;
}
