#ifndef SIMPLEPARSERS_H
#define SIMPLEPARSERS_H

#include <boost/spirit/include/classic_core.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;

//Global constants
const rule<> identifier_p = +(alnum_p | '_' | '-');
const rule<> textNoParentesis_p=
  +(alnum_p|'.'|'/'|':'|'-'|'_'|'~');

const rule<> comparison_p = 
  str_p("<=")|str_p("<")|str_p("==")|str_p("!=")|str_p(">=")|str_p(">");

const rule<> agentID_p = "(agent-identifier" 
  >> +space_p >> ":name" >> +space_p 
  >> identifier_p >> +space_p 
  >> ":addresses" >> +space_p >> "(sequence" 
  >> *(+space_p >> textNoParentesis_p)
  >> *space_p >> ')'>> *space_p >> ')';


#endif // SIMPLEPARSERS_H

