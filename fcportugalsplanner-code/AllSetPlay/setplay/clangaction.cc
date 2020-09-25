// -*-c++-*-

/***************************************************************************
                                clangaction.cc  
                       Abstract base class for CLang Actions
                             -------------------
    begin                : 28-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server 
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include <setplay/clangaction.h>
#include "simpleParsers.h"

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;

#define THRESHOLD_MARK 3.0

using namespace fcportugal::setplay;


Action::~Action()
{}

Action* Action::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*> &players){
	Action* res = NULL;

	res=ActPos::parse(in,out,parameters,players);
	if(res) return res;

	res=ActForward::parse(in,out,parameters,players);
	if(res) return res;

	res=ActPass::parse(in,out,parameters,players);
	if(res) return res;


	res=ActMark::parse(in,out,parameters,players);
	if(res) return res;


	res=ActMarkLinePlayer::parse(in,out,parameters,players);
	if(res) return res;


	res=ActMarkLineReg::parse(in,out,parameters,players);
	if(res) return res;


	res=ActDribble::parse(in,out,parameters,players);
	if(res) return res;


	res=ActOffsideLine::parse(in,out,parameters,players);
	if(res) return res;


	res=ActClear::parse(in,out,parameters,players);
	if(res) return res;


	res=ActShoot::parse(in,out,parameters,players);
	if(res) return res;


	res=ActHold::parse(in,out,parameters,players);
	if(res) return res;


	res=ActIntercept::parse(in,out,parameters,players);
	if(res) return res;


	res=ActTackle::parse(in,out,parameters,players);
	if(res) return res;


	res=ActSeq::parse(in,out,parameters,players);
	if(res) return res;


	res=ActStop::parse(in,out,parameters,players);
	if(res) return res;


	res=ActAttentionToReg::parse(in,out,parameters,players);
	if(res) return res;


	res=ActAttentionToObj::parse(in,out,parameters,players);
	if(res) return res;


	res=ActMarkGoal::parse(in,out,parameters,players);
	if(res) return res;

	res=ActReceiveBall::parse(in,out,parameters,players);
	if(res) return res;

	res=ActMoveToOffsideLine::parse(in,out,parameters,players);
	if(res) return res;

	return NULL;
}



vector<Action*>* 
Action::parseActionList(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	//check for list label
	rule<> list_p
	= *space_p >> str_p("(list") ;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),list_p,
			nothing_p);

	if(result.hit){
		string rest;

		Action* act= Action::parse(in.substr(result.length), rest,
				parameters,players);
		if(!act){
			//empty list!?!
					cerr<<"\n empty action list!\n";
					out=in;
					return NULL;
		}

		// To store the actions
		vector<Action*>* acts = new vector<Action*>;

		while(act){
			acts->push_back(act);

			//Parse a new action
			act=Action::parse(rest,rest,parameters,players);
		}

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				*space_p>>')',
				nothing_p);
		if(result.hit){
			out=rest.substr(result.length);
			return acts;
		}
	}
	else{
		//try to parse single action
		string rest;

		Action* act=
				Action::parse(in, rest,parameters,players);
		if(act){
			vector<Action*>* acts = new vector<Action*>;
			acts->push_back(act);

			out=rest;
			return acts;
		}
	}

	out=in;
	return NULL;
}



ActPos::ActPos(const Region* const r )
: Action(),m_reg( r )
{}

ActPos::~ActPos() 
{}

void
ActPos::print( std::ostream& out ) const
{ 
	out << "(pos :region ";
	if( m_reg == NULL )
		out << "(null)";
	else
		m_reg->print(out);
	out << ")";
}

void
ActPos::printPretty( std::ostream& out, 
		const std::string& line_header ) const
{ 
	out << line_header << "position self at: " << std::endl;
	if( m_reg == NULL )
		out << line_header << " (null)\n";
	else
		m_reg->printPretty( out, line_header + " " );
}

Action* ActPos::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(pos") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse region
		Region* reg= Region::parse(in.substr(result.length),rest,parameters,players);

		if(reg){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActPos(reg);
			}
		}
	}
	out=in;
	return NULL;
}

const Action* ActPos::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}


bool ActPos::isAccomplished(const PlayerID& p,const Context& world)const{
	vector<const PlayerReference*> vectorMe;
	vectorMe.push_back(new PlayerID(p));
	CondPlayerPos* accomplishedCond
	=new CondPlayerPos(vectorMe,m_reg,
			new Integer(1),new Integer(1));
	accomplishedCond->substituteNamedRegions(world);

	return accomplishedCond->eval(world);
}

const Region* ActPos::getRegion() const
{ return m_reg; }


ActForward::ActForward( const Region* const reg, 
		const BallMoveType& bm )
: Action(),
  m_reg(reg),
  m_bm( bm )
{}

ActForward::~ActForward()
{}


void
ActForward::print( std::ostream& out ) const 
{
	out << "(bto :region ";
	if( m_reg == NULL )
		out << "(null)";
	else
		m_reg->print(out);

	switch(m_bm){
	case fastBallMove:
		out<<" :type fast";
		break;
	case slowBallMove:
		out<<" :type slow";
		break;
	case undefinedBallMove:
	case normalBallMove:
		out<<" :type normal";
		break;
	}
	out<< ")";
}

void
ActForward::printPretty( std::ostream& out,
		const std::string& line_header ) const
{
	out << line_header << "ball to: use " ;

	switch(m_bm){
	case fastBallMove:
		out<<" fast";
		break;
	case slowBallMove:
		out<<" slow";
		break;
	case undefinedBallMove:
	case normalBallMove:
		out<<" normal";
	}

	out<< " to go to:" << std::endl;
	if( m_reg == NULL )
		out << line_header << " (null)\n";
	else
		m_reg->printPretty( out, line_header + " " );

}

Action* ActForward::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(bto") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse region
		Region* reg= Region::parse(in.substr(result.length), rest,parameters,players);

		//Check for type
		string type_;
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				*space_p>>":type">>*space_p
				>>identifier_p[assign_a(type_)],
				nothing_p);


		BallMoveType typeAttrib=undefinedBallMove;
		if(result.hit){


			if(type_=="normal") typeAttrib=normalBallMove;
			else if(type_=="fast") typeAttrib=fastBallMove;
			else if(type_=="slow") typeAttrib=slowBallMove;
			else{
				return NULL;
			}
			rest=rest.substr(result.length);
		}

		if(reg){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActForward(reg,typeAttrib);
			}
		}
	}
	out=in;
	return NULL;
}

const Region* ActForward::getRegion() const
{ return m_reg; }

fcportugal::setplay::BallMoveType
ActForward::ballMoveType() const
{ return m_bm; }


const Action* ActForward::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActForward::isAccomplished(const PlayerID& p,
		const Context& world) const{

	CondBallPos* accomplishedCond=new CondBallPos(m_reg);
	accomplishedCond->substituteNamedRegions(world);
	return accomplishedCond->eval(world);
}

//ActPass::ActPass():Action(),type(undefinedBallMove){}

ActPass::ActPass(const vector<const PlayerReference*>& players,BallMoveType type_ )
:Action(),M_players( players ),type(type_){}

ActPass::~ActPass(){}


void
ActPass::print( std::ostream& out ) const
{ out << "(bto :players (list ";
for(unsigned int i = 0; i!= M_players.size(); i++){
	M_players.at(i)->print(out);
	out<<" ";
}
out<< ")";

switch(type){
case normalBallMove: out<<" :type normal";
break;
case fastBallMove: out<<" :type fast";
break;
case slowBallMove:
case undefinedBallMove:
	out<<" :type slow";
	break;
}
out<< ")";
}

void
ActPass::printPretty( std::ostream& out, 
		const std::string& line_header) const
{ 
	out << line_header << "ball to players: " ;
	for(unsigned int i = 0; i!= M_players.size(); i++){
		M_players.at(i)->print(out);
		out<<" ";
	}

	switch(type){
	case normalBallMove: out<<" :type normal";
	break;
	case fastBallMove: out<<" :type fast";
	break;
	case slowBallMove:
	case undefinedBallMove:
		out<<" :type slow";
	}
	out << std::endl;
}



const vector<const PlayerReference*>& ActPass::relevantPlayers() const{
	return M_players;
}

Action* ActPass::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(bto") >> *space_p >> ":players";

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse players. Amount is not pre-defined
		//Will parse the firs one, is mandatory
		// To store the players
		vector<PlayerReference*> players_ =
				PlayerReference::parsePlayerList(in.substr(result.length), rest,
						parameters,players);
		if(players_.size()>0){
			//Check for type
			string type_;
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>":type">>*space_p
					>>identifier_p[assign_a(type_)],
					nothing_p);


			BallMoveType typeAttrib=undefinedBallMove;
			if(result.hit){
				if(type_=="normal") typeAttrib=normalBallMove;
				else if(type_=="fast") typeAttrib=fastBallMove;
				else if(type_=="slow") typeAttrib=slowBallMove;
				else{
					return NULL;
				}
				rest=rest.substr(result.length);
			}


			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				// create const vector

				return new ActPass(PlayerReference::toVectorConstPlayerReference(players_),typeAttrib);
			}
			else
				cerr<<"SP: ununderstood rest in pass parse:"<<rest<<endl;
		}
	}
	out=in;
	return NULL;
}

const vector<const PlayerReference*>&
ActPass::getPlayers() const {
	return M_players; }


const Action* ActPass::necessaryAction(const PlayerID& p,const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActPass::isAccomplished(const PlayerID& p,
		const Context& world) const{
	CondBallOwner* accomplishedCond=new CondBallOwner(M_players);

	accomplishedCond->substituteNamedRegions(world);
	return accomplishedCond->eval(world);
}

Action* ActPass::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;

	for(unsigned int i =0; i!= M_players.size();i++)
		players_new->push_back((PlayerReference*)M_players.at(i)
				->deepCopy(params,players));
	return new ActPass(*players_new,type);
}

Action* ActPass::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	return deepCopy(params,players);
}


ActMark::ActMark(const vector<const PlayerReference*>& players):Action(),M_players( players ){}

ActMark::~ActMark()
{}


void
ActMark::print( std::ostream& out ) const{
	out << "(mark :players (list ";
	for(unsigned int i = 0; i!= M_players.size(); i++){
		M_players.at(i)->print(out);
		out<<" ";
	}
	out << "))";
}

void
ActMark::printPretty( std::ostream& out, const std::string& line_header ) const{
	out << line_header;
	print(out);
	out << std::endl; }

const vector<const PlayerReference*>& ActMark::relevantPlayers() const{
	return M_players;
}


Action* ActMark::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(mark") >> *space_p >> ":players" ;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse players. Amount is not pre-defined
		// To store the players
		vector<PlayerReference*> players_=
				PlayerReference::parsePlayerList(in.substr(result.length), rest,
						parameters,players);
		if(players_.size()>0){
			//Check for the final '))'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);

				return new ActMark(PlayerReference::toVectorConstPlayerReference(players_));
			}
		}
	}
	out=in;
	return NULL;
}

const vector<const PlayerReference*>& 
ActMark::getPlayers() const{
	return M_players; }

const Action* ActMark::necessaryAction(const PlayerID& p,const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActMark::isAccomplished(const PlayerID& p,const Context& world) const{
	for(unsigned int i=0;i!=M_players.size();i++){
		vector<const PlayerReference*>* vec=new vector<const PlayerReference*>;
		vec->push_back(new PlayerID(p));
		Cond* c=new CondPlayerPos(*vec,
				new RegArc(new PointPlayer(M_players.at(i)),
						new Decimal(0),
						new Decimal(THRESHOLD_MARK)),
						new Integer(1),new Integer(1));

		if(c->eval(world))
			return true;
	}
	return false;
}

Action* ActMark::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;

	for(unsigned int i =0; i!= M_players.size();i++)
		players_new->push_back((PlayerReference*)M_players.at(i)
				->deepCopy(params,players));
	return new ActMark(*players_new);
}

Action* ActMark::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	return deepCopy(params,players);
}

ActMarkLinePlayer::ActMarkLinePlayer(const vector<const PlayerReference*>& players ) 
: Action(),M_players( players ){}

ActMarkLinePlayer::~ActMarkLinePlayer(){}

void
ActMarkLinePlayer::print( std::ostream& out ) const
{ 
	out << "(markl :players (list " ;
	for(unsigned int i = 0; i!= M_players.size();i++){
		M_players.at(i)->print(out);
		out<<" ";
	}
	out << "))"; }

void
ActMarkLinePlayer::printPretty( std::ostream& out,
		const std::string& line_header ) const{
	out << line_header
			<< "mark line to opponent ";
	for(unsigned int i = 0; i!= M_players.size();i++){
		M_players.at(i)->print(out);
		out<<" ";
	}
	out<< std::endl;
}

const vector<const PlayerReference*>& ActMarkLinePlayer::relevantPlayers() const{
	return M_players;
}

Action* ActMarkLinePlayer::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(markl") >> *space_p >> ":players";

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse players. Amount is not pre-defined
		// To store the players
		vector<PlayerReference*> players_
		= PlayerReference::parsePlayerList(in.substr(result.length), rest,
				parameters,players);

		if(players_.size()>0){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActMarkLinePlayer(PlayerReference::toVectorConstPlayerReference(players_));
			}
		}
	}
	out=in;
	return NULL;
}

const vector<const PlayerReference*>&
ActMarkLinePlayer::getPlayers() const 
{ return M_players; }



Action* ActMarkLinePlayer::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>;

	for(unsigned int i =0; i!= M_players.size();i++)
		players_new->push_back((PlayerReference*)M_players.at(i)
				->deepCopy(params,players));
	return new ActMarkLinePlayer(*players_new);
}

Action* ActMarkLinePlayer::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	return deepCopy(params,players);
}

const Action* ActMarkLinePlayer::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

const Action* ActMarkLineReg::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

void ActMarkLineReg::print( std::ostream& out ) const{ 
	out << "(markl :region ";
	if( getRegion() == NULL )
		out << "(null)";
	else
		getRegion()->print(out);
	out << ")";
}

void ActMarkLineReg::printPretty( std::ostream& out, 
		const std::string& line_header ) const{
	out << line_header << "mark line to region:" << std::endl;
	if( getRegion() == NULL )
		out << line_header << " (null)\n";
	else
		getRegion()->printPretty( out, line_header + " " );
}

Action* ActMarkLineReg::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(markl") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse region
		Region* reg= Region::parse(in.substr(result.length), rest,parameters,players);

		if(reg){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActMarkLineReg(reg);
			}
		}
	}
	out=in;
	return NULL;
}


const Action* ActOffsideLine::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

void ActOffsideLine::print( std::ostream& out ) const{ 
	out << "(oline :region ";
	if( getRegion() == NULL )
		out << "(null)";
	else
		getRegion()->print(out);
	out << ")";
}

void ActOffsideLine::printPretty( std::ostream& out, 
		const std::string& line_header ) const{
	out << line_header << "offsides line at: " << std::endl;
	if( getRegion() == NULL )
		out << line_header << " (null)\n";
	else
		getRegion()->printPretty( out, line_header + " " );
}


Action* ActOffsideLine::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(oline") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
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
				return new ActOffsideLine(reg);
			}
		}
	}
	out=in;
	return NULL;
}

void ActClear::print( std::ostream& out ) const{ 
	out << "(clear :region ";
	if( getRegion() == NULL )
		out << "(null)";
	else
		getRegion()->print(out);
	out << ")";
}

void ActClear::printPretty(std::ostream& out, 
		const std::string& line_header) const{
	out << line_header << "clear to region:" << std::endl;
	if( getRegion() == NULL )
		out << line_header << " (null)\n";
	else
		getRegion()->printPretty( out, line_header + " " );
}

Action* ActClear::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(clear") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse region
		Region* reg= Region::parse(in.substr(result.length), rest,
				parameters,players);

		if(reg){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActClear(reg);
			}
		}
	}
	out=in;
	return NULL;
}


const Action* ActClear::necessaryAction(const PlayerID& p,const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActClear::isAccomplished(const PlayerID& p,const Context& world) const{
	CondNot* accomplishedCond=new CondNot(new CondBallPos(m_reg));
	return accomplishedCond->eval(world);
}

const Action* ActHold::necessaryAction(const PlayerID& p,const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActHold::isAccomplished(const PlayerID& p,const Context& world) const{
	vector<const PlayerReference*>* playerVec=new vector<const PlayerReference*>();
	playerVec->push_back(new PlayerID(p));
	CondBallOwner* accomplishedCond
	=new CondBallOwner(*playerVec);


	accomplishedCond->substituteNamedRegions(world);

	return accomplishedCond->eval(world);
}

Action* ActHold::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(hold") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActHold();
	}
	out=in;
	return NULL;
}

const Action* ActDribble::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActDribble::isAccomplished(const PlayerID& p,
		const Context& world) const{
	CondBallPos* accomplishedCond=new CondBallPos(m_reg);

	accomplishedCond->substituteNamedRegions(world);
	return accomplishedCond->eval(world);
}

void ActDribble::print(std::ostream& out) const{ 
	out << "(dribble :region ";
	if( getRegion() == NULL )
		out << "(null)";
	else
		getRegion()->print(out);
	out << ")";
}

void ActDribble::printPretty(std::ostream& out, 
		const std::string& line_header) const{
	out << line_header << "dribble to region:" << std::endl;
	if( getRegion() == NULL )
		out << line_header << " (null)\n";
	else
		getRegion()->printPretty( out, line_header + " " );
}

Action* ActDribble::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(dribble") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
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
				return new ActDribble(reg);
			}
		}
	}
	out=in;
	return NULL;
}

const Action* ActShoot::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

Action* ActShoot::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(shoot") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActShoot();
	}
	out=in;
	return NULL;
}


const Action* ActTackle::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

Action* ActTackle::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<const PlayerReference*>* players_new= new vector<const PlayerReference*>();

	for(unsigned int i =0; i!= M_players.size();i++)
		players_new->push_back((PlayerReference*)M_players.at(i)
				->deepCopy(params, players));
	return new ActTackle(*players_new);
}

Action* ActTackle::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	return deepCopy(params,players);
}


void ActTackle::print(std::ostream& out) const{
	out << "(tackle :players (list " ;
	for(unsigned int i = 0; i!= M_players.size(); i++){
		M_players.at(i)->print(out);
		out<<" ";
	}
	out << "))";
}

void ActTackle::printPretty(std::ostream& out,
		const std::string& line_header) const{
	out << line_header
			<< "tackle player(s) ";

	for(unsigned int i = 0; i!= M_players.size(); i++){
		M_players.at(i)->print(out);
		out<<" ";
	}
	out << std::endl;
}

const vector<const PlayerReference*>& ActTackle::relevantPlayers() const{

	return M_players;
}

Action* ActTackle::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(tackle") >> *space_p >> ":players";

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse players. Amount is not pre-defined
		// To store the players
		vector<PlayerReference*> players_
		= PlayerReference::parsePlayerList(in.substr(result.length), rest,
				parameters,players);
		if(players_.size()>0){
			//Check for the final '))'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);

				return new ActTackle(PlayerReference::toVectorConstPlayerReference(players_));
			}
		}
	}
	out=in;
	return NULL;
}

const Action* ActIntercept::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

bool ActIntercept::isAccomplished(const PlayerID& p,
		const Context& world) const{
	vector<const PlayerReference*>* playerVector=new vector<const PlayerReference*>();
	playerVector->push_back(new PlayerID(p));
	CondBallOwner* accomplishedCond
	=new CondBallOwner(*playerVector);

	accomplishedCond->substituteNamedRegions(world);
	return accomplishedCond->eval(world);
}

Action* ActIntercept::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(intercept") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActIntercept();
	}
	out=in;
	return NULL;
}

const Action* ActStop::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

Action* ActStop::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(stop") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActStop();
	}
	out=in;
	return NULL;
}

void ActAttentionToReg::print( std::ostream& out ) const
{ 
	out << "(attentionTo :region ";
	if( getRegion() == NULL )
		out << "(null)";
	else
		getRegion()->print(out);
	out << ")";
}

void ActAttentionToReg::printPretty( std::ostream& out, 
		const std::string& line_header ) const{
	out << line_header << "attention to region:" << std::endl;
	if( getRegion() == NULL )
		out << line_header << " (null)\n";
	else
		getRegion()->printPretty( out, line_header + " " );
}

bool ActAttentionToReg::isAccomplished(const PlayerID& p,
		const Context& world) const{
	//TODO

	return true;
}

const Action* ActAttentionToReg::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

Action* ActAttentionToReg::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(attentionTo") >> *space_p >> ":region" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse region
		Region* reg= Region::parse(in.substr(result.length), rest,parameters,players);

		if(reg){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActAttentionToReg(reg);
			}
		}
	}
	out=in;
	return NULL;
}

const Action* ActAttentionToObj::necessaryAction(const PlayerID& p,
		const Context& world)
const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}


bool ActAttentionToObj::isAccomplished(const PlayerID& p,
		const Context& world) const{
	//TODO

	return true;
}

const vector<const PlayerReference*>& ActAttentionToObj::relevantPlayers() const{
	if(m_obj->type()=='p') {//player
		vector<const PlayerReference*>* res =new vector<const PlayerReference*>();
		res->push_back((PlayerReference*)m_obj);
		return *res;
	}
	return *(new vector<const PlayerReference*>());

}

Action* ActAttentionToObj::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(attentionTo") >> *space_p >> ":object" >> *space_p;

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse object
		Object* obj= Object::parse(in.substr(result.length), rest,
				parameters,players);

		if(obj){
			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActAttentionToObj(obj);
			}
		}
	}
	out=in;
	return NULL;
}

void ActAttentionToObj::print( std::ostream& out ) const { 
	out << "(attentionTo :object ";
	if( getObject() == NULL )
		out << "(null)";
	else
		getObject()->print(out);
	out << ")";
};

void ActAttentionToObj::printPretty( std::ostream& out, 
		const std::string& line_header) const{
	out << "direct attention to ";
	if( getObject() == NULL )
		out << "nothing!";
	else
		getObject()->print(out);
	out << ")";
}

const Action* ActMarkGoal::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}


Action* ActMarkGoal::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(markGoal") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActMarkGoal();
	}
	out=in;
	return NULL;
}

void ActSeq::getExecuted(Executor& executor) const{
	for(unsigned int i=0; i!=actions->size();i++)
		if(!actions->at(i)->done){
			actions->at(i)->getExecuted(executor);
			return;
		}
}

const Action* ActSeq::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		for(unsigned int i=0; i!=actions->size();i++)
			if(!actions->at(i)->isAccomplished(p,world))
				return actions->at(i)->necessaryAction(p,world);
	return NULL;
}

bool ActSeq::isAccomplished(const PlayerID& p,const Context& world) const{
	for(unsigned int i=0; i!=actions->size();i++)
		if(!actions->at(i)->isAccomplished(p,world))
			return false;
	return true;
}

const vector<const PlayerReference*>& ActSeq::relevantPlayers() const{
	vector<const PlayerReference*>* res = new vector<const PlayerReference*>;

	for(unsigned int i=0; i!=actions->size();i++){
		vector<const PlayerReference*> vecAct=actions->at(i)->relevantPlayers();
		// TODO: avoid duplicates
		if(vecAct.size()==0){
			res->reserve(vecAct.size());
			copy(vecAct.begin(),vecAct.end(),res->end());
		}
	}

	return *res;
}

Action* ActSeq::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<Action*>* actions_new= new vector<Action*>;

	for(unsigned int i =0; i!= actions->size();i++)
		actions_new->push_back(actions->at(i)->deepCopy(params, players));
	return new ActSeq(actions_new);
}

Action* ActSeq::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<Action*>* actions_new= new vector<Action*>;

	for(unsigned int i =0; i!= actions->size();i++)
		actions_new->push_back(actions->at(i)->inversion(params, players));
	return new ActSeq(actions_new);
}

void ActSeq::substituteNamedRegions(const Context& world){
	for(unsigned int i=0;i!=actions->size();i++)
		actions->at(i)->substituteNamedRegions(world);
}



void ActSeq::print(std::ostream& out) const{
	out << "(seq  " ;
	for(unsigned int i = 0; i!= actions->size(); i++){
		actions->at(i)->print(out);
		out<<" ";
	}
	out << ")";
}

void ActSeq::printPretty(std::ostream& out,
		const std::string& line_header) const{
	out << line_header
			<< "action sequence ";

	for(unsigned int i = 0; i!= actions->size(); i++){
		actions->at(i)->printPretty(out,line_header);
		out<<" ";
	}
	out << std::endl;
}

Action* ActSeq::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(seq") >> *space_p ;
	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest;

		//Must parse actions. Amount is not pre-defined
		//Will parse the first one, is mandatory
		Action* act=
				Action::parse(in.substr(result.length), rest,parameters,players);

		if(act){
			// To store the actions
			vector<Action*>* actions = new vector<Action*>;

			while(act){
				actions->push_back(act);

				//Parse a new action
				act= Action::parse(rest, rest,parameters,players);
			}

			//Check for the final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p>>')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new ActSeq(actions);
			}
		}
	}
	out=in;
	return NULL;
}


const Action* 
ActReceiveBall::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}

const vector<const PlayerReference*>& ActReceiveBall::relevantPlayers() const{
	// TODO
	// should look at the player with ball possession, but how should that be
	//detemined here, without any SoW info?
	return *(new  vector<const PlayerReference*>());
}

bool ActReceiveBall::isAccomplished(const PlayerID& p,
		const Context& world) const{
	vector<const PlayerReference*> playerVector;
	playerVector.push_back(new PlayerID(p));
	CondBallOwner* accomplishedCond
	=new CondBallOwner(playerVector);

	accomplishedCond->substituteNamedRegions(world);
	return accomplishedCond->eval(world);
}

Action* ActReceiveBall::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> act_p
	= *space_p >> str_p("(receiveBall") >> *space_p>>')';

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		out=in.substr(result.length);
		return new ActReceiveBall();
	}
	out=in;
	return NULL;
}

void ActMoveToOffsideLine::print( std::ostream& out ) const{
	out << "(moveToOffSideLine :y ";
	y->print(out);
	out<<")";
}

void ActMoveToOffsideLine::printPretty(std::ostream& out,
		const std::string& line_header) const{
	out<<line_header<<"moveToOffSideLine at y";
	y->print(out);
	out<<std::endl;
}

const Action* 
ActMoveToOffsideLine::necessaryAction(const PlayerID& p,
		const Context& world) const{
	if(!isAccomplished(p,world))
		return this;
	else
		return NULL;
}



Action* ActMoveToOffsideLine::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){

	rule<> act_p
	= *space_p >> str_p("(moveToOffSideLine") >> *space_p>>":y";

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),act_p,
			nothing_p);

	if(result.hit){
		string rest=in.substr(result.length);
		Decimal* d=(Decimal*)Decimal::parse(rest,rest,parameters);
		if(d!=NULL){
			act_p = *space_p >>')';
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),act_p,nothing_p);

			if(result.hit){
				out=rest.substr(result.length);
				return new ActMoveToOffsideLine(d);
			}
		}
	}
	out=in;
	return NULL;
}
