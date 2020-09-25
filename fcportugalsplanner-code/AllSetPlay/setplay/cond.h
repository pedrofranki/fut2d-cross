// -*-c++-*-

/***************************************************************************
                                   cond.h  
                               Clang conditions
                             -------------------
    begin                : 28-NOV-2006
    copyright            : (C) 2006 by Luis Mota
    email                : luis.mota@iscte.pt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                       *
 *                                                                         *
 ***************************************************************************/

#ifndef COND_H_
#define COND_H_
#include <setplay/clang/clangutil.h>
#include <memory>
#include <setplay/clang/compop.h>
#include <setplay/clang/hasa.h>
#include <setplay/clang/rcssexceptions.h>
#include <setplay/region.h>
#include <setplay/playerReference.h>
#include <setplay/parameter.h>

#include <string.h>

namespace fcportugal{
namespace setplay{

// Conditions are described further down, close to their classes'
// definitions
class CondBool;
class CondPlayerPos;
class CondBallOwner;
class CondClearShotAtGoal;
class CondClearPassToPlayer;
class CondClearPassToRegion;
class CondPlayMode;
class CondAnd;
class CondOr;
class CondNot;
class CondBallPos;
class CondNamed;
class CondTime;
class CondOppGoal;
class CondOurGoal;
class CondGoalDiff;
class CondNearOffsideLine;


class Context
{
public:
	Context()
{}

	virtual 
	~Context()
	{}

	virtual 
	bool
	lookup( const CondBallOwner& cond ) const = 0;

	virtual 
	bool
	lookup( const CondClearShotAtGoal& cond ) const = 0;

	virtual 
	bool
	lookup( const CondClearPassToPlayer& cond ) const = 0;

	virtual 
	bool
	lookup( const CondClearPassToRegion& cond ) const = 0;

	virtual 
	bool
	lookup( const CondNamed& cond ) const = 0;

	virtual 
	bool
	lookup( const CondOppGoal& cond ) const = 0;

	virtual 
	bool
	lookup( const CondOurGoal& cond ) const = 0;

	virtual 
	bool
	lookup( const CondGoalDiff& cond ) const = 0; 

	virtual 
	bool
	lookup( const CondNearOffsideLine& cond ) const = 0; 

	// Run time inspectors
	virtual const PlayerID& me() const=0;

	virtual double time() const=0;

	// methods to determine object positions
	virtual const fcportugal::setplay::PointSimple*
	ballPos() const=0;

	// Player nums variate from 1 to numPlayersPerTeam()
	// Deals both with teammates and opponents
	virtual const fcportugal::setplay::PointSimple*
	playerPos(const PlayerID& pl) const=0;

	// To check the number of players, variable from league to league
	virtual unsigned int numPlayersPerTeam() const=0;

	// To invalidate outside points in CBR classification
	virtual double fieldLength() const=0;
	virtual double fieldWidth() const=0;

	virtual rcss::clang::PlayMode playMode() const=0;

	// For name substitutions
	virtual const fcportugal::setplay::NameSubstitutions& 
	nameSubstitutions() const=0;

	virtual const string& opponentName() const=0;

	// Trivial, not virtual, cond evaluators
	bool lookup( const CondPlayMode& cond ) const; 

	bool lookup( const CondPlayerPos& cond ) const;

	bool lookup( const CondBallPos& cond ) const;

	bool lookup( const CondTime& cond ) const;

}; // end Context


class Cond{
public:
	Cond()
{}

	virtual
	~Cond()
	{}

	virtual void
	print( std::ostream& out ) const = 0;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const = 0;


	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual bool
	eval( const Context& context ) const = 0;

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual void substituteNamedRegions(const Context& world) =0;

};

// Primitive cond, true or false
class CondBool
		: public Cond
{
public:

	inline CondBool(): Cond(),	m_state( false ){}

	inline CondBool( bool state ): Cond(),m_state( state ){}

	inline ~CondBool(){}

	inline virtual void
	print( std::ostream& out ) const{out<<"("<<(getState()?"true":"false")<<")";}

	inline virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const
	{out<<line_header<<(getState()?"true":"false" )<<std::endl;}


	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& ) const{return m_state;}

	inline bool
	getState() const{return m_state;}

	inline virtual Cond*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondBool(m_state);}

	inline virtual Cond*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondBool(m_state);}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	bool m_state;
};

// Player(s) position in a region
class CondPlayerPos:public Cond{
public:

	/*inline CondPlayerPos(): Cond(),M_min_match(new Integer(1)),
	M_max_match(new Integer(22)){}*/

	CondPlayerPos(const vector<const PlayerReference*>&  players,
			const Region* const reg,
			Integer* min_match,
			Integer* max_match );

	inline virtual ~CondPlayerPos(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup( *this );}

	/* class specific stuff */

	inline const Integer*
	getMinMatch() const{return M_min_match;}

	inline const Integer*
	getMaxMatch() const{ return M_max_match; }

	inline const Region*
	getRegion() const { return m_reg; }

	inline const vector<const PlayerReference* >&
	getPlayerSet() const { return m_players; }

	inline void
	setMinMatch( Integer* x ){ M_min_match = x; }

	inline void
	setMaxMatch( Integer* x ){ M_max_match = x; }


	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world);

private:
	Integer* M_min_match;
	Integer* M_max_match;
	vector<const PlayerReference*> m_players;
	const Region* const m_reg;
};


// Ball position in a region
class CondBallPos : public Cond
{
public:
	inline CondBallPos(const Region* const reg ): Cond(),m_reg( reg ){}

	inline ~CondBallPos(){}

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const {return context.lookup( *this );}

	inline const Region*
	getRegion() const{return m_reg;}


	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world);

private:
	const Region* const m_reg;
};


// Ball possession by player(s)
class CondBallOwner: public Cond
{
public:
	CondBallOwner(const vector<const PlayerReference* >& players);

	inline ~CondBallOwner(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup(*this);}



	/* class specific stuff */
	inline const vector<const PlayerReference* >&
	getPlayerSet() const{return m_players;}

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	vector<const PlayerReference* >  m_players;
};


// Abstract condition, cannot be instantiated: do not use
class CondClearPass: public Cond
{
public:
	CondClearPass(const vector<const PlayerReference*>& players);

	inline ~CondClearPass(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	bool
	eval( const Context& context ) const=0;

	/* class specific stuff */
	inline const vector<const PlayerReference* >&
	getFromPlayerSet() const{return m_players_from;}

	/* Inviable, as players is const
      inline void
      setFromPlayers( vector<PlayerReference*> *players){m_players_from=players;}

      inline void
      addFromPlayer( PlayerReference* i ){m_players_from->push_back(i);}

      inline void
      clearFromPlayers(){m_players_from->clear();}*/

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	inline virtual void substituteNamedRegions(const Context& world)=0;

protected:
	vector<const PlayerReference*> m_players_from;
};

// Player(s) can shoot at goal with an estimated good success probability
class CondClearShotAtGoal: public CondClearPass
{
public:
	inline CondClearShotAtGoal(const vector<const PlayerReference*>& players)
	:CondClearPass(players){}

	inline ~CondClearShotAtGoal(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup(*this);}


	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};

};

// Player(s) can pass to other player(s) with an estimated good success
// probability
class CondClearPassToPlayer: public CondClearPass
{
public:
	CondClearPassToPlayer(const vector<const PlayerReference*>& players_from,
			const vector<const PlayerReference*>& players_to);

	inline ~CondClearPassToPlayer(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup(*this);};

	/* class specific stuff */
	inline const vector<const PlayerReference*>&
	getToPlayerSet() const{return m_players_to;}

	/* Inviable, as players is const
      inline void
      setToPlayers( vector<PlayerReference*> *players){m_players_to = players;}

      inline void
      addToPlayer( PlayerReference* i ){m_players_to->push_back( i );}

      inline void
      clearToPlayers(){m_players_to->clear();}*/

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	vector<const PlayerReference*> m_players_to;
};


// Player(s) can pass to a region with an estimated good success
// probability
class CondClearPassToRegion: public CondClearPass
{
public:
	inline CondClearPassToRegion(const vector<const PlayerReference*>& players_from,
			const Region* const to)
	: CondClearPass(players_from),m_region_to(to){}

	inline ~CondClearPassToRegion(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup(*this);};

	/* class specific stuff */
	inline const Region*
	getToRegion() const{return m_region_to;}

	/* Inviable, as region is const
      inline void
      setToRegion( Region* reg){m_region_to = reg;}*/

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void substituteNamedRegions(const Context& world)
	{m_region_to->substituteNamedRegions(world);};

private:
	const Region* const m_region_to;
};



// Check for current Play Mode
class CondPlayMode: public Cond
{
public:
	inline CondPlayMode(const rcss::clang::PlayMode& pm):Cond(),M_pm(pm){}

	inline ~CondPlayMode(){}

	inline virtual void
	print( std::ostream& out ) const{out<<"(playm "<<
		rcss::clang::MODE_STRINGS[M_pm]<<")";}

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static rcss::clang::PlayMode* parsePlayMode(const string& mode);

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup(*this);}

	/* class specific stuff */
	inline rcss::clang::PlayMode
	getPlayMode() const{ return M_pm; }

	inline void
	setPlayMode( const rcss::clang::PlayMode& pm){ M_pm = pm; }

	inline virtual Cond* deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondPlayMode(M_pm);};

	inline virtual Cond* inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondPlayMode(M_pm);};

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};

private:
	rcss::clang::PlayMode M_pm;
};

// Composite condition: logical and
class CondAnd : public Cond
{
public:
	typedef std::list< Cond* > Storage;

	inline CondAnd(const Storage& conds):Cond(),m_conds(conds){}

	virtual ~CondAnd();

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	bool eval( const Context& context ) const;

	inline const Storage&
	getConds() const{return m_conds;}

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world);

private:
	Storage m_conds;

};

// Composite condition: logical or
class CondOr
		: public Cond
{
public:
	typedef std::list< Cond* > Storage;

	inline CondOr(const Storage& conds):Cond(),m_conds(conds){}

	virtual ~CondOr();

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	bool eval( const Context& context ) const;

	inline const Storage&
	getConds() const{return m_conds;}


	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world);

private:
	Storage m_conds;
};


// Composite condition: logical not
class CondNot
		: public Cond{
public:
	inline CondNot(Cond* cond):Cond(),m_cond(cond){}

	inline virtual ~CondNot(){}

	virtual void
	print( std::ostream& out ) const ;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	bool eval( const Context& context ) const;

	inline const Cond*
	getCond() const{ return m_cond; }


	inline virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players ) const{
		return new CondNot(m_cond->deepCopy(params,players));}

	inline virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players ) const
	{return new CondNot(m_cond->inversion(params,players));}

	inline virtual void substituteNamedRegions(const Context& world){
		m_cond->substituteNamedRegions(world);};


private:
	Cond* m_cond;
};


// Class to model conditions with operators. Do not use directly,
// use subsequent sub-classes
template< typename VALUE >
class CondComp
		: public Cond{
		private:
	const rcss::util::CompOp* M_comp;
	VALUE M_value;

		public:
	inline CondComp( const VALUE& value, const rcss::util::CompOp& comp )
	:M_comp(&comp),M_value(value){}

	inline ~CondComp(){}

	inline const VALUE
	getValue() const{return M_value;}

	inline VALUE
	setValue( const VALUE& value ){return M_value = value;}

	inline const rcss::util::CompOp&
	getComp() const{return *M_comp;}

	inline const rcss::util::CompOp&
	setComp(const rcss::util::CompOp& comp){return *(M_comp=&comp);}
};


// Condition with comparison operator to evaluate current time
class CondTime : public CondComp< int >
{
public:
	inline CondTime(const int& value,const rcss::util::CompOp& comp)
	:CondComp<int>(value, comp){}

	inline ~CondTime(){}

	inline virtual void
	print(std::ostream& out) const
	{out<<"(time "<<getComp()<<" "<<getValue()<<")";}

	inline virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const
	{print(out<<line_header);}

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual bool
	eval( const Context& context ) const{return context.lookup(*this);}

	inline Cond*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondTime(*this);}

	inline Cond*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondTime(*this);}

	inline void substituteNamedRegions(const Context& /*world*/){};

};

// Condition with comparison operator to evaluate opponent goals
class CondOppGoal : public CondComp< int >
{
public:
	inline CondOppGoal( const int& value, const rcss::util::CompOp& comp )
	: CondComp< int >( value, comp ){}

	inline ~CondOppGoal(){}

	inline virtual void
	print(std::ostream& out) const
	{out<<"(opp_goals "<<getComp()<<" "<<getValue()<<")";}

	inline virtual void
	printPretty(std::ostream& out,const std::string& line_header) const
	{print(out<<line_header);}

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	inline virtual bool
	eval( const Context& context ) const{return context.lookup(*this);}

	inline Cond*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondOppGoal(*this);}


	inline Cond*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondOppGoal(*this);}

	inline void substituteNamedRegions(const Context& /*world*/){};

};

// Condition with comparison operator to evaluate own goals
class CondOurGoal : public CondComp< int >{
public:
	inline CondOurGoal(const int& value,const rcss::util::CompOp& comp)
	:CondComp<int>(value,comp){}

	inline ~CondOurGoal(){}

	inline void
	print(std::ostream& out) const{
		out<<"(our_goals "<<getComp()<<" "<<getValue()<<")";}

	inline void
	printPretty(std::ostream& out,const std::string& line_header) const
	{print(out<<line_header);}

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual bool
	eval( const Context& context ) const{return context.lookup(*this);}

	inline Cond*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondOurGoal(*this);}

	inline Cond*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondOurGoal(*this);}

	inline void substituteNamedRegions(const Context& /*world*/){};

};

// Condition with comparison operator to evaluate score difference between
// our team and the opponent
class CondGoalDiff : public CondComp< int >
{
public:
	inline CondGoalDiff( const int& value, const rcss::util::CompOp& comp )
	: CondComp< int >( value, comp ){}

	inline ~CondGoalDiff(){}

	inline virtual void
	print(std::ostream& out) const
	{out<<"(goal_diff "<<getComp()<<" "<<getValue()<<")";}

	inline virtual void
	printPretty(std::ostream& out,const std::string& line_header) const
	{print(out<<line_header);}

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual bool
	eval(const Context& context) const{return context.lookup(*this);}

	inline Cond*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new CondGoalDiff(*this);}

	inline Cond*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const
	{return new CondGoalDiff(*this);}


	inline void substituteNamedRegions(const Context& /*world*/){};

};

class CondNearOffsideLine:public Cond{
public:

	//inline CondNearOffsideLine(): Cond(){}

	CondNearOffsideLine(const vector<const PlayerReference*>& players);

	inline virtual ~CondNearOffsideLine(){}

	virtual void
	print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Cond* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	inline bool
	eval( const Context& context ) const{return context.lookup( *this );}


	/* class specific stuff */
	inline const vector<const PlayerReference* >&
	getPlayerSet() const { return m_players; }

	/* Inviable, as players is const
      inline void
      set(vector<PlayerReference*> *players ){ m_players = players; }

      inline void
      addPlayer(PlayerReference* i ){ m_players->push_back( i ); }

      inline void
      clearPlayers(){ m_players->clear(); }*/

	virtual Cond* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Cond* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void substituteNamedRegions(const Context& /*world*/){};

private:
	vector<const PlayerReference*> m_players;
};
}
}
#endif
