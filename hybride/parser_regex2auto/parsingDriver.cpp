#include "parsingDriver.hpp"
#include "yacc.tab.hpp"
#include <fstream>

regex_driver::regex_driver ()
  : trace_scanning (false), trace_parsing (false)
{
}

regex_driver::~regex_driver ()
{
}

unordered_set<string> find_variables(string filename){
  unordered_set<string> res;
  fstream file(filename, fstream::in);
  char c;
  string buffer = "";
  bool variable = false;
  while(file.get(c)){
    if( c == ' '|| c =='\n' || c=='\t'){
      continue;
    }else if(c == '(' ||c == ')'||c == '<'||c == '>'||c == '+'|| c == '*'
           ||c == '?' ||c == '.'||c == '&'||c == '|'||c == '%'|| c == '~'){
      if(buffer != "" && variable){
        res.insert(buffer);
        variable = false;
      }
      buffer = "";
    }else if(c == '#'){
      if(buffer != ""){
        cout << "Fail to parse. Misplaced '#'\n";
        file.close();
        exit(1);
      }
      variable = true;
    }else if (c == '!'){
      if(buffer != ""){
        res.insert(buffer);
        variable = false;
        buffer = "";
      }
    }else if(c == '['){
      bool over = false;
      while(file.get(c)){
        if(c ==']'){
          over = true;
          break;
        }
      }
      if(!over || buffer != ""){
        cout << "Fail to parse. Misplaced '['\n";
        file.close();
        exit(1);
      }
    }else if(c=='{'){
      if(buffer != "" && variable){
        res.insert(buffer);
      }
      variable = false;
      buffer = "";
      bool over = false;
      while(file.get(c)){
        if( c == ' '|| c =='\n' || c=='\t'){
          continue;
        }
        if(c == ','){
          res.insert(buffer);
          buffer = "";
        }else if(c =='}'){
          over = true;
          res.insert(buffer);
          buffer = "";
          break;
        }else if(c == '(' ||c == ')'||c == '<'||c == '>'||c == '+'|| c == '*'
               ||c == '?' ||c == '.'||c == '&'||c == '|'||c == '%'|| c == '@'
               ||c == '#' ||c == '!'||c == '{'||c == '['||c == ']'){
           cout << "Fail to parse. unexpected character" << c << "\n";
           file.close();
           exit(1);
       }else{
          buffer += c;
        }
      }
      if(!over){
        cout << "Fail to parse. Misplaced '{'\n";
        file.close();
        exit(1);
      }
    }else{
      buffer += c;
    }
  }

  file.close();
  return res;
}

int regex_driver::parse (automate::Automate*& automate, const std::string& f)
{
  file = f;
  unordered_set<string> variables_name;
  variables_name = find_variables(f);
  map<string,int> variables_id;
  int nb_variables = 0;
  for(string var_name : variables_name){
    variables_id[var_name] = nb_variables++;
  }
  scan_begin ();
  tnure_ast::AST_node * ast;
  yy::parser parser (&ast, variables_id, *this);
  parser.set_debug_level (trace_parsing);
  int res = parser.parse ();
  scan_end ();
  if(res == 0){
    automate = ast->convert();
    delete ast;
  }else{
    cout << "ERROR : Expression Parsing fail!!\n";
  }
  automate->ressources = nb_variables;
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
