/*
 * Context.h
 *
 *  Created on: 23 Oct 2013
 *      Author: luismota
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <rcsc/player/world_model.h>
#include <setplay/cond.h>
#include <setplay/playerReference.h>

#define BALL_OWNER_DISTANCE_THRESHOLD 2.0

class Context : public fcportugal::setplay::Context{
public:
	Context(const rcsc::WorldModel& worldModel);
	virtual ~Context();


	virtual bool lookup( const fcportugal::setplay::CondBallOwner& cond ) const ;
	virtual bool lookup( const fcportugal::setplay::CondClearShotAtGoal& cond ) const ;
	virtual bool lookup( const fcportugal::setplay::CondClearPassToPlayer& cond ) const ;
	virtual bool lookup( const fcportugal::setplay::CondClearPassToRegion& cond ) const ;
	virtual bool lookup( const fcportugal::setplay::CondNamed& cond ) const ;
	virtual bool lookup( const fcportugal::setplay::CondOppGoal& cond ) const;
	virtual bool lookup( const fcportugal::setplay::CondOurGoal& cond ) const;
	virtual bool lookup( const fcportugal::setplay::CondGoalDiff& cond ) const;
	virtual bool lookup( const fcportugal::setplay::CondNearOffsideLine& cond ) const;

	virtual const fcportugal::setplay::PlayerID& me() const;
	virtual double time() const;
	virtual const fcportugal::setplay::PointSimple* ballPos() const;
	virtual const fcportugal::setplay::PointSimple* playerPos(const fcportugal::setplay::PlayerID&) const;
	virtual rcss::clang::PlayMode playMode() const;
	virtual const std::string& opponentName() const;
	virtual unsigned int numPlayersPerTeam() const;
	virtual const fcportugal::setplay::NameSubstitutions& nameSubstitutions() const;


	double fieldLength() const;
	double fieldWidth() const;

	// for usage in role_fcportugal_setplay
	inline const rcsc::WorldModel& getWorldModel() const{return *worldModel;}

private:
	const rcsc::WorldModel* worldModel;

	// aux functions
	int ourScore() const;
	int oppScore() const;
};

#endif /* CONTEXT_H_ */
