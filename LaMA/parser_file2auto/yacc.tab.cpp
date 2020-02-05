// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.
#line 1 "yacc.ypp" // lalr1.cc:404

  #include <string>
  #include <map>
  #include <tuple>
  #include "../lama_automata/grammar.hpp"
  #include "yacc.tab.hpp"
  using namespace automate;

  yy::parser::symbol_type yylex (yy::parser::semantic_type *arg);
  unsigned int nb_vars(0);
  //int variable_to_int(string variable_name, autom_driver& driver);

#line 49 "yacc.tab.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "yacc.tab.hpp"

// User implementation prologue.

#line 63 "yacc.tab.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 39 "yacc.ypp" // lalr1.cc:413

# include "parsingDriver.hpp"

#line 69 "yacc.tab.cpp" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 155 "yacc.tab.cpp" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  parser::parser (automate::Automate* generate_automate_yyarg, autom_driver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      generate_automate (generate_automate_yyarg),
      driver (driver_yyarg)
  {}

  parser::~parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  parser::symbol_number_type
  parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 19: // autom
        value.move< automate::Automate* > (that.value);
        break;

      case 4: // NUMBER
        value.move< int > (that.value);
        break;

      case 29: // var
        value.move< std::pair<int,int> > (that.value);
        break;

      case 30: // actions
        value.move< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > (that.value);
        break;

      case 3: // NAME
        value.move< std::string > (that.value);
        break;

      case 28: // vars
      case 31: // allocs
      case 32: // frees
        value.move< std::vector<automate::Variable> > (that.value);
        break;

      case 26: // names
      case 27: // list_name
        value.move< std::vector<std::string> > (that.value);
        break;

      case 21: // layer
        value.move< std::vector<std::vector<std::string>> > (that.value);
        break;

      case 20: // init_alloc
        value.move< std::vector<std::vector<std::vector<std::string>>> > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 19: // autom
        value.copy< automate::Automate* > (that.value);
        break;

      case 4: // NUMBER
        value.copy< int > (that.value);
        break;

      case 29: // var
        value.copy< std::pair<int,int> > (that.value);
        break;

      case 30: // actions
        value.copy< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > (that.value);
        break;

      case 3: // NAME
        value.copy< std::string > (that.value);
        break;

      case 28: // vars
      case 31: // allocs
      case 32: // frees
        value.copy< std::vector<automate::Variable> > (that.value);
        break;

      case 26: // names
      case 27: // list_name
        value.copy< std::vector<std::string> > (that.value);
        break;

      case 21: // layer
        value.copy< std::vector<std::vector<std::string>> > (that.value);
        break;

      case 20: // init_alloc
        value.copy< std::vector<std::vector<std::vector<std::string>>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 33 "yacc.ypp" // lalr1.cc:741
{
  // Initialize the initial location.
  yyla.location.begin.filename = yyla.location.end.filename = &driver.file;
  nb_vars = 0;
}

#line 509 "yacc.tab.cpp" // lalr1.cc:741

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (driver));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 19: // autom
        yylhs.value.build< automate::Automate* > ();
        break;

      case 4: // NUMBER
        yylhs.value.build< int > ();
        break;

      case 29: // var
        yylhs.value.build< std::pair<int,int> > ();
        break;

      case 30: // actions
        yylhs.value.build< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ();
        break;

      case 3: // NAME
        yylhs.value.build< std::string > ();
        break;

      case 28: // vars
      case 31: // allocs
      case 32: // frees
        yylhs.value.build< std::vector<automate::Variable> > ();
        break;

      case 26: // names
      case 27: // list_name
        yylhs.value.build< std::vector<std::string> > ();
        break;

      case 21: // layer
        yylhs.value.build< std::vector<std::vector<std::string>> > ();
        break;

      case 20: // init_alloc
        yylhs.value.build< std::vector<std::vector<std::vector<std::string>>> > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 60 "yacc.ypp" // lalr1.cc:859
    { 
    generate_automate->initial_valuation = Valuation (yystack_[9].value.as< std::vector<std::vector<std::vector<std::string>>> > (),nb_vars);
    yylhs.value.as< automate::Automate* > () = generate_automate;
  }
#line 661 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 3:
#line 69 "yacc.ypp" // lalr1.cc:859
    {
    yystack_[2].value.as< std::vector<std::vector<std::vector<std::string>>> > ().push_back(yystack_[0].value.as< std::vector<std::vector<std::string>> > ()); 
    nb_vars = nb_vars < yystack_[0].value.as< std::vector<std::vector<std::string>> > ().size()?yystack_[0].value.as< std::vector<std::vector<std::string>> > ().size():nb_vars;
    yylhs.value.as< std::vector<std::vector<std::vector<std::string>>> > () = yystack_[2].value.as< std::vector<std::vector<std::vector<std::string>>> > ();
  }
#line 671 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 4:
#line 74 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::vector<std::vector<std::vector<std::string>>> > () = {yystack_[0].value.as< std::vector<std::vector<std::string>> > ()};
           nb_vars = nb_vars < yystack_[0].value.as< std::vector<std::vector<std::string>> > ().size()?yystack_[0].value.as< std::vector<std::vector<std::string>> > ().size():nb_vars; 
  }
#line 679 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 5:
#line 77 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::vector<std::vector<std::vector<std::string>>> > () = {};}
#line 685 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 6:
#line 79 "yacc.ypp" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::vector<std::string>> > () = {yystack_[0].value.as< std::vector<std::string> > ()}; }
#line 691 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 7:
#line 80 "yacc.ypp" // lalr1.cc:859
    {yystack_[1].value.as< std::vector<std::vector<std::string>> > ().push_back(yystack_[0].value.as< std::vector<std::string> > ()); yylhs.value.as< std::vector<std::vector<std::string>> > () = yystack_[1].value.as< std::vector<std::vector<std::string>> > (); }
#line 697 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 8:
#line 83 "yacc.ypp" // lalr1.cc:859
    { /*Nothing to do.*/ }
#line 703 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 9:
#line 84 "yacc.ypp" // lalr1.cc:859
    { /*Nothing to do.*/ }
#line 709 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 10:
#line 87 "yacc.ypp" // lalr1.cc:859
    {
     if (generate_automate->start!=nullptr){
       yy::parser::error (yylhs.location,"A start state was already choosen.");
     }
     generate_automate->states.push_back(State(yystack_[1].value.as< std::string > ()));
     std::list<State>::iterator ite = generate_automate->states.end();
     ite--;
     generate_automate->start = &(*ite);
     generate_automate->transitions.insert(std::pair<State*, vector<Transition*>>(generate_automate->start, {}));
   }
#line 724 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 11:
#line 97 "yacc.ypp" // lalr1.cc:859
    { 
    generate_automate->states.push_back(State(yystack_[1].value.as< std::string > ()));
    std::list<State>::iterator ite = generate_automate->states.end();
    ite--;
    generate_automate->endStates.push_back(&(*ite));
    generate_automate->transitions.insert(std::pair<State*, vector<Transition*>>(&(*ite), {}));
  }
#line 736 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 12:
#line 104 "yacc.ypp" // lalr1.cc:859
    {
    generate_automate->states.push_back(State(yystack_[0].value.as< std::string > ()));
    std::list<State>::iterator ite = generate_automate->states.end();
    ite--;
    generate_automate->transitions.insert(std::pair<State*, vector<Transition*>>(&(*ite), {}));
  }
#line 747 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 13:
#line 110 "yacc.ypp" // lalr1.cc:859
    {
    if (generate_automate->start!=nullptr){
      yy::parser::error (yylhs.location,"A start state was already choosen.");
    }
    generate_automate->states.push_back(State(yystack_[2].value.as< std::string > ()));
    std::list<State>::iterator ite = generate_automate->states.end();
    ite--;
    generate_automate->start = &(*ite);
    generate_automate->endStates.push_back(&(*ite));
    generate_automate->transitions.insert(std::pair<State*, vector<Transition*>>(generate_automate->start, {}));
  }
#line 763 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 14:
#line 121 "yacc.ypp" // lalr1.cc:859
    {
    if (generate_automate->start!=nullptr){
      yy::parser::error (yylhs.location,"A start state was already choosen.");
    }
    generate_automate->states.push_back(State(yystack_[2].value.as< std::string > ()));
    std::list<State>::iterator ite = generate_automate->states.end();
    ite--;
    generate_automate->start = &(*ite);
    generate_automate->endStates.push_back(&(*ite));
    generate_automate->transitions.insert(std::pair<State*, vector<Transition*>>(generate_automate->start, {}));
  }
#line 779 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 15:
#line 134 "yacc.ypp" // lalr1.cc:859
    {/*Nothing to do.*/ }
#line 785 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 16:
#line 135 "yacc.ypp" // lalr1.cc:859
    {/*Nothing to do.*/ }
#line 791 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 17:
#line 139 "yacc.ypp" // lalr1.cc:859
    {
     //Create the new transition for each pairs (origine, destination)
     for(string ori : yystack_[3].value.as< std::vector<std::string> > ()){
       State* origine_state = generate_automate->getState(ori);
       if(origine_state == nullptr){
         yy::parser::error (yylhs.location, "state undeclared : "+ori);
       }
       for(string dest : yystack_[2].value.as< std::vector<std::string> > ()){
         State* destination_state = generate_automate->getState(dest);
         if(destination_state == nullptr){
           yy::parser::error (yylhs.location, "state undeclared : "+dest);
         }
         Transition* new_trans = new Var_Transition(origine_state, destination_state,yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().first, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().second, yystack_[1].value.as< std::vector<automate::Variable> > ());
         generate_automate->transitions[origine_state].push_back(new_trans);
       }
     }
   }
#line 813 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 18:
#line 157 "yacc.ypp" // lalr1.cc:859
    {
      //Create the new transition for each pairs of origine, destination
      for(string ori : yystack_[3].value.as< std::vector<std::string> > ()){
        State* origine_state = generate_automate->getState(ori);
        if(origine_state == nullptr){
          yy::parser::error (yylhs.location, "state undeclared : "+ori);
        }
        for(string dest : yystack_[2].value.as< std::vector<std::string> > ()){
          State* destination_state = generate_automate->getState(dest);
          if(destination_state == nullptr){
            yy::parser::error (yylhs.location, "state undeclared : "+dest);
          }
          Transition* new_trans = new Epsilon_Transition(origine_state, destination_state, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().first, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().second);
          generate_automate->transitions[origine_state].push_back(new_trans);
        }
      }
    }
#line 835 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 19:
#line 176 "yacc.ypp" // lalr1.cc:859
    {
      //Create the new transition for each pairs of origine, destination
      for(string ori : yystack_[3].value.as< std::vector<std::string> > ()){
        State* origine_state = generate_automate->getState(ori);
        if(origine_state == nullptr){
          yy::parser::error (yylhs.location, "state undeclared : "+ori);
        }
        for(string dest : yystack_[2].value.as< std::vector<std::string> > ()){
          State* destination_state = generate_automate->getState(dest);
          if(destination_state == nullptr){
            yy::parser::error (yylhs.location, "state undeclared : "+dest);
          }
          Transition* new_trans = new Universal_Transition(origine_state, destination_state, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().first, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().second);
          generate_automate->transitions[origine_state].push_back(new_trans);
        }
      }
    }
#line 857 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 20:
#line 194 "yacc.ypp" // lalr1.cc:859
    {
      //Create the new transition for each pairs of origine, destination
      for(string ori : yystack_[3].value.as< std::vector<std::string> > ()){
        State* origine_state = generate_automate->getState(ori);
        if(origine_state == nullptr){
          yy::parser::error (yylhs.location, "state undeclared : "+ori);
        }
        for(string dest : yystack_[2].value.as< std::vector<std::string> > ()){
          State* destination_state = generate_automate->getState(dest);
          if(destination_state == nullptr){
            yy::parser::error (yylhs.location, "state undeclared : "+dest);
          }
          Transition* new_trans = new Constant_Transition(origine_state, destination_state, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().first, yystack_[0].value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ().second, yystack_[1].value.as< std::string > ());
          generate_automate->transitions[origine_state].push_back(new_trans);
        }
      }
      generate_automate->constants.insert(yystack_[1].value.as< std::string > ());
    }
#line 880 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 21:
#line 214 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::vector<std::string> > () = yystack_[1].value.as< std::vector<std::string> > ();}
#line 886 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 22:
#line 215 "yacc.ypp" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::string> > () = vector<std::string>();}
#line 892 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 23:
#line 218 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::vector<std::string> > () = {yystack_[0].value.as< std::string > ()};}
#line 898 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 24:
#line 221 "yacc.ypp" // lalr1.cc:859
    { yystack_[2].value.as< std::vector<std::string> > ().push_back(yystack_[0].value.as< std::string > ()); yylhs.value.as< std::vector<std::string> > () = yystack_[2].value.as< std::vector<std::string> > ();}
#line 904 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 25:
#line 222 "yacc.ypp" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::string> > () = { yystack_[0].value.as< std::string > () };}
#line 910 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 26:
#line 225 "yacc.ypp" // lalr1.cc:859
    { yystack_[2].value.as< std::vector<automate::Variable> > ().push_back(Variable(yystack_[0].value.as< std::pair<int,int> > ().first,yystack_[0].value.as< std::pair<int,int> > ().second)); yylhs.value.as< std::vector<automate::Variable> > () = yystack_[2].value.as< std::vector<automate::Variable> > ();}
#line 916 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 27:
#line 226 "yacc.ypp" // lalr1.cc:859
    { yylhs.value.as< std::vector<automate::Variable> > () = vector<Variable>(); yylhs.value.as< std::vector<automate::Variable> > ().push_back(Variable(yystack_[0].value.as< std::pair<int,int> > ().first,yystack_[0].value.as< std::pair<int,int> > ().second));}
#line 922 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 28:
#line 228 "yacc.ypp" // lalr1.cc:859
    { yylhs.value.as< std::pair<int,int> > () = pair<int,int>(yystack_[3].value.as< int > (),yystack_[1].value.as< int > ()); }
#line 928 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 29:
#line 231 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ()= pair<vector<Variable>,vector<Variable>>({},{});}
#line 934 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 30:
#line 232 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ()= pair<vector<Variable>,vector<Variable>>(yystack_[0].value.as< std::vector<automate::Variable> > (),{});}
#line 940 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 31:
#line 233 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ()= pair<vector<Variable>,vector<Variable>>({},yystack_[0].value.as< std::vector<automate::Variable> > ());}
#line 946 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 32:
#line 234 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ()= pair<vector<Variable>,vector<Variable>>(yystack_[0].value.as< std::vector<automate::Variable> > (),yystack_[1].value.as< std::vector<automate::Variable> > ());}
#line 952 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 33:
#line 235 "yacc.ypp" // lalr1.cc:859
    {yylhs.value.as< std::pair<std::vector<automate::Variable>,std::vector<automate::Variable>> > ()= pair<vector<Variable>,vector<Variable>>(yystack_[1].value.as< std::vector<automate::Variable> > (),yystack_[0].value.as< std::vector<automate::Variable> > ());}
#line 958 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 34:
#line 238 "yacc.ypp" // lalr1.cc:859
    {
    yylhs.value.as< std::vector<automate::Variable> > ()=yystack_[0].value.as< std::vector<automate::Variable> > ();
  }
#line 966 "yacc.tab.cpp" // lalr1.cc:859
    break;

  case 35:
#line 243 "yacc.ypp" // lalr1.cc:859
    {
    yylhs.value.as< std::vector<automate::Variable> > ()=yystack_[0].value.as< std::vector<automate::Variable> > ();
  }
#line 974 "yacc.tab.cpp" // lalr1.cc:859
    break;


#line 978 "yacc.tab.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -24;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      18,   -24,    -1,     3,    -4,    18,   -24,   -24,   -24,    19,
     -24,     1,   -24,   -24,    26,    10,    18,   -24,    33,    20,
      25,   -24,    27,    29,     7,   -24,   -24,    28,   -24,    18,
      30,   -24,    18,    18,    -2,   -24,    16,    37,    16,    16,
       2,   -24,    34,    34,   -24,    31,    32,    38,   -24,   -24,
      34,   -24,    39,    39,   -24,   -24,    40,   -24,    42,   -24
  };

  const unsigned char
  parser::yydefact_[] =
  {
       5,    23,     0,     0,     0,     4,     6,    25,    22,     0,
       1,     0,     7,    21,     0,     0,     3,    24,     0,    12,
       0,     9,    10,    11,     0,    13,    14,     0,     8,     0,
       0,    16,     0,     2,     0,    15,    29,     0,    29,    29,
      29,    27,     0,     0,    20,    30,    31,     0,    18,    19,
       0,    17,    35,    34,    33,    32,     0,    26,     0,    28
  };

  const signed char
  parser::yypgoto_[] =
  {
     -24,   -24,   -24,    41,   -24,    24,   -24,    21,    -5,   -24,
      -8,     5,   -23,    11,     6
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     3,     4,     5,    20,    21,    30,    31,     6,     9,
      40,    41,    44,    45,    46
  };

  const unsigned char
  parser::yytable_[] =
  {
      12,    36,     7,    10,     1,     8,     2,    37,    11,    50,
      19,    12,    38,    39,    15,    48,    49,    51,    42,    43,
      27,     1,    18,     2,    32,    13,    14,    34,    32,    17,
      22,    23,    42,    43,    52,    53,    19,    24,    25,    26,
      29,    47,    33,    37,    58,    56,    50,    42,    28,    43,
      59,    54,    16,     0,    35,    57,     0,    55
  };

  const signed char
  parser::yycheck_[] =
  {
       5,     3,     3,     0,     3,     6,     5,     9,    12,     7,
       3,    16,    14,    15,    13,    38,    39,    40,    16,    17,
      13,     3,    12,     5,    29,     6,     7,    32,    33,     3,
      10,    11,    16,    17,    42,    43,     3,    12,    11,    10,
      12,     4,    12,     9,     4,     7,     7,    16,    24,    17,
       8,    45,    11,    -1,    33,    50,    -1,    46
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     5,    19,    20,    21,    26,     3,     6,    27,
       0,    12,    26,     6,     7,    13,    21,     3,    12,     3,
      22,    23,    10,    11,    12,    11,    10,    13,    23,    12,
      24,    25,    26,    12,    26,    25,     3,     9,    14,    15,
      28,    29,    16,    17,    30,    31,    32,     4,    30,    30,
       7,    30,    28,    28,    32,    31,     7,    29,     4,     8
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    18,    19,    20,    20,    20,    21,    21,    22,    22,
      23,    23,    23,    23,    23,    24,    24,    25,    25,    25,
      25,    26,    26,    26,    27,    27,    28,    28,    29,    30,
      30,    30,    30,    30,    31,    32
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,    10,     3,     1,     0,     1,     2,     3,     1,
       2,     2,     1,     3,     3,     3,     1,     4,     4,     4,
       4,     3,     2,     1,     3,     1,     3,     1,     5,     0,
       1,     1,     2,     2,     2,     2
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "NAME", "NUMBER", "LACOLADE",
  "RACOLADE", "COMMA", "R_PARENTHESE", "L_PARENTHESE", "START_NODE",
  "END_NODE", "NLINE", "DOUBLE_SLASH", "NONE", "ALL", "FREE", "ALLOC",
  "$accept", "autom", "init_alloc", "layer", "nodes", "node", "arretes",
  "arrete", "names", "list_name", "vars", "var", "actions", "allocs",
  "frees", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    60,    60,    69,    74,    77,    79,    80,    83,    84,
      87,    97,   104,   110,   121,   134,   135,   139,   157,   176,
     194,   214,   215,   218,   221,   222,   225,   226,   228,   231,
     232,   233,   234,   235,   238,   243
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy
#line 1381 "yacc.tab.cpp" // lalr1.cc:1167
#line 247 "yacc.ypp" // lalr1.cc:1168

void yy::parser::error (const yy::location& l,
                          const std::string& m)
{
  driver.error (l, m);
}

/*int variable_to_int(string variable_name, autom_driver& driver){
  int variable = -1;
  try{
    variable = driver.map_variable.at(variable_name);
  }catch(const std::out_of_range& oor){
    variable = driver.variable_cpt;
    driver.map_variable[variable_name]=driver.variable_cpt++;
  }
  return variable;
}*/
