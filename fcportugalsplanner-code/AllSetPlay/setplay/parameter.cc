#include <setplay/parameter.h>
#include <setplay/simpleParsers.h>
#include <setplay/setplayexception.h>

#include "region.h"

#include <math.h>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace fcportugal::setplay;

SetplayParameter::SetplayParameter(string const name)
  :m_instantiated(false),name_(string(name)){}

SetplayParameter* 
SetplayParameter::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  SetplayParameter* res = Integer::parse(in,out,parameters);
  if(res) return res;

  res = Decimal::parse(in,out,parameters);
  if(res) return res;

  res = (RegVar*)Region::parse(in,out,parameters,players);
  if(res) return res;

  res = (PointVar*)Point::parse(in,out,parameters,players);
  if(res) return res;

  return NULL;
}

Integer::Integer(string const name_):SetplayParameter(name_),_value(0)
{}


Integer::Integer(int const value):SetplayParameter("UNNAMED"),
				  _value(value)
{m_instantiated = true;}

// string const Integer::type() const{
//   return "integer";
// }

const string Integer::instantiationText(const Context& world,
					bool shortForm) const{
  if(instantiated()){
    stringstream ss;
    ss<<_value;
    return ss.str();
  }
  else
	  throw SetplayException("Accessing instantiation text of not instantiated integer");
  return "";
}

void Integer::print(ostream &os) const{
  if(instantiated())
    os<<_value;
  else
    os<<"(integer :name "<< name()<<")";
}
 
SetplayParameter* Integer::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters){
  string name;

  rule<> integer_p
    = *space_p >> str_p("(integer") >> *space_p >> ":name" >> *space_p 
	       >> identifier_p[assign_a(name)] >> *space_p >> ")";

  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),integer_p,
						      nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new Integer(name);
  }
  else{
    //Check if it is a plain number
    int num;
    result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
					    *space_p >> int_p[assign_a(num)], 
					    nothing_p);
    if(result.hit){
      out=in.substr(result.length);
      return new Integer(num);
    }
    else{ //Check if it is a parameter name, included in SetplayParameter vector
      for(unsigned int i=0; i!= parameters.size(); i++){
	rule<> param_p
	  = *space_p >> str_p(parameters.at(i)->name().c_str()) >> *space_p;
	
	result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),param_p,
						nothing_p);
	if(result.hit  && parameters.at(i)->type()=='i'){
	  out=in.substr(result.length);
	  return parameters.at(i)->deepCopy();
	}
      }
    }
  }
  out=in;
  return NULL;
}

Integer* Integer::deepCopy(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const{
  if(name()!="UNNAMED"){
    for(unsigned int i = 0; i!=params.size();i++)
      if(params.at(i)->name()==name())
	return (Integer*)params.at(i);

    throw SetplayException("Integer parameter not found in parameter list!");
    return NULL;
  }
  else
    return new Integer(_value);
}

Integer* Integer::deepCopy() const{
  if(name()!="UNNAMED"){
    Integer* res=new Integer(name());
    if(instantiated())
      res->_value=_value;
    return res;
  }
  else
    return new Integer(_value);
}

 Integer* Integer::inversion(const vector<SetplayParameter*>& params,
 			    const vector<PlayerReference*>& players) const{
   if(name()!="UNNAMED"){
     for(unsigned int i = 0; i!=params.size();i++)
       if(params.at(i)->name()==name())
	 return (Integer*)params.at(i);
     
     throw SetplayException("Integer parameter not found in parameter list!");
     return NULL;
  }
   else
     return new Integer(-_value);
 }



Decimal::Decimal(string const name):SetplayParameter(name),_value(0.0){}

Decimal::Decimal(double const value):SetplayParameter("UNNAMED"),
				     _value(value)
{m_instantiated = true;}


const string Decimal::instantiationText(const Context& world,
					bool shortForm) const{
  if(m_instantiated)
    if(shortForm){
      stringstream ss;
      ss<<(int)round(_value);
      return ss.str();
    }
    else{
      stringstream ss;
      ss<<_value;
      return ss.str();
    }
  else
	  throw SetplayException("Accessing instantiation text of not instantiated integer");
  return "";
}


void Decimal::print(ostream &os) const{
  if(instantiated()) 
    os<<_value;
  else
    os<<"(decimal :name "<<name()
      <<")";
}

Decimal* Decimal::deepCopy(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const{
  if(name()!="UNNAMED"){
    for(unsigned int i = 0; i!=params.size();i++)
      if(params.at(i)->name()==name())
	return (Decimal*)params.at(i);

    throw SetplayException("Decimal parameter not found in parameter list!");
    return NULL;
  }
  else
    return new Decimal(_value);
}


Decimal* Decimal::deepCopy() const{
  if(name()!="UNNAMED"){
    Decimal* res=new Decimal(name());
    if(instantiated())
      res->_value=_value;

    return res;
  }
  else
    return new Decimal(_value);
}

Decimal* Decimal::inversion(const vector<SetplayParameter*>& params,
 			    const vector<PlayerReference*>& players) const{
  if(name()!="UNNAMED"){
    for(unsigned int i = 0; i!=params.size();i++)
      if(params.at(i)->name()==name())
	return (Decimal*)params.at(i);
    
    throw SetplayException("Decimal parameter not found in parameter list!");
    return NULL;
  }
  else
    return new Decimal(-_value);
}

// Decimal* Decimal::inversion() const{
//   return deepCopy();
// }


SetplayParameter* Decimal::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters){
  string name;
  
  rule<> decimal_p
    = *space_p >> str_p("(decimal") >> *space_p >> ":name" >> *space_p
	       >> identifier_p[assign_a(name)] >> *space_p >> ")";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),decimal_p, 
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new Decimal(name);
  }
  else {
    //Check if it is a plain number
    double num;
    result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
					    *space_p >> real_p[assign_a(num)], 
					    nothing_p);
    if(result.hit){
      out=in.substr(result.length);
      return new Decimal(num);
    }
    else{
      //Check if it is a parameter name, included in SetplayParameter vector
      for(unsigned int i=0; i!= parameters.size(); i++){
	rule<> param_p
	  = *space_p >> str_p(parameters.at(i)->name().c_str()) >> *space_p;
	
	result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),param_p,
						nothing_p);
	if(result.hit  && parameters.at(i)->type()=='d'){
	  out=in.substr(result.length);
	  return parameters.at(i)->deepCopy();
	}
      }
    }
  }
  out=in;
  return NULL;
}

