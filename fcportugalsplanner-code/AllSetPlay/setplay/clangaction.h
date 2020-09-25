// -*-c++-*-

/***************************************************************************
                                clangaction.h  
                       Abstract base class for CLang Actions
                             -------------------
    begin                : 28-MAY-2002
    copyright            : (C) 2006 Luis Mota
    email                : luis.mota@iscte.pt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/
#ifndef CLANGACTION_H
#define CLANGACTION_H

#include <memory>
#include <cassert>
//#include "hasa.h"
#include <setplay/region.h>
#include <setplay/clang/clangutil.h>
#include <setplay/playerReference.h>
#include <setplay/cond.h>

namespace fcportugal{
namespace setplay{
// Actions will be described further down, next to their classes' definitions
class ActPos; //'p'
class ActForward; // 'f'
class ActPass; // 'y'
class ActMark; // 'm'
class ActMarkLinePlayer; //'l'
class ActMarkLineReg; // 'r'
class ActDribble; // 'd'
class ActOffsideLine; // 'o'
class ActClear; //'c'
class ActShoot; // 's'
class ActHold; // 'h'
class ActIntercept; // 'i'
class ActTackle; // 't'
class ActSeq; // 'q'
//Actions introduced in 2009:
class ActStop; // '0'
class ActAttentionToReg; // 'a'
class ActAttentionToObj; // 'b'
class ActMarkGoal; //'g'
// Actions introduced when applying to the middle-size league
class ActReceiveBall; // 'v'
//Actions introduced in 2010
class ActMoveToOffsideLine; //'e'

class Action{
public:
	class Executor{
	public:
		virtual ~Executor()
		{}

		virtual void execute( const ActPos& a )=0;
		virtual void execute( const ActForward& a )=0;
		virtual void execute( const ActPass& a )=0;
		virtual void execute( const ActMark& a )=0;
		virtual void execute( const ActMarkLinePlayer& a )=0;
		virtual void execute( const ActMarkLineReg& a )  = 0;
		virtual void execute( const ActDribble& a )  = 0;
		virtual void execute( const ActOffsideLine& a )  = 0;
		virtual void execute( const ActClear& a )  = 0;
		virtual void execute( const ActShoot& a )  = 0;
		virtual void execute( const ActHold& a )  = 0;
		virtual void execute( const ActIntercept& a )  = 0;
		virtual void execute( const ActTackle& a )  = 0;

		// Not necessary, since it suffices to call the individual
		// actions in the sequence.
		//virtual void execute( const ActSeq& a )  = 0;

		//2009
		virtual void execute( const ActStop& a )  = 0;
		virtual void execute( const ActAttentionToReg& a )  = 0;
		virtual void execute( const ActAttentionToObj& a )  = 0;
		virtual void execute( const ActMarkGoal& a )  = 0;

		//Middle-size
		virtual void execute( const ActReceiveBall& a )  = 0;

		//2010
		virtual void execute( const ActMoveToOffsideLine& a )  = 0;

	};

	Action():active(true),done(false){}

	virtual
	~Action();

	virtual void getExecuted(Executor& executor) const = 0;

	// Used for implementation in original simulation 2d team
	virtual char actionType() const =0;
	// Don't think this is necessary...
	//virtual const PointSimple* actionPoint() const =0;

	virtual void print(std::ostream& out) const = 0;

	virtual void printPretty(std::ostream& out,
			const std::string& line_header) const = 0;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	static vector<Action*>*
	parseActionList(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual void substituteNamedRegions(const Context& world)=0;

	virtual string label() const =0;

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world)const=0;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const =0;

	virtual const vector<const PlayerReference*>& relevantPlayers() const=0;

	// In order to flag an action as nor being active, for situations
	// when the waitTime has not elapsed. In this case, the player will
	// know the action, and therefore be prepared to the execution,
	// but without actually executing it
	// Attribute is mutable in order to change const actions in
	// Setplay.possibleActions...
	// Is this acceptable?
	mutable bool active;
	mutable bool done;

}; // end Action


// Position the player in a region (argument)
class ActPos:public Action{
public:

	ActPos( const Region* const   r );

	virtual ~ActPos();

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'p';}

	virtual void print( std::ostream& out ) const;

	virtual void printPretty(std::ostream& out,
			const std::string& line_header ) const;

	const Region* getRegion() const;

	inline virtual string label() const {return "pos";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players)
	const{
		return new ActPos(m_reg->deepCopy(params,players));};

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActPos(m_reg->inversion(params,players));};


	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};



private:
	const Region* const m_reg;
};

typedef enum _BallMoveType {
	normalBallMove, fastBallMove, slowBallMove, undefinedBallMove
} BallMoveType;


// Forward the ball to a region (argument)
class ActForward : public Action{
public:
	//ActForward();

	ActForward(const Region* const reg,const BallMoveType& move=undefinedBallMove);

	virtual ~ActForward();

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	// 'f' for forward
	inline virtual char actionType() const {return 'f';}

	virtual void print( std::ostream& out ) const;

	virtual void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	const Region* getRegion() const;

	BallMoveType ballMoveType() const;

	inline virtual string label() const {return "forward";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActForward(m_reg->deepCopy(params,players),m_bm);};

	inline virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActForward(m_reg->inversion(params,players),m_bm);};

	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};


private:
	const Region* const m_reg;
	const BallMoveType m_bm;
};


// Pass the ball to player(s) (argument). Pass can be done to any player
class ActPass:public Action{
public:

	ActPass();

	ActPass(const vector<const PlayerReference*>& players ,
			BallMoveType type=normalBallMove);

	virtual ~ActPass();

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	// 'y' for plaYer
	inline virtual char actionType() const {return 'y';}
	//inline virtual const PointSimple* actionPoint() const{
	//assert(false); return NULL;} //BUST: should not be called!

	virtual void print( std::ostream& out ) const;

	virtual void printPretty( std::ostream& out,
			const std::string& line_header) const;


	const vector<const PlayerReference*>& getPlayers() const;

	BallMoveType ballMoveType() const{return type;}

	inline virtual string label() const {return "ballToPlayer";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world)const;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};

private:
	const vector<const PlayerReference*> M_players;
	BallMoveType type;
} ;


// Actively mark player(s) (argument). Marking can be done to any player
class ActMark:public Action{
public:
	ActMark();

	ActMark(const vector<const PlayerReference*>& players);

	virtual ~ActMark();

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'm';}

	//virtual const PointSimple* actionPoint() const{
	//assert(false);return NULL;} //BUST: should not be called!

	virtual void print(std::ostream& out) const;

	virtual void printPretty(std::ostream& out,
			const std::string& line_header) const;

	const vector<const PlayerReference*>& getPlayers() const;

	inline virtual string label() const {return "mark";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,
			const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};

private:
	const vector<const PlayerReference*> M_players;
} ;

// Actively mark pass-line to player(s) (argument). Marking can be done
// to any player
class ActMarkLinePlayer:public Action{
public:
	ActMarkLinePlayer();

	ActMarkLinePlayer(const vector<const PlayerReference*>& players);

	virtual ~ActMarkLinePlayer();

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'l';}

	//inline virtual const PointSimple* actionPoint() const{
	//assert(false);return NULL;} //BUST: should not be called!

	virtual void print(std::ostream& out) const;

	virtual void printPretty(std::ostream& out,
			const std::string& line_header) const;

	const vector<const PlayerReference*>& getPlayers() const;

	inline virtual string label() const {return "markLinePlayer";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;


	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;}

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	const vector<const PlayerReference*> M_players;
} ;

// Actively mark pass-line to region (argument).
class ActMarkLineReg:public Action{
public:
	inline ActMarkLineReg(const Region* const reg):Action(),m_reg(reg){}

	inline ~ActMarkLineReg(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'r';}

	//inline virtual const PointSimple* actionPoint() const{
	//return m_reg->getCentralPoint()->asPointSimple();}

	void print( std::ostream& out ) const;

	void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline const Region* getRegion() const { return m_reg; }


	inline virtual string label() const {return "markLineReg";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;


	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;};

	virtual vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActMarkLineReg(m_reg->deepCopy(params,players));};

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActMarkLineReg(m_reg->inversion(params,players));};


	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};


private:
	const Region* const m_reg;
};


// Set offside line trap to region (argument).
class ActOffsideLine: public Action {
public:

	//inline ActOffsideLine():Action(){}

	inline ActOffsideLine(const Region* const r):Action(),m_reg(r){}

	inline virtual ~ActOffsideLine(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'o';}

	void print( std::ostream& out ) const;

	void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline const Region* getRegion() const{ return m_reg; }

	inline virtual string label() const {return "offsideLine";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;}

	virtual const vector<const PlayerReference*>& relevantPlayers() const{
		return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);


	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActOffsideLine(m_reg->deepCopy( params,players));}

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActOffsideLine(m_reg->inversion(params,players));}

	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);}

private:
	const Region* const m_reg;
};

// Dribble to region (argument).
class ActDribble:public Action{
public:
	//inline ActDribble():Action(){}

	inline ActDribble(const Region* const reg):Action(),m_reg(reg){}

	inline ~ActDribble(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);    };

	inline virtual char actionType() const {return 'd';}

	void print( std::ostream& out ) const;
	void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline const Region* getRegion() const { return m_reg; }

	inline virtual string label() const {return "dribble";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const{
		return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActDribble(m_reg->deepCopy(params,players));}

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActDribble(m_reg->inversion(params,players));}

	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};


private:
	const Region* const m_reg;
};


// Clear ball from region (argument).
class ActClear:public Action{
public:
	inline ActClear(const Region* const reg ):Action(),m_reg(reg){}

	inline ~ActClear(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'c';}


	void print( std::ostream& out ) const;

	void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline const Region* getRegion() const { return m_reg; }

	inline virtual string label() const {return "clear";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActClear(m_reg->deepCopy(params,players));}

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActClear(m_reg->inversion(params,players));}

	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};


private:
	const Region* const m_reg;
};

// Shoot at goal
class ActShoot:public Action{
public:
	inline ActShoot():Action(){}

	inline ~ActShoot(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 's';}

	inline void print( std::ostream& out ) const{out<<"(shoot)";}

	inline void printPretty(std::ostream& out,
			const std::string& line_header ) const{
		out << line_header << "shoot" << std::endl; }

	inline virtual string label() const {return "shoot";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;}

	virtual const vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActShoot();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActShoot();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


};

// Hold ball possession in present position
class ActHold:public Action{
public:
	inline ActHold():Action(){}

	inline ~ActHold(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	//'h'
	inline virtual char actionType() const {return 'h';}

	inline void print( std::ostream& out ) const{ out << "(hold)"; }

	inline void
	printPretty( std::ostream& out, const std::string& line_header ) const
	{ out << line_header << "hold" << std::endl; }


	inline virtual string label() const {return "hold";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActHold();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActHold();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


};

// Actively intercept ball
class ActIntercept:public Action{
public:
	inline ActIntercept():Action(){}

	inline ~ActIntercept(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 'i';}


	inline void print( std::ostream& out ) const{ out << "(intercept)"; }

	inline void printPretty(std::ostream& out,
			const std::string& line_header) const{
		out<<line_header<<"intercept"<<std::endl;}

	inline virtual string label() const {return "intercept";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActIntercept();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActIntercept();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};
};


// Tackle player(s) (argument)
class ActTackle:public Action{
public:
	inline ActTackle(const vector<const PlayerReference*>& players)
	:Action(),M_players(players){}

	inline ~ActTackle(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 't';}

	void print(std::ostream& out) const;

	void printPretty(std::ostream& out,
			const std::string& line_header) const;

	/* class specific stuff */
	inline const vector<const PlayerReference*>& getPlayers() const {
		return M_players; }

	inline virtual string label() const {return "tackle";}

	virtual const Action*
	necessaryAction(const PlayerID& p,
			const Context& world) const ;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;}

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	const vector<const PlayerReference*> M_players;
};


//Actions introduced in 2009:

// Stop at the current position
class ActStop:public Action{
public:
	inline ActStop():Action(){}

	inline ~ActStop(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return '0';}

	inline void print( std::ostream& out ) const{out << "(stop)"; }

	inline void printPretty(std::ostream& out,
			const std::string& line_header ) const{
		out<<line_header<<"stop "<<std::endl; }


	inline virtual string label() const {return "stop";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return false;}

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActStop();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActStop();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){}


};

// Keep visual attention to region (argument)
class ActAttentionToReg : public Action{
public:
	//inline ActAttentionToReg():Action(){}

	inline ActAttentionToReg(const Region* const reg):Action(),m_reg(reg){}

	inline ~ActAttentionToReg(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 'a';}

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	inline const Region* getRegion() const { return m_reg; }

	inline virtual string label() const {return "attentionToReg";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActAttentionToReg(m_reg->deepCopy(params,players));}

	inline virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActAttentionToReg(m_reg->inversion(params,players));}

	inline virtual void substituteNamedRegions(const Context& world){
		m_reg->substituteNamedRegions(world);};


private:
	const Region* const m_reg;
};

// Keep visual attention to object (argument): ball, player,...
class ActAttentionToObj: public Action{
public:
	inline ActAttentionToObj():Action(),m_obj(NULL){};

	inline ActAttentionToObj( Object* obj):Action(),m_obj(obj){};

	inline virtual ~ActAttentionToObj(){};

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);};

	inline virtual char actionType() const {return 'b';}

	virtual void print( std::ostream& out ) const ;

	virtual void printPretty( std::ostream& out,
			const std::string& line_header) const;

	inline const Object* getObject() const{return m_obj;}

	void setObject( Object* obj );

	inline virtual string label() const {return "attentionToObj";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActAttentionToObj(m_obj->deepCopy(params,players));}

	inline virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActAttentionToObj(m_obj->inversion(params,players));}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


private:
	Object* m_obj;
} ;


// Mark shot-line to goal
class ActMarkGoal: public Action{
public:
	inline ActMarkGoal():Action(){}

	inline ~ActMarkGoal(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 'g';}


	inline void print(std::ostream& out) const{out<<"(markGoal)";}

	inline void printPretty( std::ostream& out,
			const std::string& line_header ) const{
		out<<line_header<<"mark goal "<<std::endl; }


	inline virtual string label() const {return "markGoal";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{
		//TODO
		return true;}

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActMarkGoal();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActMarkGoal();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};


};


// Sequence of actions (argument)
class ActSeq:public Action{
public:
	inline ActSeq():Action(),actions(new vector<Action*>){}

	inline ActSeq( vector<Action*>* actions_ ):Action(),actions(actions_){}

	inline ~ActSeq(){}

	inline virtual void getExecuted(Executor& executor) const;

	// caution: must have special treatment
	inline virtual char actionType() const {return 'q';}

	void print(std::ostream& out) const;

	void printPretty(std::ostream& out,
			const std::string& line_header) const;

	/* class specific stuff */
	inline const vector<Action*>* getActions() const{return actions;}

	inline void setActions( vector<Action*>* actions_ ){actions=actions_;}

	inline void addAction(Action* i){actions->push_back( i );}

	inline void clearActions(){actions->clear();}

	inline virtual string label() const {return "seq";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Action* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Action* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world);

private:
	vector<Action*>* actions;
};





// Receive a pass from ball owner and gain ball possession
class ActReceiveBall:public Action{
public:
	inline ActReceiveBall():Action(){}

	inline ~ActReceiveBall(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 'v';}


	inline void print( std::ostream& out ) const{ out << "(receiveBall)"; }

	inline void printPretty(std::ostream& out,
			const std::string& line_header) const{
		out<<line_header<<"receiveBall"<<std::endl;}

	inline virtual string label() const {return "receiveBall";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const ;

	virtual bool isAccomplished(const PlayerID& p,const Context& world) const;

	virtual const vector<const PlayerReference*>& relevantPlayers() const;

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActReceiveBall();}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& /*params*/,
			const vector<PlayerReference*>& /*players*/) const{
		return new ActReceiveBall();}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};

};

// Move close to the opponent's offside line. x coordinate received as
// argument
class ActMoveToOffsideLine:public Action{
public:
	inline ActMoveToOffsideLine(Decimal* yCoord):Action(),y(yCoord){}
	inline ActMoveToOffsideLine():Action(),y(NULL){}

	inline ~ActMoveToOffsideLine(){}

	inline virtual void getExecuted(Executor& executor) const{
		executor.execute(*this);}

	inline virtual char actionType() const {return 'e';}

	void print( std::ostream& out ) const;

	void printPretty(std::ostream& out,
			const std::string& line_header) const;

	inline const Decimal* getY() const{return y;}

	inline virtual string label() const {return "moveToOffSideLine";}

	virtual const Action* necessaryAction(const PlayerID& p,
			const Context& world) const;

	inline virtual bool
	isAccomplished(const PlayerID& /*p*/,
			const Context& /*world*/) const{return false;};

	virtual const
	vector<const PlayerReference*>& relevantPlayers() const{return *(new vector<const PlayerReference*>());}

	static Action* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Action*
	deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActMoveToOffsideLine(y->deepCopy(params,players));}

	inline virtual Action*
	inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new ActMoveToOffsideLine(y->inversion(params,players));}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/){};
private:
	Decimal* y;
};
}
}
#endif
