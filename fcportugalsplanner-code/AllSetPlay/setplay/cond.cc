#include <setplay/cond.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <setplay/simpleParsers.h>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;

using namespace std;
using namespace fcportugal::setplay;

// Trivial, not virtual, cond evaluators
bool Context::lookup( const CondPlayMode& cond ) const{
  return cond.getPlayMode()==playMode();
}


 
bool Context::lookup( const CondPlayerPos& cond ) const{
  // Setup
  const vector<const PlayerReference*> players_const =  cond.getPlayerSet();
  
    
  int playerCount=0;
  

  for(unsigned int i = 0; i != players_const.size();i++){
    
    if(cond.getRegion()->includesPoint(playerPos(*(players_const.at(i)->value())),
				       *this))
      playerCount++;
  }
  
  
  return cond.getMinMatch()->value()<=playerCount
    && cond.getMaxMatch()->value()>=playerCount; 
  
  
  /* LMOTA 2009 test implementation, not tested. Decided to use code developed in 2007
  // Will be testing players by their number in the position vector!
  // ATTENTION!  
  const fcportugal::setplay::Region* reg=cond.getRegion();
  
  unsigned int count = 0;
  
  for(unsigned int i=0; i!= cond.getPlayerSet()->size(); i++){
  const PlayerID* pl=NULL;

    if(cond.getPlayerSet()->at(i)->isRole())
      pl=cond.getPlayerSet()->at(i)->value();
    else
      pl=(PlayerID*)cond.getPlayerSet()->at(i);

    if(pl->team=="our")
      if(reg->includesPoint(new PointSimple(new Decimal(PO_TeammatePos[pl->number].x),
					    new Decimal(PO_TeammatePos[pl->number].y))))
	count++;
      else ;
    else 
      if(reg->includesPoint(new PointSimple(new Decimal(PO_OpponPos[pl->number].x),
					    new Decimal(PO_OpponPos[pl->number].y))))
	count++;
  }  
  return cond.getMinMatch()->value() <= (int)count 
    && cond.getMaxMatch()->value() >= (int)count;
  */
}
	
bool Context::lookup( const CondBallPos& cond ) const{

  //2007 code
  return cond.getRegion()->includesPoint(ballPos(),*this);
  /* 2009 test implementation, not tested. Decided to use code developed in 2007

  const fcportugal::setplay::Region* reg=cond.getRegion();

  return reg->includesPoint(new PointSimple(new Decimal(PO_BallPos.x),
					    new Decimal(PO_BallPos.y)));
  */
}

bool Context::lookup( const CondTime& cond ) const{
	return cond.getComp().compare(time(),cond.getValue());

}


Cond* Cond::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  Cond* res=NULL;

  res=CondPlayerPos::parse(in,out,parameters,players);
  if(res) return res;

  res=CondBallPos::parse(in,out,parameters,players);
  if(res) return res;

  res=CondBallOwner::parse(in,out,parameters,players);
  if(res) return res;

  res=CondClearShotAtGoal::parse(in,out,parameters,players);
  if(res) return res;

  res=CondClearPassToPlayer::parse(in,out,parameters,players);
  if(res) return res;

  res=CondClearPassToRegion::parse(in,out,parameters,players);
  if(res) return res;

  res=CondAnd::parse(in,out,parameters,players);
  if(res) return res;

  res=CondOr::parse(in,out,parameters,players);
  if(res) return res;

  res=CondNot::parse(in,out,parameters,players);
  if(res) return res;

  res=CondPlayMode::parse(in,out,parameters,players);
  if(res) return res;

  res=CondTime::parse(in,out,parameters,players);
  if(res) return res;

  res=CondOurGoal::parse(in,out,parameters,players);
  if(res) return res;

  res=CondOppGoal::parse(in,out,parameters,players);
  if(res) return res;

  res=CondGoalDiff::parse(in,out,parameters,players);
  if(res) return res;

  res=CondNearOffsideLine::parse(in,out,parameters,players);
  if(res) return res;

  return NULL;
}



CondPlayerPos::CondPlayerPos(const vector<const PlayerReference*>& players,
			     const Region* reg,
			     Integer* min_match=new Integer(1),
			     Integer* max_match=new Integer(22))
      : Cond(),
	M_min_match( min_match ),
	M_max_match( max_match ),
	m_reg( reg ){
      // if there is a single player and its number is 0, it means that
      // any player is meant, and all players
      // from 1 to 11 will be inserted into the list.
      // if team number is "any", then this will be made with both teams
      if(players.size()==1 && !players.at(0)->isRole()
	 && players.at(0)->value()->number==0){
	//create a new vector
	vector<const PlayerReference*> *allPlayers = new vector<const PlayerReference*>;
	const PlayerReference* player=players.at(0);
	
	if(player->value()->team=="our"||player->value()->team=="any")
	  for(unsigned int i = 1; i!=12; i++)
	    allPlayers->push_back(new PlayerID("our",i));
	if(player->value()->team=="opp"||player->value()->team=="any")
	  for(unsigned int i = 1; i!=12; i++)
	    allPlayers->push_back(new PlayerID("opp",i));

	// set the class attribute
	m_players= *allPlayers;
      }
      else
	m_players= players ;
}

void
CondPlayerPos::print( ostream& out ) const
{
  out << "(ppos :players (list " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out<< ") :min ";
  M_min_match->print(out);
  out<< " :max ";
  M_max_match->print(out);
  out<<" :region ";
  if( m_reg == NULL )
    out << "(null)";
  else
    m_reg->print(out);
  out << ")";
}

void
CondPlayerPos::printPretty( ostream& out, 
			    const string& line_header ) const
{
  out << line_header
      << "are ";

  M_min_match->print(out);
  out << "-" ;
  M_max_match->print(out);
  out << " "<< " (list " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out << "in:" << std::endl;
  if( m_reg == NULL )
    out << line_header << " (null)\n";
  else
    m_reg->printPretty( out, line_header + " " );
}


Cond* CondPlayerPos::parse(const string& in, string& out,
		const vector<SetplayParameter*> &parameters,
		const vector<PlayerReference*>& players){
  rule<> cond_p
    = *space_p >> str_p("(ppos") >> *space_p >> ":players" >> *space_p;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    
    // To store the players
    vector<PlayerReference*> players_cond
      = PlayerReference::parsePlayerList(in.substr(result.length),rest,
					 parameters,players);
    
    Integer *min= new Integer(1), *max= new Integer(11);
    
    
    //Check :min, not mandatory
    rule<> min_p = *space_p >> ":min" >> *space_p;
    result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),min_p,
					   nothing_p);
    if(result.hit)
      min=(Integer*)Integer::parse(rest.substr(result.length),rest,
				   parameters);
    
    
    //Check :max, not mandatory
    rule<> max_p = *space_p >> ":max" >> *space_p;
    result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),max_p,
					   nothing_p);
    if(result.hit)
      max=(Integer*)Integer::parse(rest.substr(result.length),rest,
				   parameters);
    
    //check ":region"
    result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					    *space_p  >> ":region" >> *space_p,
					    nothing_p);
    
    if(result.hit){
      //Must parse region
      Region* reg= Region::parse(rest.substr(result.length), rest,
				 parameters,players);
      
      if(reg){
	//Check for the final ')'
	result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
						nothing_p);
	if(result.hit){
	  out=rest.substr(result.length);
	  return new CondPlayerPos(PlayerReference::toVectorConstPlayerReference(players_cond),reg,min,
				   max);
	}
      }
    }
  }

  
  out=in;
  return NULL;
}

Cond* CondPlayerPos::deepCopy(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondPlayerPos(*players_new,
			   m_reg->deepCopy(params,players),
			   M_min_match->deepCopy(params,players),
			   M_max_match->deepCopy(params,players));
}

Cond* CondPlayerPos::inversion(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondPlayerPos(*players_new,
			   m_reg->inversion(params,players),
			   M_min_match->deepCopy(params,players),
			   M_max_match->deepCopy(params,players));
}



void
CondPlayerPos::substituteNamedRegions(const Context& world){
  m_reg->substituteNamedRegions(world);
}




void CondBallPos::print( std::ostream& out ) const
{ 
  out << "(bpos :region ";
  if( m_reg == NULL )
    out << "(null)";
  else
    m_reg->print(out);
  out << ")";
}

void CondBallPos::printPretty( std::ostream& out, const std::string& line_header ) const
{ 
  out << line_header << "is ball position at: " << endl;
  if( m_reg == NULL )
    out << line_header << " (null)\n";
  else
    m_reg->printPretty( out, line_header + " " );
}

Cond* CondBallPos::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 rule<> cond_p
    = *space_p >> str_p("(bpos") >> *space_p >> ":region" >> *space_p;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;

    //Must parse region
    Region* reg= Region::parse(in.substr(result.length), rest,
			       parameters,players);

    if(reg){
      //Check for the final ')'
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					      nothing_p);
      if(result.hit){
	out=rest.substr(result.length);
	return new CondBallPos(reg);
      }
    }
  }

  out=in;
  return NULL;
}

Cond* CondBallPos::deepCopy(const vector<SetplayParameter*>& params,
			    const vector<PlayerReference*>& players) const{
  if( m_reg!=NULL )
    return new CondBallPos( m_reg->deepCopy(params,players) ) ;
  else
    return new CondBallPos(NULL);
}

Cond* CondBallPos::inversion(const vector<SetplayParameter*>& params,
			    const vector<PlayerReference*>& players) const{
  if( m_reg!=NULL )
    return new CondBallPos( m_reg->inversion(params,players) ) ;
  else
    return new CondBallPos(NULL);
}

void CondBallPos::substituteNamedRegions(const Context& world){
  m_reg->substituteNamedRegions(world);
}


CondBallOwner::CondBallOwner(const vector<const PlayerReference*>& players)
	:Cond(){
  // if there is a single player and its number is 0, it means that
  // any player is meant, and all players
  // from 1 to 11 will be inserted into the list.
  // if team number is "any", then this will be made with both teams
  if(players.size()==1 && !players.at(0)->isRole()
     && players.at(0)->value()->number==0){
    //create a new vector
    vector<const PlayerReference*> *allPlayers = new vector<const PlayerReference*>;
    const PlayerReference* player=players.at(0);
    
    if(player->value()->team=="our"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("our",i));
    if(player->value()->team=="opp"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("opp",i));
    
    // set the class attribute
    m_players= *allPlayers;
  }
  else
    m_players= players ;
}


void
CondBallOwner::print( std::ostream& out ) const
{
  out << "(bowner :players "
      << " (list " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out << "))";
}




void
CondBallOwner::printPretty( ostream& out, 
			    const string& line_header ) const
{
  out << line_header
      << "is ball controlled by someone in " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out << std::endl;
}

Cond* CondBallOwner::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*> &players){
 rule<> cond_p
    = *space_p >> str_p("(bowner") >> *space_p >> ":players";
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    //Must parse players. Amount is not pre-defined
    // To store the players
    vector<PlayerReference*> players_
      = PlayerReference::parsePlayerList(in.substr(result.length),
					 rest,parameters,players);
      
    if(players_.size()>0){
      //Check for the final '))'
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					      *space_p>>')',
					      nothing_p);
      if(result.hit){
	out=rest.substr(result.length);
	return new CondBallOwner(PlayerReference::toVectorConstPlayerReference(players_));
      }
    }
  }
  out=in;
  return NULL;
}

Cond* CondBallOwner::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_new= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondBallOwner(*players_new);
}

Cond* CondBallOwner::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondBallOwner(*players_new);
}

CondClearPass::CondClearPass(const vector<const PlayerReference*>& players)
	:Cond(){
  // if there is a single player and its number is 0, it means that
  // any player is meant, and all players
  // from 1 to 11 will be inserted into the list.
  // if team number is "any", then this will be made with both teams
  if(players.size()==1 && !players.at(0)->isRole()
     && players.at(0)->value()->number==0){
    //create a new vector
    vector<const PlayerReference*> *allPlayers = new vector<const PlayerReference*>;
    const PlayerReference* player=players.at(0);
    
    if(player->value()->team=="our"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("our",i));
    if(player->value()->team=="opp"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("opp",i));
    
    // set the class attribute
    m_players_from= *allPlayers;
  }
  else
    m_players_from= players;
}

void
CondClearShotAtGoal::print( ostream& out ) const
{


  out << "(canShoot :players ";
  CondClearPass::print(out);
  out << ")";
}

void
CondClearShotAtGoal::printPretty( ostream& out, 
				  const string& line_header ) const
{
  out << line_header
      << "does someone in " ;
  for(unsigned int i = 0; i!= getFromPlayerSet().size(); i++){
    getFromPlayerSet().at(i)->print(out);
    out<<" have a clear shot at goal";
  }
  out << std::endl;
}

Cond* CondClearShotAtGoal::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 rule<> cond_p
    = *space_p >> str_p("(canShoot") >> *space_p >> ":players" 
	       >> *space_p ;
 
 parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						      nothing_p);
 
 if(result.hit){
   string rest;
   //Must parse players. Amount is not pre-defined
 
   // To store the players
   vector<PlayerReference*> players_
     = PlayerReference::parsePlayerList(in.substr(result.length), rest,
					parameters,players);
   
   //Check for the final ')'
   result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					   *space_p>>')',
					   nothing_p);
   if(result.hit){
     out=rest.substr(result.length);
     return new CondClearShotAtGoal(PlayerReference::toVectorConstPlayerReference(players_));
   }
 }

 out=in;
 return NULL;
}

Cond* CondClearShotAtGoal::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_new= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=getFromPlayerSet().size();i++)
    players_new->push_back((PlayerReference*)getFromPlayerSet().at(i)
			   ->deepCopy(params,players));
  
  return new CondClearShotAtGoal(*players_new);
}

Cond* CondClearShotAtGoal::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=getFromPlayerSet().size();i++)
    players_new->push_back((PlayerReference*)getFromPlayerSet().at(i)
			   ->deepCopy(params,players));
  
  return new CondClearShotAtGoal(*players_new);
}

void
CondClearPass::print( ostream& out ) const{
  out << " (list " ;
  for(unsigned int i = 0; i!= m_players_from.size(); i++){
    m_players_from.at(i)->print(out);
    out<<" ";
  }
  out << ")";
}

void CondClearPass::printPretty( ostream& out, 
			    const string& line_header ) const{
  out << line_header
      << "does someone in " ;
  for(unsigned int i = 0; i!= m_players_from.size(); i++)
    m_players_from.at(i)->print(out);
}



void CondPlayMode::printPretty( ostream& out, const string& line_header ) const
{  out << line_header << "is play mode '"
       << rcss::clang::MODE_STRINGS[ M_pm ] << "'" << std::endl; }

rcss::clang::PlayMode*
CondPlayMode::parsePlayMode(const string& modeAsString){
  for(int i=0; i!= rcss::clang::MODE_STRINGS_LENGTH; i++)
    if(strcmp(rcss::clang::MODE_STRINGS[i],modeAsString.c_str())==0)
      return new rcss::clang::PlayMode((rcss::clang::PlayMode)i);

  cerr<<"SP: failed parsisng play mode:"<<modeAsString<<endl;
  return NULL;
}

Cond* CondPlayMode::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string mode;
  
  rule<> cond_p
    = *space_p >> str_p("(playm") >> *space_p >> identifier_p[assign_a(mode)]
	       >> *space_p >> ")" ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);
  
  if(result.hit){
    out=in.substr(result.length);
    return new CondPlayMode(*parsePlayMode(mode));
  }
  out=in;
  return NULL;
}

CondClearPassToPlayer::CondClearPassToPlayer(const vector<const PlayerReference*>& 
					     players_from,
					     const vector<const PlayerReference*>&
					     players_to)
 :CondClearPass(players_from){
  // if there is a single player and its number is 0, it means that
  // any player is meant, and all players
  // from 1 to 11 will be inserted into the list.
  // if team number is "any", then this will be made with both teams
  if(players_to.size()==1 && !players_to.at(0)->isRole()
     && players_to.at(0)->value()->number==0){
    //create a new vector
    vector<const PlayerReference*> *allPlayers = new vector<const PlayerReference*>;
    const PlayerReference* player=players_to.at(0);
    
    if(player->value()->team=="our"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("our",i));
    if(player->value()->team=="opp"||player->value()->team=="any")
      for(unsigned int i = 1; i!=12; i++)
	allPlayers->push_back(new PlayerID("opp",i));
    
    // set the class attribute
    m_players_to= *allPlayers;
  }
  else
    m_players_to= players_to;
}

void
CondClearPassToPlayer::print( ostream& out ) const{
  out<<"(canPassPl :from ";
  CondClearPass::print(out);
  out << " :to (list " ;
  for(unsigned int i = 0; i!= m_players_to.size(); i++){
    m_players_to.at(i)->print(out);
    out<<" ";
  }
  out << "))";
}

void
CondClearPassToPlayer::printPretty( ostream& out, 
				  const string& line_header ) const
{
  CondClearPass::printPretty(out,line_header);
  
  out<<" have a clear pass line to someone in" << std::endl;
  for(unsigned int i = 0; i!= m_players_to.size(); i++)
    m_players_to.at(i)->print(out);
}

Cond* CondClearPassToPlayer::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 rule<> cond_p
    = *space_p >> str_p("(canPassPl") >> *space_p >> ":from";
 
 parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						      nothing_p);
 
 if(result.hit){
   string rest=in.substr(result.length);
   //Must parse :from players. Amount is not pre-defined
   
   // To store the players
   vector<PlayerReference*> players_
     = PlayerReference::parsePlayerList(rest,rest,parameters,players);

   // There must be at least one...
   if(players_.size()>0){
     result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					   *space_p>>":to",
					     nothing_p);
   
     if(result.hit){
       //Must parse :to players. Amount is not pre-defined
       
       // To store the players
       vector<PlayerReference*> players2
	 =PlayerReference::parsePlayerList(rest.substr(result.length),
					   rest,parameters,players);
       // There must be at least one...
       if(players2.size()>0){
	 //Check for the final ')'
	 result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
						 *space_p>>')',
						 nothing_p);
	 if(result.hit){
	   out=rest.substr(result.length);
	   return new CondClearPassToPlayer(PlayerReference::toVectorConstPlayerReference(players_),
			   PlayerReference::toVectorConstPlayerReference(players2));
	 }
       }
     }
   }
 }
 out=in;
 return NULL;
}



Cond* CondClearPassToPlayer::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_from= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=m_players_from.size();i++)
    players_from->push_back((PlayerReference*)m_players_from.at(i)
			    ->deepCopy(params,players));


  vector<const PlayerReference*> *players_to= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=m_players_to.size();i++)
    players_to->push_back((PlayerReference*)m_players_to.at(i)
			  ->deepCopy(params,players));
  
  return new CondClearPassToPlayer(*players_from,*players_to);
}


Cond* CondClearPassToPlayer::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_from= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=m_players_from.size();i++)
    players_from->push_back((PlayerReference*)m_players_from.at(i)
			    ->deepCopy(params,players));


  vector<const PlayerReference*> *players_to= new vector<const PlayerReference*>();
  for(unsigned int i = 0; i!=m_players_to.size();i++)
    players_to->push_back((PlayerReference*)m_players_to.at(i)
			  ->deepCopy(params,players));
  
  return new CondClearPassToPlayer(*players_from,*players_to);
}


void
CondClearPassToRegion::print( ostream& out ) const
{
  out<<"(canPassReg :from ";
  CondClearPass::print(out);
  out << " :to " ;
  m_region_to->print(out);
  out << ")";
}

void
CondClearPassToRegion::printPretty( ostream& out, 
				  const string& line_header ) const
{
  CondClearPass::printPretty(out,line_header);
  
  out<<" have a clear pass line to " << std::endl;
  m_region_to->print(out);
}

Cond* CondClearPassToRegion::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
rule<> cond_p
    = *space_p >> str_p("(canPassReg") >> *space_p >> ":from" ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest=in.substr(result.length);
    //Must parse :from players. Amount is not pre-defined
   
    // To store the players
    vector<PlayerReference*> players_
      = PlayerReference::parsePlayerList(rest,rest,parameters,players);

    // There must be at least one...
    if(players_.size()>0){
      
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					      *space_p>>":to",
					      nothing_p);
      
      if(result.hit){
	//Must parse :to region. Amount is not pre-defined
	//Will parse the first one, is mandatory
	Region* reg= Region::parse(rest.substr(result.length), rest,
				   parameters,players);
	
	if(reg){
	  //Check for the final ')'
	  result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
						  nothing_p);
	  if(result.hit){
	    out=rest.substr(result.length);
	    return new CondClearPassToRegion(PlayerReference::toVectorConstPlayerReference(players_),reg);
	  }
	}
      }
    }
  }
  out=in;
  return NULL;
}

Cond* CondClearPassToRegion::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_from= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players_from.size();i++)
    players_from->push_back((PlayerReference*)m_players_from.at(i)
			    ->deepCopy(params,players));

  return new CondClearPassToRegion(*players_from,
				   m_region_to->deepCopy(params,players));
}

Cond* CondClearPassToRegion::inversion(const vector<SetplayParameter*>& params,
				      const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*> *players_from= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players_from.size();i++)
    players_from->push_back((PlayerReference*)m_players_from.at(i)
			    ->inversion(params,players));

  return new CondClearPassToRegion(*players_from,
				   m_region_to->inversion(params,players));
}


CondAnd::~CondAnd(){
  for( Storage::iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    delete *i;
  m_conds.clear();
}


void
CondAnd::print( ostream& out ) const
{
  out << "(and";
  for( Storage::const_iterator iter = getConds().begin();
       iter != getConds().end(); ++iter )
      if( *iter == NULL )
	out << " (null)";
      else{
	out << " ";
	(*iter)->print(out);
      }
  out << ")";
}

void
CondAnd::printPretty( ostream& out, 
		      const string& line_header ) const
{
  out << line_header << "and" << std::endl;
  for( Storage::const_iterator iter = getConds().begin(); 
       iter != getConds().end(); ++iter )
    {
      if( *iter == NULL )
	out << line_header << " +(null)\n";
      else
	(*iter)->printPretty( out, line_header + " +" );
    }
}

Cond* CondAnd::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 rule<> cond_p
    = *space_p >> str_p("(and") >> *space_p ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    //Must parse conds. Amount is not pre-defined
    //Will parse the first one, is mandatory
    Cond* cond= 
      Cond::parse(in.substr(result.length), rest,parameters,players);

    if(cond){
      // To store the players
      list<Cond*>* conds = new list<Cond*>;
      
      while(cond){
	conds->push_back(cond);
	
	//Parse a new player
	cond= Cond::parse(rest, rest,parameters,players);
      }

      //Check for the final ')'
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					      nothing_p);
      if(result.hit){
	out=rest.substr(result.length);
	return new CondAnd(*conds);
      }
    }
  }
  out=in;
  return NULL;
}

bool
CondAnd::eval( const Context& context ) const
{
  for( Storage::const_iterator iter = getConds().begin(); 
       iter != getConds().end(); ++iter )
    {
      if( *iter == NULL )
	throw rcss::util::NullErr( __FILE__, __LINE__,
			     "Null condition in CondAnd\n" );
      if( !(*iter)->eval( context ) )
	return false;
    }
  return true;
}

Cond* CondAnd::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  CondAnd::Storage conds;
  for(  CondAnd::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    conds.push_back((*i)->deepCopy(params,players));
  
  return new CondAnd(conds);
}


Cond* CondAnd::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  CondAnd::Storage conds;
  for(  CondAnd::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    conds.push_back((*i)->inversion(params,players));
  
  return new CondAnd(conds);
}

void CondAnd::substituteNamedRegions(const Context& world){
  
  for(  CondAnd::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    (*i)->substituteNamedRegions(world);
  
}


CondOr::~CondOr(){	  
  for( Storage::iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    delete *i;
  m_conds.clear();
}

void
CondOr::print( std::ostream& out ) const
{
  out << "(or";
  for( Storage::const_iterator iter = getConds().begin(); 
       iter != getConds().end(); ++iter) 
    if( *iter == NULL )
      out << " (null)";
    else{
      out << " ";
      (*iter)->print(out);
    }
  out << ")";
}

void
CondOr::printPretty( std::ostream& out, const std::string& line_header ) const
{
  out << line_header << "or" << std::endl;
  for( Storage::const_iterator iter = getConds().begin();
       iter != getConds().end(); ++iter )
    {
      if( *iter == NULL )
	out << line_header << " +(null)\n";
      else
	(*iter)->printPretty( out, line_header + " +" );
    }
}

Cond* CondOr::parse(const string& in, string& out,
		const vector<SetplayParameter*> &parameters,
		const vector<PlayerReference*>& players){
rule<> cond_p
    = *space_p >> str_p("(or") >> *space_p ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    //Must parse conds. Amount is not pre-defined
    //Will parse the first one, is mandatory
    Cond* cond= 
      Cond::parse(in.substr(result.length), rest,parameters,players);

    if(cond){
      // To store the players
      list<Cond*>* conds = new list<Cond*>;
      
      while(cond){
	conds->push_back(cond);
	
	//Parse a new player
	cond= Cond::parse(rest, rest,parameters,players);
      }

      //Check for the final ')'
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					      nothing_p);
      if(result.hit){
	out=rest.substr(result.length);
	return new CondOr(*conds);
      }
    }
  }
  out=in;
  return NULL;
}

bool
CondOr::eval( const Context& context ) const
{
  for( Storage::const_iterator iter = getConds().begin();
       iter != getConds().end(); ++iter )
    {
      if( *iter == NULL )
	throw rcss::util::NullErr( __FILE__, __LINE__, 
			     "Null condition in CondOr\n" );
      if( (*iter)->eval( context ) )
	return true;
    }
  return false;
}

Cond* CondOr::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  CondOr::Storage conds;
  for(  CondOr::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    conds.push_back((*i)->deepCopy(params,players));
  
  return new CondOr(conds);
}

Cond* CondOr::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  CondOr::Storage conds;
  for(  CondOr::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    conds.push_back((*i)->inversion(params,players));
  
  return new CondOr(conds);
}

void CondOr::substituteNamedRegions(const Context& world){
  for(  CondOr::Storage::const_iterator i = m_conds.begin();
       i != m_conds.end(); ++i )
    (*i)->substituteNamedRegions(world);
}




 void CondNot::print( std::ostream& out ) const { 
   out << "(not ";
   if( getCond() == NULL )
     out << "(null)\n";
   else
     getCond()->print(out);
   out << ")";
 }

void CondNot::printPretty( std::ostream& out, const std::string& line_header ) const{ 
  out << line_header << "not " << std::endl;
  if( getCond() == NULL )
    out << line_header << " (null)\n";
  else
    getCond()->printPretty( out, line_header + " +" );
}

Cond* CondNot::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
rule<> cond_p
    = *space_p >> str_p("(not") >> *space_p ;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    //Must parse conds. Amount is not pre-defined
    //Will parse the first one, is mandatory
    Cond* cond= 
      Cond::parse(in.substr(result.length), rest,parameters,players);

    if(cond){
      //Check for the final ')'
      result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					      nothing_p);
      if(result.hit){
	out=rest.substr(result.length);
	return new CondNot(cond);
      }
    }
  }
  out=in;
  return NULL;
}

bool CondNot::eval( const Context& context ) const{
  if( getCond() == NULL )
    throw rcss::util::NullErr( __FILE__, __LINE__, 
			       "Null condition in CondNot\n" );
  else
    return !( getCond()->eval( context ) );
}

Cond* CondOurGoal::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string op;
  int value;

  rule<> cond_p
    = *space_p >> str_p("(our_goals") >> *space_p >> comparison_p[assign_a(op)]
	       >> *space_p >> int_p[assign_a(value)] >> *space_p >> ')';
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);
  
  if(result.hit){
    out=in.substr(result.length);
    if(op=="<") return new CondOurGoal(value,rcss::util::CompOp::less());
    if(op=="<=") return new CondOurGoal(value,rcss::util::CompOp::lessEqual());
    if(op=="==") return new CondOurGoal(value,rcss::util::CompOp::equal());
    if(op=="!=") return new CondOurGoal(value,rcss::util::CompOp::notEqual());
    if(op==">=") return new CondOurGoal(value,rcss::util::CompOp::greaterEqual());
    if(op==">") return new CondOurGoal(value,rcss::util::CompOp::greater());
    }
  out=in;
  return NULL;
}

Cond* CondGoalDiff::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string op;
  int value;

  rule<> cond_p
    = *space_p >> str_p("(goal_diff") >> *space_p >> comparison_p[assign_a(op)]
	       >> *space_p >> int_p[assign_a(value)] >> *space_p >> ')';
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);
  
  if(result.hit){
    out=in.substr(result.length);
    if(op=="<") return new CondGoalDiff(value,rcss::util::CompOp::less());
    if(op=="<=") return new CondGoalDiff(value,rcss::util::CompOp::lessEqual());
    if(op=="==") return new CondGoalDiff(value,rcss::util::CompOp::equal());
    if(op=="!=") return new CondGoalDiff(value,rcss::util::CompOp::notEqual());
    if(op==">=") return new CondGoalDiff(value,rcss::util::CompOp::greaterEqual());
    if(op==">") return new CondGoalDiff(value,rcss::util::CompOp::greater());
    }
  out=in;
  return NULL;
}


Cond* CondOppGoal::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string op;
  int value;

  rule<> cond_p
    = *space_p >> str_p("(opp_goals") >> *space_p >> comparison_p[assign_a(op)]
	       >> *space_p >> int_p[assign_a(value)] >> *space_p >> ')';
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);
  
  if(result.hit){
    out=in.substr(result.length);
    if(op=="<") return new CondOppGoal(value,rcss::util::CompOp::less());
    if(op=="<=") return new CondOppGoal(value,rcss::util::CompOp::lessEqual());
    if(op=="==") return new CondOppGoal(value,rcss::util::CompOp::equal());
    if(op=="!=") return new CondOppGoal(value,rcss::util::CompOp::notEqual());
    if(op==">=") return new CondOppGoal(value,rcss::util::CompOp::greaterEqual());
    if(op==">") return new CondOppGoal(value,rcss::util::CompOp::greater());
    }
  out=in;
  return NULL;
}

Cond* CondTime::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string op;
  int value;

  rule<> cond_p
    = *space_p >> str_p("(time") >> *space_p >> comparison_p[assign_a(op)]
	       >> *space_p >> int_p[assign_a(value)] >> *space_p >> ')';
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);
  
  if(result.hit){
    out=in.substr(result.length);
    if(op=="<") return new CondTime(value,rcss::util::CompOp::less());
    if(op=="<=") return new CondTime(value,rcss::util::CompOp::lessEqual());
    if(op=="==") return new CondTime(value,rcss::util::CompOp::equal());
    if(op=="!=") return new CondTime(value,rcss::util::CompOp::notEqual());
    if(op==">=") return new CondTime(value,rcss::util::CompOp::greaterEqual());
    if(op==">") return new CondTime(value,rcss::util::CompOp::greater());
    }
  out=in;
  return NULL;
}

CondNearOffsideLine::CondNearOffsideLine(const vector<const PlayerReference*>& players)
      : Cond(){
      // if there is a single player and its number is 0, it means that
      // any player is meant, and all players
      // from 1 to 11 will be inserted into the list.
      // if team number is "any", then this will be made with both teams
      if(players.size()==1 && !players.at(0)->isRole()
	 && players.at(0)->value()->number==0){
	//create a new vector
	vector<const PlayerReference*> *allPlayers = new vector<const PlayerReference*>;
	const PlayerReference* player=players.at(0);
	
	if(player->value()->team=="our"||player->value()->team=="any")
	  for(unsigned int i = 1; i!=12; i++)
	    allPlayers->push_back(new PlayerID("our",i));
	if(player->value()->team=="opp"||player->value()->team=="any")
	  for(unsigned int i = 1; i!=12; i++)
	    allPlayers->push_back(new PlayerID("opp",i));

	// set the class attribute
	m_players= *allPlayers;
      }
      else
	m_players= players ;
}

void
CondNearOffsideLine::print( ostream& out ) const
{
  out << "(nearOffsideLine :players (list " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out<< "))";
}

void
CondNearOffsideLine::printPretty( ostream& out, 
			    const string& line_header ) const
{
  out << line_header
      << "are (list " ;
  for(unsigned int i = 0; i!= m_players.size(); i++){
    m_players.at(i)->print(out);
    out<<" ";
  }
  out << ") near the Offside Line?" << std::endl;
}


Cond* CondNearOffsideLine::parse(const string& in, string& out,
		const vector<SetplayParameter*> &parameters,
		const vector<PlayerReference*>& players){
  rule<> cond_p
    = *space_p >> str_p("(nearOffsideLine") >> *space_p 
	       >> ":players" >> *space_p;
  
  parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),cond_p,
						       nothing_p);

  if(result.hit){
    string rest;
    
    // To store the players
    vector<PlayerReference*> players_cond
      = PlayerReference::parsePlayerList(in.substr(result.length),rest,
					 parameters,players);
    
    
    //Check for the final ')'
    result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					    nothing_p);
    if(result.hit){
      out=rest.substr(result.length);
      return new CondNearOffsideLine(PlayerReference::toVectorConstPlayerReference(players_cond));
    }
  }
  out=in;
  return NULL;
}

Cond* CondNearOffsideLine::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondNearOffsideLine(*players_new);
}

Cond* CondNearOffsideLine::inversion(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const{
  vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;
  for(unsigned int i = 0; i!=m_players.size();i++)
    players_new->push_back((PlayerReference*)m_players.at(i)
			   ->deepCopy(params,players));
  
  return new CondNearOffsideLine(*players_new);
}
