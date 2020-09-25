
#include <setplay/object.h>
#include <setplay/playerReference.h>
 
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace std;
using namespace fcportugal::setplay;



Object* Object::parse(const std::string& in, std::string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  Object* obj=NULL;

  obj=StaticObject::parse(in,out,params,players);
  if(obj) return obj;

  obj=MobileObject::parse(in,out,params,players);
  
  return obj;
}

Object* StaticObject::parse(const std::string& in, std::string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  Object* obj=NULL;

  obj=OppGoal::parse(in,out);
  if(obj) return obj;


  obj=OwnGoal::parse(in,out);
  if(obj) return obj;
  //TODO: other static object parsers

  return obj;
}

Object* MobileObject::parse(const std::string& in, std::string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  Object* obj=NULL;

  obj=Ball::parse(in,out);
  if(obj) return obj;

  obj=PlayerReference::parse(in,out,params,players);
  
  return obj;
}

Object* Ball::parse(const std::string& in, std::string& out){

  rule<> ball_p
    = *space_p >> str_p("(ball") >> *space_p >> ")";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),ball_p,
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new Ball();
  }
  else {
    out=in;
    return NULL;
  }
}


Object* OppGoal::parse(const std::string& in, std::string& out){

  rule<> ball_p
    = *space_p >> str_p("(oppGoal") >> *space_p >> ")";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),ball_p,
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new OppGoal();
  }
  else {
    out=in;
    return NULL;
  }
}


Object* OwnGoal::parse(const std::string& in, std::string& out){

  rule<> ball_p
    = *space_p >> str_p("(ownGoal") >> *space_p >> ")";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),ball_p,
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new OwnGoal();
  }
  else {
    out=in;
    return NULL;
  }
}
