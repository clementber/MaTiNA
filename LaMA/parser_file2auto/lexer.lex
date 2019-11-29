/* Basic flex for timed regulare expression */

%{
#include <string>
#include "parsingDriver.hpp"
#include "yacc.tab.hpp"

# undef yywrap
# define yywrap() 1
#define yyterminate() yy::parser::make_END(loc);
using namespace yy;

// The location of the current token.
static yy::location loc;
%}
%option noyywrap

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

word [[:alpha:]]([[:alnum:]]|"-"|"_")*

%%

%{
  // Code run each time yylex is called.
  loc.step ();
%}

\n* return (yy::parser::make_NLINE(loc));
[ \t]
"//" return (yy::parser::make_DOUBLE_SLASH(loc));
"{" return (yy::parser::make_LACOLADE(loc));
"}" return (yy::parser::make_RACOLADE(loc));
"#S" return (yy::parser::make_START_NODE(loc));
"#E" return (yy::parser::make_END_NODE(loc));
"#N" return (yy::parser::make_NONE(loc));
"#A" return (yy::parser::make_ALL(loc));
"alloc:" return (yy::parser::make_ALLOC(loc));
"free:" return (yy::parser::make_FREE(loc));
"reset:" return (yy::parser::make_RESET(loc));
[0-9]+ return yy::parser::make_NUMBER(std::stod(yytext),loc);
{word} return yy::parser::make_NAME(yytext,loc);
<<EOF>> { return yyterminate(); }
%%

void autom_driver::scan_begin ()
{
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      error ("cannot open " + file + ": " + strerror(errno));
      exit (EXIT_FAILURE);
    }
}

void autom_driver::scan_end ()
{
  fclose (yyin);
}
