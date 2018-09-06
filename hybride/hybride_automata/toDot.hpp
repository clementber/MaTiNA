#ifndef __AUTOMATE_TO_DOT__

#define __AUTOMATE_TO_DOT__

#include "grammar.hpp"
#include <iostream>

void convert_to_dot(automate::Automate* autom, std::ostream& ouput);
void output_Clocks_constraint(ostream& output, automate::DBM const& clocks_constraints,
                              vector<automate::Clock*> const& clocks);
#endif
