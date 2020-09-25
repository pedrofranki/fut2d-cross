/*
 * Context.cpp
 *
 *  Created on: 23 Oct 2013
 *      Author: luismota
 */

#include "fcportugal_context.h"
#include <rcsc/common/server_param.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/common/logger.h>
#include <cstddef>



using namespace rcsc;

Context::Context(const rcsc::WorldModel& model):worldModel(&model) {}

Context::~Context() {}


bool Context::lookup( const fcportugal::setplay::CondBallOwner& cond ) const {
int debug=0;//JAF2014
    const vector<const fcportugal::setplay::PlayerReference*>& players_const=
			cond.getPlayerSet();

	// does not include self...
	const PlayerObject* nearestTeammate = worldModel->getTeammateNearestToBall(10,true);
	// First arg: count_thr accuracy count threshold, second: with_goalie
	// LMOTA don't have a clue what it means
	const PlayerObject* nearestOpponent = worldModel->getOpponentNearestToBall(10,true);



	for(unsigned int i=0; i!=players_const.size();i++){
		const fcportugal::setplay::PlayerID* const player=(players_const.at(i)->value());


//            if(nearestTeammate->unum() == player->number)
//            {
//            }

		// Rationale: player must be the closest to the ball, this distance must be below
		// BALL_OWNER_DISTANCE_THRESHOLD and the ball must be moving slowly or stopped
        if(player->team=="our"){
            if((int)player->number==worldModel->self().unum()){// its me!
                if(worldModel->ball().velValid() && worldModel->ball().vel().length()<2.0
						&& worldModel->self().distFromBall() < BALL_OWNER_DISTANCE_THRESHOLD
//            			&& (nearestTeammate ==NULL
//            					|| worldModel->self().distFromBall() <= nearestTeammate->distFromBall())
//            					&& (nearestOpponent == NULL || worldModel->self().distFromBall() <= nearestOpponent->distFromBall())
                   ){
//                   return (worldModel->ball().velValid() && worldModel->ball().vel().length()<2.0
//                            && worldModel->self().distFromBall() < BALL_OWNER_DISTANCE_THRESHOLD
//                            && (nearestTeammate ==NULL
//                                    || worldModel->self().distFromBall() <= nearestTeammate->distFromBall())
//                                    && (nearestOpponent == NULL || worldModel->self().distFromBall() <= nearestOpponent->distFromBall()
                     return(true);
                    }
            }
            else{ // some teammate
                if (nearestTeammate != NULL
						&& nearestTeammate->distFromBall() < BALL_OWNER_DISTANCE_THRESHOLD
						&& nearestTeammate->distFromBall() <= worldModel->self().distFromBall()
                        && (nearestOpponent == NULL ||  nearestTeammate->distFromBall() <= nearestOpponent->distFromBall())){

                    return(true);
                }
            }
        }
        else{ // opponent
            if (nearestOpponent != NULL
					&& nearestOpponent->unum()==(int)player->number
					&& worldModel->self().distFromBall() >= nearestOpponent->distFromBall()
					&& nearestOpponent->distFromBall() < BALL_OWNER_DISTANCE_THRESHOLD
					&& (nearestTeammate ==NULL
                            || nearestOpponent->distFromBall() <= nearestTeammate->distFromBall())
                ){

                return(true);
	}

        }
    }//end of for...

	return false;
}

bool Context::lookup( const fcportugal::setplay::CondClearShotAtGoal& cond ) const {
	 vector<const fcportugal::setplay::PlayerReference*> players =
			 cond.getFromPlayerSet();
	 // TODO Consider using shoot_table2008?

	 // Simply check if player has ball and is inside their penalty box...
	 // CAUTION
	 // This is not the best condition: it might be that one player in players_const has
	 // the ball, while a different one is in the penalty area... The conditon would be satisfied,
	 // but this is not desired. Must rethink this...
	 return lookup(fcportugal::setplay::CondBallOwner(players))
			 && fcportugal::setplay::Context::lookup(fcportugal::setplay::CondPlayerPos(players,
					 nameSubstitutions()
					 .getSubstitutionRegion("their_penalty_box"),
					 new fcportugal::setplay::Integer(1),
					 new fcportugal::setplay::Integer(players.size())));
}

bool Context::lookup( const fcportugal::setplay::CondClearPassToPlayer& cond ) const {
	// CAUTION
	// Will only evaluate own passes...
	// This implies that players other than the ball owner will never have this condition satisfied.
	// But since it is the ball owner that does all decisions, this may not be a question...

	// Check if this player has ball possession
	vector<const fcportugal::setplay::PlayerReference*> vectorMe;
	vectorMe.push_back(new fcportugal::setplay::PlayerID("our",worldModel->self().unum()));

	if(!lookup(fcportugal::setplay::CondBallOwner(vectorMe)))
			return false;

	// Check is this player is in the from list
	bool amInFromList=false;
	vector<const fcportugal::setplay::PlayerReference*> players_from=cond.getFromPlayerSet();

	for(unsigned int i=0; i<players_from.size();i++)
		if(players_from.at(i)->value()->number==(unsigned int)worldModel->self().unum()){
			amInFromList=true;
			break;
		}

	if(!amInFromList)
		return false;



	vector<const fcportugal::setplay::PlayerReference*> players_to=cond.getToPlayerSet();


    double first_speed = 0.0;//JAF2014 Changed to 0.0 - no minimum speed for pass
    double first_speed_thr = 0.1;//JAF2014
    int max_step = 3;//JAF2014 1 step pass, 2 or 3 step pass(2 or 3???)
    // Using KickTable, like with CondClearPassToRegion
    KickTable::Sequence sequence;
    for(unsigned int i=0; i<players_to.size();i++){
        // Check if player has a valid position...
        if(worldModel->ourPlayer(players_to.at(i)->value()->number)!=NULL
                && worldModel->ourPlayer(players_to.at(i)->value()->number)->pos().isValid()){

            bool success = KickTable::instance().simulate(*worldModel,
            worldModel->ourPlayer(players_to.at(i)->value()->number)->pos(),
            first_speed,
            first_speed_thr,
            max_step,//max_number_of_steps for pass(1, 2 or 3!)
            sequence);

            if(success){
				return true;
		}
	}
    }
	return false;

	/* Original atempt, using get_best_pass. Makes no sense, as passes are being done through
	 * forwards...
	Vector2D * target_point=0;
	double * first_speed=0;

	for(unsigned int i=0; i<players_to.size();i++){
		int toNumber=players_to.at(i)->value()->number ;
                        __FILE__"SP:lookup(condClearPass): verifying condition to pass to player %d.", toNumber);
		if(Body_Pass::get_best_pass( *worldModel,target_point,first_speed,&toNumber))
				return true;

	}
                  __FILE__"SP:Context:lookup(condClearPass): player did not find a good pass.");

	return false;
	*/

}

bool Context::lookup( const fcportugal::setplay::CondClearPassToRegion& cond ) const {
	// Will only evaluate own passes...
		// This implies that players other than the ball owner will never have this condition satisfied.
		// But since it is the ball owner that does all decisions, this may not be a question...

		// Check if this player has ball possession
		vector<const fcportugal::setplay::PlayerReference*> vectorMe;
		vectorMe.push_back(new fcportugal::setplay::PlayerID("our",worldModel->self().unum()));

		if(!lookup(fcportugal:: setplay::CondBallOwner(vectorMe)))
				return false;

		// Check is this player is in the from list
		bool amInFromList=false;
		vector<const fcportugal::setplay::PlayerReference*> players_from=cond.getFromPlayerSet();

		for(unsigned int i=0; i<players_from.size();i++)
			if(players_from.at(i)->value()->number==(unsigned int)worldModel->self().unum()){
				amInFromList=true;
				break;
			}

		if(!amInFromList)
			return false;


		KickTable::Sequence sequence;
		bool success = KickTable::instance().simulate(*worldModel,
				Vector2D(cond.getToRegion()->getCentralPoint(*this)->getX()->value(),
						cond.getToRegion()->getCentralPoint(*this)->getY()->value()),
						//TODO What values to choose?!?
						2.7, // first_speed,
						2.7,// allowable_speed)
						10,//max_step
						sequence);

	return success;
}

bool Context::lookup( const fcportugal::setplay::CondNamed& /*cond*/ ) const {
	// Error! Not really defined.
	return 3/0>3;
}

int Context::ourScore() const{
	if(worldModel->ourSide()==LEFT)
		return worldModel->gameMode().scoreLeft();
	else
		return worldModel->gameMode().scoreRight();
}


int Context::oppScore() const{
	if(worldModel->ourSide()==RIGHT)
		return worldModel->gameMode().scoreLeft();
	else
		return worldModel->gameMode().scoreRight();
}

bool Context::lookup( const fcportugal::setplay::CondOppGoal& cond ) const{
	return cond.getComp().compare(oppScore(),cond.getValue());
}
bool Context::lookup( const fcportugal::setplay::CondOurGoal& cond ) const{
	return cond.getComp().compare(ourScore(),cond.getValue());
}
bool Context::lookup( const fcportugal::setplay::CondGoalDiff& cond ) const{
	return cond.getComp().compare(ourScore()-oppScore(),cond.getValue());
}


bool Context::lookup( const fcportugal::setplay::CondNearOffsideLine& cond ) const{
	const vector<const fcportugal::setplay::PlayerReference*> players=cond.getPlayerSet();

	for(unsigned int i=0; i!= players.size();i++){
		const AbstractPlayerObject* p;

		if(players.at(i)->value()->team=="our")
			p=worldModel->ourPlayer(players.at(i)->value()->number);
		else
			p=worldModel->theirPlayer(players.at(i)->value()->number);

		if ( p->side() == worldModel->self().side() ){
			return p->pos().x > worldModel->offsideLineX();
		}
		else if ( p->side() == NEUTRAL ){
			return false;
		}
		else{
			return p->pos().x < bound( worldModel->ball().pos().x, worldModel->ourDefenseLineX(), 0.0 );
		}
	}

	// Default...
	return false;
}

const fcportugal::setplay::PlayerID& Context::me() const{
	return *(new fcportugal::setplay::PlayerID("our",worldModel->self().unum()));
}

double Context::time() const{
	return worldModel->time().cycle();
}
const fcportugal::setplay::PointSimple* Context::ballPos() const{
	return  new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(worldModel->ball().pos().x),
			new fcportugal::setplay::Decimal(worldModel->ball().pos().y));
}

const fcportugal::setplay::PointSimple* Context::playerPos(const fcportugal::setplay::PlayerID& player) const{
	if(player.team=="our"){
		const AbstractPlayerObject* apo=worldModel->ourPlayer(player.number);
		if(apo==NULL)
			return NULL;
		return new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(apo->pos().x),
				new fcportugal::setplay::Decimal(apo->pos().y));
	}
	else{
		const AbstractPlayerObject* apo=worldModel->theirPlayer(player.number);
		if(apo==NULL)
					return NULL;
		return  new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(apo->pos().x),
				new fcportugal::setplay::Decimal(apo->pos().y));
	}
}

double Context::fieldLength() const{
	return ServerParam::i().pitchLength();
}

double Context::fieldWidth() const{
	return ServerParam::i().pitchWidth();
}

rcss::clang::PlayMode Context::playMode() const{
	switch(worldModel->gameMode().type()){
	case rcsc::GameMode::BeforeKickOff: return rcss::clang::BeforeKickOff;
	case rcsc::GameMode::TimeOver: return rcss::clang::TimeOver;
	case rcsc::GameMode::PlayOn: return rcss::clang::PlayOn;
	case rcsc::GameMode::KickOff_:    // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::KickOff_Our;
		else
			return rcss::clang::KickOff_Opp;
	case rcsc::GameMode::KickIn_:     // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::KickIn_Our;
		else
			return rcss::clang::KickIn_Opp;

	case rcsc::GameMode::FoulCharge_: // Left | Right
	case rcsc::GameMode::FoulPush_:   // Left | Right
	case rcsc::GameMode::FoulMultipleAttacker_: // Left | Right
	case rcsc::GameMode::FoulBallOut_:    // Left | Right
	case rcsc::GameMode::BackPass_:       // Left | Right
	case rcsc::GameMode::FreeKickFault_: // Left | Right
	case rcsc::GameMode::CatchFault_: // Left | Right
	case rcsc::GameMode::OffSide_:    // Left | Right
	case rcsc::GameMode::FreeKick_:   // Left | Right
	case rcsc::GameMode::PenaltyKick_:  // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::FreeKick_Our;
		else
			return rcss::clang::FreeKick_Opp;
	case rcsc::GameMode::CornerKick_: // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			 return  rcss::clang::CornerKick_Our;
		else
			 return  rcss::clang::CornerKick_Opp;
	case rcsc::GameMode::GoalKick_:   // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::GoalKick_Our;
		else
			return rcss::clang::GoalKick_Opp;
	case rcsc::GameMode::AfterGoal_:  // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::AfterGoal_Our;
		else
			return rcss::clang::AfterGoal_Opp;
	        //Drop_Ball,   // Left | Right
	case rcsc::GameMode::IndFreeKick_: // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
		    return rcss::clang::Ind_FreeKick_Our;
		else
		    return rcss::clang::Ind_FreeKick_Opp;
	case rcsc::GameMode::GoalieCatch_: // Left | Right
		if(worldModel->ourSide()==worldModel->gameMode().side())
			return rcss::clang::GoalieCatch_Our;
		else
			return rcss::clang::GoalieCatch_Opp;


		// Gamemodes with no correspondence
	case rcsc::GameMode::FirstHalfOver:
	case rcsc::GameMode::Pause:
	case rcsc::GameMode::Human:

	case rcsc::GameMode::PenaltySetup_: // Left | Right
	case rcsc::GameMode::PenaltyReady_: // Left | Right
	case rcsc::GameMode::PenaltyTaken_: // Left | Right
	case rcsc::GameMode::PenaltyMiss_: // Left | Right
	case rcsc::GameMode::PenaltyScore_: // Left | Right

		// these are not a real playmode
	case rcsc::GameMode::PenaltyOnfield_: // next real playmode is PenaltySetup_
	case rcsc::GameMode::PenaltyFoul_:    // next real playmode is PenaltyMiss_ or PenaltyScore_
	        //PenaltyWinner_,  // next real playmode is TimeOver
	        //PenaltyDraw,     // next real playmode is TimeOver


	case rcsc::GameMode::ExtendHalf:
	    //???
	case rcsc::GameMode::MODE_MAX:
	default:
		return rcss::clang::Invalid;
	}
}
const string& Context::opponentName() const{
	return worldModel->opponentTeamName();
}
unsigned int Context::numPlayersPerTeam() const{
	return 11;
}

const fcportugal::setplay::NameSubstitutions& Context::nameSubstitutions() const{

	fcportugal::setplay::NameSubstitutions* res=new fcportugal::setplay::NameSubstitutions();

	// Aux vars
	double pitch_length=rcsc::ServerParam::i().pitchLength();
	double pitch_width=rcsc::ServerParam::i().pitchWidth();
	double penalty_area_length=rcsc::ServerParam::i().penaltyAreaLength();
	double penalty_area_width=rcsc::ServerParam::i().penaltyAreaWidth();
	// Definition of all the relevant points

	// Corners
	fcportugal::setplay::PointSimple* ltc
	= new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
			new fcportugal::setplay::Decimal(-pitch_width/2));


	  fcportugal::setplay::PointSimple* rtc =
			  new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					  new fcportugal::setplay::Decimal(pitch_width/2));

	  fcportugal::setplay::PointSimple* loc = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					     new fcportugal::setplay::Decimal(-pitch_width/2));


	  fcportugal::setplay::PointSimple* roc =  new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					      new fcportugal::setplay::Decimal(pitch_width/2));

	  // Left points
	  fcportugal::setplay::PointSimple* lt23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/3),
					      new fcportugal::setplay::Decimal(-pitch_width/2));


	  fcportugal::setplay::PointSimple* lt13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/6),
					      new fcportugal::setplay::Decimal(-pitch_width/2));

	  fcportugal::setplay::PointSimple* lm = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(0),
					    new fcportugal::setplay::Decimal(-pitch_width/2));


	  fcportugal::setplay::PointSimple* lo23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/3),
					      new fcportugal::setplay::Decimal(-pitch_width/2));


	  fcportugal::setplay::PointSimple* lo13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/6),
					      new fcportugal::setplay::Decimal(-pitch_width/2));


	  // Right points
	  fcportugal::setplay::PointSimple* rt23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/3),
					      new fcportugal::setplay::Decimal(pitch_width/2));


	  fcportugal::setplay::PointSimple* rt13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/6),
					      new fcportugal::setplay::Decimal(pitch_width/2));

	  fcportugal::setplay::PointSimple* rm = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(0),new fcportugal::setplay::Decimal(pitch_width/2));


	  fcportugal::setplay::PointSimple* ro23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/3),
					      new fcportugal::setplay::Decimal(pitch_width/2));


	  fcportugal::setplay::PointSimple* ro13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/6),
					      new fcportugal::setplay::Decimal(pitch_width/2));

	  // Back points
	  fcportugal::setplay::PointSimple* bl13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					      new fcportugal::setplay::Decimal(-pitch_width/6));

	  fcportugal::setplay::PointSimple* bl23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					      new fcportugal::setplay::Decimal(-pitch_width/3));

	  fcportugal::setplay::PointSimple* br13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					      new fcportugal::setplay::Decimal(pitch_width/6));

	  fcportugal::setplay::PointSimple* br23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					      new fcportugal::setplay::Decimal(pitch_width/3));


	  fcportugal::setplay::PointSimple* og = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					    new fcportugal::setplay::Decimal(0));

	  // Top points
	  fcportugal::setplay::PointSimple* tl13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					      new fcportugal::setplay::Decimal(-pitch_width/6));

	  fcportugal::setplay::PointSimple* tl23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					      new fcportugal::setplay::Decimal(-pitch_width/3));

	  fcportugal::setplay::PointSimple* tr13 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					      new fcportugal::setplay::Decimal(pitch_width/6));

	  fcportugal::setplay::PointSimple* tr23 = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					      new fcportugal::setplay::Decimal(pitch_width/3));


	  fcportugal::setplay::PointSimple* tg = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					    new fcportugal::setplay::Decimal(0));

	  // Penalty area points
	  fcportugal::setplay::PointSimple* ctgal = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2
							   -penalty_area_length),
					       new fcportugal::setplay::Decimal(-penalty_area_width/2));

	  fcportugal::setplay::PointSimple* ttgar = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
					       new fcportugal::setplay::Decimal(penalty_area_width/2));

	  fcportugal::setplay::PointSimple* cogar = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2
							   +penalty_area_length),
					       new fcportugal::setplay::Decimal(penalty_area_width/2));

	  fcportugal::setplay::PointSimple* bogal = new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					       new fcportugal::setplay::Decimal(-penalty_area_width/2));

	  //field
	  fcportugal::setplay::Region* reg  = new fcportugal::setplay::RegRec(loc, rtc);
	  res->addSubstitution("field",reg);


	  //mid fields
	  reg=new fcportugal::setplay::RegRec(loc,rm);

	  res->addSubstitution("our_middle_field",reg);


	  reg=new fcportugal::setplay::RegRec(lm,rtc);

	  res->addSubstitution("their_middle_field",reg);


	  //penalty boxes
	  reg=new fcportugal::setplay::RegRec(ctgal,ttgar);

	  res->addSubstitution("their_penalty_box",reg);


	  reg=new fcportugal::setplay::RegRec(bogal,cogar);

	  res->addSubstitution("our_penalty_box",reg);

	  //left and right...
	  reg = new fcportugal::setplay::RegRec(loc,tg);
	  res->addSubstitution("left",reg);

	  reg = new fcportugal::setplay::RegRec(og,rtc);
	  res->addSubstitution("right",reg);

	  // Added 1 meter for kick-ins and corners...
	  reg = new fcportugal::setplay::RegRec( new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2),
					     new fcportugal::setplay::Decimal(-pitch_width/2-1)),
			    tl23);
	  res->addSubstitution("far_left",reg);

	  reg = new fcportugal::setplay::RegRec(bl23,tl13);
	  res->addSubstitution("mid_left",reg);

	  reg = new fcportugal::setplay::RegRec(bl13,tg);
	  res->addSubstitution("centre_left",reg);

	  reg = new fcportugal::setplay::RegRec(og,tr13);
	  res->addSubstitution("centre_right",reg);

	  reg = new fcportugal::setplay::RegRec(br13,tr23);
	  res->addSubstitution("mid_right",reg);

	  // Added 1 meters for kick-ins and corners...
	  reg = new fcportugal::setplay::RegRec(br23,new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2),
						new fcportugal::setplay::Decimal(pitch_width/2+1)));
	  res->addSubstitution("far_right",reg);

	  // Fronts and backs...
	  reg = new fcportugal::setplay::RegRec(loc,ro23);
	  res->addSubstitution("our_back",reg);

	  reg = new fcportugal::setplay::RegRec(lo23,ro13);
	  res->addSubstitution("our_middle",reg);

	  reg = new fcportugal::setplay::RegRec(lo13,rm);
	  res->addSubstitution("our_front",reg);

	  reg = new fcportugal::setplay::RegRec(lm,rt13);
	  res->addSubstitution("their_front",reg);

	  reg = new fcportugal::setplay::RegRec(lt13,rt23);
	  res->addSubstitution("their_middle",reg);

	  reg = new fcportugal::setplay::RegRec(lt23,rtc);
	  res->addSubstitution("their_back",reg);

	  // Slices/radial regions
	  reg = new fcportugal::setplay::RegTri(ltc,tg,lt23);
	  res->addSubstitution("sl_1",reg);

	  reg = new fcportugal::setplay::RegTri(lt23,tg,lt13);
	  res->addSubstitution("sl_2",reg);

	  reg = new fcportugal::setplay::RegTri(lt13,tg,lm);
	  res->addSubstitution("sl_3",reg);

	  reg = new fcportugal::setplay::RegTri(lm,tg,lo13);
	  res->addSubstitution("sl_4",reg);

	  reg = new fcportugal::setplay::RegTri(lo13,tg,lo23);
	  res->addSubstitution("sl_5",reg);

	  reg = new fcportugal::setplay::RegTri(lo23,tg,loc);
	  res->addSubstitution("sl_6",reg);

	  reg = new fcportugal::setplay::RegTri(loc,tg,bl23);
	  res->addSubstitution("sl_7",reg);

	  reg = new fcportugal::setplay::RegTri(bl23,tg,bl13);
	  res->addSubstitution("sl_8",reg);

	  reg = new fcportugal::setplay::RegTri(bl13,tg,og);
	  res->addSubstitution("sl_9",reg);

	  reg = new fcportugal::setplay::RegTri(og,tg,br13);
	  res->addSubstitution("sr_9",reg);

	  reg = new fcportugal::setplay::RegTri(br13,tg,br23);
	  res->addSubstitution("sr_8",reg);

	  reg = new fcportugal::setplay::RegTri(br23,tg,roc);
	  res->addSubstitution("sr_7",reg);

	  reg = new fcportugal::setplay::RegTri(roc,tg,ro23);
	  res->addSubstitution("sr_6",reg);

	  reg = new fcportugal::setplay::RegTri(ro23,tg,ro13);
	  res->addSubstitution("sr_5",reg);

	  reg = new fcportugal::setplay::RegTri(ro13,tg,rm);
	  res->addSubstitution("sr_4",reg);

	  reg = new fcportugal::setplay::RegTri(rm,tg,rt13);
	  res->addSubstitution("sr_3",reg);

	  reg = new fcportugal::setplay::RegTri(rt13,tg,rt23);
	  res->addSubstitution("sr_2",reg);

	  reg = new fcportugal::setplay::RegTri(rt23,tg,rtc);
	  res->addSubstitution("sr_1",reg);

	  //left and right... Add always one meter for kick-ins...
	  reg = new fcportugal::setplay::RegRec(new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2-1),
					   new fcportugal::setplay::Decimal(-pitch_width/2-1)),
			   new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2+1),
					   new fcportugal::setplay::Decimal(0)));
	  res->addSubstitution("left",reg);


	  reg = new fcportugal::setplay::RegRec(new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(-pitch_length/2-1),
					   new fcportugal::setplay::Decimal(0)),
			   new fcportugal::setplay::PointSimple(new fcportugal::setplay::Decimal(pitch_length/2+1),
					   new fcportugal::setplay::Decimal(pitch_width/2+1)));
	  res->addSubstitution("right",reg);

	  return *res;
}
