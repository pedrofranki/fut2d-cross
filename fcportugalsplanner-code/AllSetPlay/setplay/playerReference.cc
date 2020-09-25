#include <setplay/playerReference.h>
#include <setplay/setplayexception.h>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

#include "simpleParsers.h"

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace fcportugal::setplay;


Object* PlayerReference::parse(const string& in, string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  Object* resPR = PlayerRole::parse(in,out,params,players);
  if(resPR) return resPR;

  Object* resPID = PlayerID::parse(in,out,params,players);
  if(resPID) return resPID;

  return NULL;
}

vector<PlayerReference*>
PlayerReference::parsePlayerList(const string& in, string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  //check for list label
  rule<> list_p
    = *space_p >> str_p("(list") ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),list_p,
						       nothing_p);

  if(result.hit){
    string rest;
    
    PlayerReference* pl= 
      (PlayerReference*)PlayerReference::parse(in.substr(result.length), rest,
					       params,players);
    if(!pl){
      //empty list!?!
      out=in;
      return *(new vector<PlayerReference*>());
    }
    
    // To store the players
    vector<PlayerReference*>* players_here = new vector<PlayerReference*>;
    
    while(pl){
      players_here->push_back(pl);
      
      //Parse a new player
      pl= (PlayerReference*)PlayerReference::parse(rest, rest,params,players);
    }
    
    result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					    *space_p>>')',
					    nothing_p);
    if(result.hit){
      out=rest.substr(result.length);
      return *players_here;
    }
  }
  else{
    //try to parse single player
    string rest;
    
    PlayerReference* pl= 
      (PlayerReference*)PlayerReference::parse(in, rest,
					       params,players);
    if(pl){
      vector<PlayerReference*>* players_here = new vector<PlayerReference*>;
      players_here->push_back(pl);

      out=rest;
      return *players_here;
    }
  }
   
  out=in;
  return *(new vector< PlayerReference*>());
}

vector<const PlayerReference*> PlayerReference::toVectorConstPlayerReference(const vector<PlayerReference*>& vec){
	vector<const PlayerReference*>* res = new vector<const PlayerReference*>(vec.size());

	copy(vec.begin(),vec.end(),res->begin());

	return *res;
}

void PlayerRole::print(ostream &os) const{
  if(!instantiated())
    os<< "(playerRole :roleName "<< roleName()<<")";
  else
    value_->print(os);
}

const PlayerID* PlayerRole::value() const{
  if(!instantiated())
	  throw SetplayException("Accessing value of non instantiated PlayerRole:"+roleName());
  
  return value_->value();
};

bool PlayerRole::equals(const PlayerReference* other) const{
  if(instantiated())
    if(other->isRole())
      return roleName()==((PlayerRole*)other)->roleName() && ((PlayerRole*)other)->instantiated() 
	&& value()->equals(((PlayerRole*)other)->value());
    else
      return value()->equals((PlayerID*)other);
  else
    return other->isRole() && !((PlayerRole*)other)->instantiated() 
      && roleName()==((PlayerRole*)other)->roleName();

}


const string PlayerRole::instantiationText(const Context& world,
					   bool shortForm) const{
  if(instantiated())
    return value_->instantiationText(world,shortForm);

  throw SetplayException("Trying to access instantiation text from non-instantiated PlayerRole!");
  return "";    
}

Object* PlayerRole::parse(const string& in, string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*> &players){
  string name;

  rule<> playerRole_p
    = *space_p >> str_p("(playerRole") >> *space_p >> ":roleName" >> *space_p
	       >> identifier_p[assign_a(name)] >> *space_p >> ")";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),playerRole_p,
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new PlayerRole(name);
  }
  else {
    //Must check if it is simply one of the existent role names
    for(unsigned int i=0; i!= players.size(); i++)
      if(players.at(i)->isRole()){
	playerRole_p
	  = *space_p 
	  >> str_p(((PlayerRole*)players.at(i))->roleName().c_str())
	  >> *space_p;
  
      result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),playerRole_p,
					      nothing_p);
      if(result.hit && players.at(i)->isRole()){
	out=in.substr(result.length);
	PlayerRole* pl=(PlayerRole*)(players.at(i));
	return pl->deepCopy();
      }
    }
    out=in;
    return NULL;
  }
}

Object* PlayerRole::deepCopy(const vector<SetplayParameter*>& params,
			     const vector<PlayerReference*>& players) const{
  for(unsigned int i=0; i!= players.size();i++)
    if(players.at(i)->isRole() 
       && ((PlayerRole*)players.at(i))->roleName()==roleName())
    	// CANNOT be a copy...
      return players.at(i);
  throw SetplayException("Player Role not found in players list!");
  return NULL;
}

Object* PlayerRole::inversion(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const{
  
  return deepCopy(params,players);
}

PlayerReference* PlayerRole::deepCopy() const{
   PlayerRole* res=new PlayerRole(roleName());

   if(instantiated())
     res->set((PlayerID*)value_->deepCopy());

   return res;
}



bool PlayerRole::isRole() const{
  return true;
}

PlayerID::PlayerID(string team_, unsigned int number_):PlayerReference(),team(team_),number(number_){};


void PlayerID::print(ostream &os) const{
  os<< "(player :team "<< team<< " :number "<< number<<")";
}

bool PlayerID::isRole() const{
  return false;
}


bool PlayerID::equals(const PlayerReference* other) const{
  if(other->isRole())
    return ((PlayerRole*)other)->instantiated() && equals(((PlayerRole*)other)->value());
  else
    return team==((PlayerID*)other)->team && number == ((PlayerID*)other)->number;
}

Object* PlayerID::parse(const string& in, string& out,
		const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players){
  //Last argument is ignored, no player roles here...

  // might be, in instantiations, a single number, representing a player from
  // our team
  int number;
  rule<> playerID_p
    = *space_p >> int_p[assign_a(number)];

  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),playerID_p,
						       nothing_p);

  if(result.hit && number >=0){
    out=in.substr(result.length);
    return new PlayerID("our",(unsigned int)number);
  }

  string name;

  playerID_p = *space_p >> str_p("(player") >> *space_p 
			>> ":team" >> *space_p >> identifier_p[assign_a(name)] 
			>> *space_p >> ":number" >> *space_p 
			>> uint_p[assign_a(number)]
			>> ")";
  
  result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),playerID_p,
						       nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new PlayerID(name,number);
  }
  else {
    out=in;
    return NULL;
  }
}

const string PlayerID::instantiationText(const Context& world,
					 bool shortForm) const{
  stringstream ss;
  if(shortForm)
    if(team=="our")
      ss<<number;
    else
      ss<<number+11;
  else
    ss<<"(player :team "<< team<< " :number "<<number<<")";

  return ss.str();
}

vector<PlayerReference*>* PlayerReference::allPlayersFrom(string team){
  if( team!="our" && team!="opp" && team!="any"){
	  throw SetplayException("Bad team name");
    return NULL;
  }
  vector<PlayerReference*>* players= new vector<PlayerReference*>;
  if(team=="our"||team=="any")
    for(unsigned int i=1; i!=12;i++)
      players->push_back(new PlayerID("our",i));
  if(team=="opp"||team=="any")
    for(unsigned int i=1; i!=12;i++)
      players->push_back(new PlayerID("opp",i));

  return players;
}



