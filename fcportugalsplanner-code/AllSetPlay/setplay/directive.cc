#include <setplay/directive.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace fcportugal::setplay;


Directive::Directive(const vector<const PlayerReference*>& players_,vector<Action*> *actions_)
 :players(players_),actions(actions_){}



Directive* Directive::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 rule<> do_p
    = *space_p >> str_p("(do") >> *space_p >> ":players" ;
  
 bool isDo=false;
 
 parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),do_p,
						      nothing_p);

 string rest;
 
 if(result.hit){
   isDo=true;
   rest=in.substr(result.length);
 }
 else{
   rule<> dont_p
     = *space_p >> str_p("(dont") >> *space_p >> ":players";
   
   result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),dont_p,
					   nothing_p);
   
   if(result.hit){
     isDo=false;
     rest=in.substr(result.length);
   }
   else{ //is neither Do nor Dont...
     out=in;
     return NULL;
   }
 }

 //parse players
 // To store the players
 vector<PlayerReference*> players_
   = PlayerReference::parsePlayerList(rest, rest,
				      parameters,players);
   
 if(players_.size()>0){//Players are mandatory
   //parse the actions
   result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					   *space_p
					   >>":actions",
					   nothing_p);
   if(!result.hit){ // parse failed
     out=in;
     return NULL;
   }
   
   // To store the actions
   vector<Action*>* actions 
     = Action::parseActionList(rest.substr(result.length),rest,
			       parameters,players);
   
   
   //Check for the final ')'
   result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					   *space_p>>')',
					   nothing_p);
   if(result.hit){
     out=rest.substr(result.length);
     if(isDo)
       return new Do(PlayerReference::toVectorConstPlayerReference(players_),actions);
     else
       return new Dont(PlayerReference::toVectorConstPlayerReference(players_),actions);
   }
 }
 out=in;
 return NULL;
}




const vector<const PlayerReference*>& Directive::getPlayers() const{
  return players;
}

void Directive::setActions(vector<Action*> *actions_){
  actions=actions_;
}

const vector<Action*>* Directive::getActions() const{
  return actions;
}

void Directive::setActionsAsNotDone() const{
  for(unsigned int i=0; i!= actions->size(); i++)
    actions->at(i)->done=false;
}

void Directive::substituteNamedRegions(const Context& world){

  for(unsigned int i =0; i!= actions->size();i++)
    actions->at(i)->substituteNamedRegions(world);

}



void Directive::printAux(ostream &os) const{
  os<<" :players (list ";
  for(unsigned int i = 0; i!=players.size();i++){
    players.at(i)->print(os);
    os<<" ";
  }
  os<<") :actions (list ";
  for(unsigned int i = 0; i!=actions->size();i++){
    actions->at(i)->print(os);
    os<<" ";
  }
  os<<"))";
}

void Directive::printAux2(ostream &os, unsigned int &indent) const{
  os<<"\t:players (list ";
  for(unsigned int i = 0; i!=players.size();i++){
    players.at(i)->print(os);
    os<<" ";
  }
  os<<")"<< "\n";
  for(unsigned int i= 0; i < indent; i++)
      os << "\t";
  os << ":actions (list ";
  for(unsigned int i = 0; i!=actions->size();i++){
    actions->at(i)->print(os);
    os<<" ";
  }
  os<<")"<< "\n";
}

Do::Do(const vector<const PlayerReference*>& players,vector<Action*> *actions)
  :Directive(players,actions){}

void Do::print(ostream &os) const{
  os<< "(do ";
  printAux(os);
}

void Do::print2(ostream &os, unsigned int &indent) const{
  for(unsigned int i= 0; i < indent; i++)
    os << "\t";
  os<< "(do ";
  indent++;
  printAux2(os, indent);
  indent--;
  for(unsigned int i= 0; i < indent; i++)
      os << "\t";
  os << ")\n";
}

Directive* Do::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players_sub) const{

  vector<PlayerReference*>* players_copy= new vector<PlayerReference*>;

  for(unsigned int i =0; i!= players.size();i++)
    players_copy->push_back((PlayerReference*)players.at(i)->deepCopy(params,players_sub));

  vector<Action*>* actions_copy= new vector<Action*>;

  for(unsigned int i =0; i!= actions->size();i++)
    actions_copy->push_back(actions->at(i)->deepCopy(params,players_sub));

  return new Do(PlayerReference::toVectorConstPlayerReference(*players_copy),actions_copy);
}

Directive* Do::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players_sub) const{

  vector<const PlayerReference*>* players_copy= new vector<const PlayerReference*>;

  for(unsigned int i =0; i!= players.size();i++)
    players_copy->push_back((PlayerReference*)players.at(i)->deepCopy(params,
								       players_sub));

  vector<Action*>* actions_copy= new vector<Action*>;

  for(unsigned int i =0; i!= actions->size();i++)
    actions_copy->push_back(actions->at(i)->inversion(params,players_sub));

  return new Do(*players_copy,actions_copy);
}


Dont::Dont(const vector<const PlayerReference*>& players,vector<Action*> *actions)
  :Directive(players,actions){}

void Dont::print(ostream &os) const{
  os<< "(dont ";
  printAux(os);
}

void Dont::print2(ostream &os, unsigned int &indent) const{
  os<< "(dont ";
  indent++;
  printAux2(os, indent);
  indent--;
  for(unsigned int i= 0; i < indent; i++)
      os << "\t";
  os << ")";
}

Directive* Dont::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players_sub) const{

  //Similar to same method in Do
  vector<const PlayerReference*>* players_copy= new vector<const PlayerReference*>;

  for(unsigned int i =0; i!= players.size();i++)
    players_copy->push_back((PlayerReference*)players.at(i)->deepCopy(params,players_sub));

  vector<Action*>* actions_copy= new vector<Action*>;

  for(unsigned int i =0; i!= actions->size();i++)
    actions_copy->push_back(actions->at(i)->deepCopy(params,players_sub));

  return new Dont(*players_copy,actions_copy);
}

Directive* Dont::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players_sub) const{

  //Similar to same method in Do
  vector<const PlayerReference*>* players_copy= new vector<const PlayerReference*>;
  
  for(unsigned int i =0; i!= players.size();i++)
    players_copy->push_back((PlayerReference*)players.at(i)->deepCopy(params,players_sub));
  
  vector<Action*>* actions_copy= new vector<Action*>;
  
  for(unsigned int i =0; i!= actions->size();i++)
    actions_copy->push_back(actions->at(i)->inversion(params,players_sub));

  return new Dont(*players_copy,actions_copy);
}


