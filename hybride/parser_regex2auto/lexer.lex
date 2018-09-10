/* Basic flex for timed regulare expression */

%{
#include <string>
#include "ast.hpp"
#include "parsingDriver.hpp"
#include "yacc.tab.hpp"

# undef yywrap
# define yywrap() 1
#define yyterminate() yy::parser::make_END(loc);
using namespace yy;

// The location of the current token.
static yy::location loc;
%}
%option nodefault noyywrap

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step ();
%}

[[:blank:]]
"\n"
"#" return (yy::parser::make_FRESH(loc));
"!" return (yy::parser::make_FREE(loc));
\. return(yy::parser::make_DOT(loc));
"<" return (yy::parser::make_LBALISE(loc));
">" return (yy::parser::make_RBALISE(loc));
\+ return (yy::parser::make_PLUS(loc));
\* return (yy::parser::make_STAR(loc));
\? return (yy::parser::make_QUESTION(loc));
"%" return (yy::parser::make_SHUFFLE(loc));
"and"|"&" return (yy::parser::make_AND(loc));
"or"|"|" return (yy::parser::make_OR(loc));
"," return (yy::parser::make_COMMA(loc));
"{" return (yy::parser::make_LACCOLADE(loc));
"}" return (yy::parser::make_RACCOLADE(loc))
"(" return (yy::parser::make_LPARENTHESE(loc));
")" return (yy::parser::make_RPARENTHESE(loc));
"[" return (yy::parser::make_LCROCHET(loc));
"]" return (yy::parser::make_RCROCHET(loc));
[0-9]+(\.[0-9]+)? return yy::parser::make_NUMBER(std::stod(yytext),loc);
[[:alpha:]]([[:alnum:]]|"-"|"_")* {return yy::parser::make_EVENT(yytext,loc);}
<<EOF>> { return yyterminate(); }
. { cout << "Symbol non reconnu : " << yytext <<"\n"; }
%%

void regex_driver::scan_begin ()
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

void regex_driver::scan_end ()
{
  fclose (yyin);
}
