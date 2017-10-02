#ifndef AUTOM_DRIVER_HH
# define AUTOM_DRIVER_HH
# include <string>
# include "../fm_automata/grammar.hpp"
# include "yacc.tab.hpp"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::parser::symbol_type yylex (autom_driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing of Calc++.
class autom_driver
{
public:
  autom_driver ();
  virtual ~autom_driver ();

  std::map<std::string, int> variables;

  int result;


  // Handling the scanner.
  void scan_begin ();
  void scan_end ();
  bool trace_scanning;

  // Run the parser on file F.
    // Return 0 on success.
    int parse (automate::Automate*automate, const std::string& f);
    // The name of the file being parsed.
    // Used later to pass the file name to the location tracker.
    std::string file;
    // Whether parser traces should be generated.
    bool trace_parsing;

    // Error handling.
 void error (const yy::location& l, const std::string& m);
 void error (const std::string& m);
};
#endif
