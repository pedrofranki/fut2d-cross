// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "role_fcportugal_setplay.h"

#include "fcportugal_setplay_exception.h"

#include <setplay/setplaymanager.h>

#include "strategy.h"

#include "bhv_basic_offensive_kick.h"
#include "bhv_basic_move.h"
#include "bhv_go_to_static_ball.h"

#include <rcsc/formation/formation.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/neck_turn_to_player_or_scan.h>
#include <rcsc/action/neck_scan_players.h>

#include <rcsc/action/neck_turn_to_ball.h>

#include <rcsc/action/body_turn_to_ball.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_clear_ball.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/bhv_shoot.h>
#include <rcsc/action/bhv_scan_field.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

using namespace rcsc;
using namespace fcportugal::setplay;

const std::string RoleFCPortugalSetplay::NAME( "FCPortugalSetplay" );

/*-------------------------------------------------------------------*/
/*!

 */
namespace {
rcss::RegHolder role = SoccerRole::creators().autoReg( &RoleFCPortugalSetplay::create,
		RoleFCPortugalSetplay::NAME );
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
RoleFCPortugalSetplay::execute( PlayerAgent * agent )
{
	// The agent invoking this method must be an instance of FCPortugalSetplayPlayer
	player= (FCPortugalSetplayPlayer*) agent;
	SetplayManager* setplayManager=player->getSetplayManager();

    const WorldModel&  wm =player->getContext().getWorldModel();

    setplayManager->updateInternalState(player->getContext());


	if ( !setplayManager->isSetplayActive() )
		// This should only be RoleFCPortugalSetplay::executed when a Setplay is active, which is tested in fcportugal_setplay_player
		throw FCPortugalSetplayException("Trying to RoleFCPortugalSetplay::execute a Setplay without one being active!");

	const Action * action=setplayManager->currentAction(player->getContext());

	if(action!=NULL)

	if(action && action->active
            && !action->isAccomplished(player->getContext().me(),player->getContext())){
        action->getExecuted(*this);
    }
    else
    {   if(setplayManager->getCurrentSetplay().amLeadPlayer(player->getContext())
           &&
           (
            ((setplayManager->getCurrentSetplay().currentStepNumber()!=0)&&(!player->world().self().goalie()))
              || ////JAF2014: Not executing HoldBall if I'm starting the setplay - state 0....
            ((setplayManager->getCurrentSetplay().currentStepNumber()>1)&&(player->world().self().goalie()))
           ) //JAF2014: goalie: can be in state 0 or 1....
          ){

            Body_HoldBall().execute(player);
        }
        else
        {
            // Move to setplay position, if it was not reached...
            const Point* pos=setplayManager->myCurrentPositioning(player->getContext());

            if(pos!=NULL && Vector2D(pos->asPointSimple(player->getContext())->getX()->value(),
                                     pos->asPointSimple(player->getContext())->getY()->value()).dist(wm.self().pos())>1.5)
            {
                execute(ActPos(new const RegPoint(pos)));
            }
            else{// Scan field...
                (new Bhv_ScanField())->execute(player);
                player->debugClient().addMessage("Scanning field.");
            }
        }

	// Neck actions, done independently from body actions.
	if(action!=NULL){
		// check if there are relevant players
		const vector<const PlayerReference*> relevantPlayers=action->relevantPlayers();
		if(relevantPlayers.size()>0){
			// Look at first player...
			(new Neck_TurnToPlayerOrScan(wm.ourPlayer(relevantPlayers.at(0)->value()->number)))->execute(player);
			player->debugClient().addMessage("Looking at:"+relevantPlayers.at(0)->value()->number);
		}
            else{
                // Simply look to Ball(in place of scan field //JAF2014 26032014!!!
                (new Neck_TurnToBall())->execute(player);
                player->debugClient().addMessage("Looking to Ball...");

                // Simply scan field
//                (new Neck_ScanPlayers())->execute(player);
//                player->debugClient().addMessage("Scanning players...");
            }
        }
    }

	return true;
}

/*-------------------------------------------------------------------*/

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActPos& a ){
	const PointSimple* targetPos=
			a.getRegion()->getCentralPoint(player->getContext())->asPointSimple(player->getContext());


	// Code adapted from bhv_basic_move.cpp
	const Vector2D target_point(targetPos->getX()->value(),targetPos->getY()->value());
	const double dash_power = Strategy::get_normal_dash_power( player->world() );

	// Must check if it is a goalie catch, in which case the goalie can warp
	if(player->world().self().goalie()
			&& player->world().gameMode().type()==rcsc::GameMode::GoalieCatch_){

			player->debugClient().addMessage( "ActPos MOVING");
			player->debugClient().setTarget( target_point );
			player->debugClient().addCircle( target_point, 1 );

			player->doMove(target_point.x,target_point.y);

			return;
		}
	double dist_thr = player->world().self().pos().dist(target_point) * 0.1;
	if ( dist_thr < 1.0 ) dist_thr = 1.0;


	player->debugClient().addMessage( "ActPos%.0f", dash_power );
	player->debugClient().setTarget( target_point );
	player->debugClient().addCircle( target_point, dist_thr );

    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power ).execute( player ) ){
	        Body_TurnToBall().execute( player );
	}

}

/* Bhv_BasicMove::Execute - Copied by JAF2014 04/04

bool
Bhv_BasicMove::execute( PlayerAgent * agent )
{
    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove" );

    //-----------------------------------------------
    // tackle
    if ( Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )
    {
        return true;
    }

    const WorldModel & wm = agent->world();
    //--------------------------------------------------------
    // chase ball
    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( ! wm.existKickableTeammate()
         && ( self_min <= 3
              || ( self_min <= mate_min
                   && self_min < opp_min + 3 )
              )
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        Body_Intercept().execute( agent );
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;
    }

    const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    const double dash_power = Strategy::get_normal_dash_power( wm );

    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
    }

    if ( wm.existKickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new Neck_TurnToBall() );
    }
    else
    {
        agent->setNeckAction( new Neck_TurnToBallOrScan() );
    }

    return true;
}

*/

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActForward& a ){
	const WorldModel&  wm =player->getContext().getWorldModel();
	// Must check if ball is kickable...
	if(! wm.self().isKickable()){
		// Code adapted from bhv_set_play_kick_in.cpp
		AngleDeg ball_place_angle = ( wm.ball().pos().y > 0.0
		                                  ? -90.0
		                                  : 90.0 );

		player->debugClient().addMessage( "ActForward: approaching ball");

		Bhv_GoToStaticBall( ball_place_angle ).execute( player ) ;
		return;
	}

	const PointSimple* targetPos=
			a.getRegion()->getCentralPoint(player->getContext())->asPointSimple(player->getContext());

	// Code adapted from bhv_set_play_kick_in.cpp
	Vector2D target_point(targetPos->getX()->value(),targetPos->getY()->value());

	const double max_ball_speed = player->world().self().kickRate() * ServerParam::i().maxPower();

	target_point.x += 0.5;

	double ball_move_dist = player->world().ball().pos().dist( target_point );
	int ball_reach_step
	= static_cast< int >( std::ceil( calc_length_geom_series( max_ball_speed,
			ball_move_dist,
			ServerParam::i().ballDecay() ) ) );
	double ball_speed = 0.0;
	if ( ball_reach_step > 3 ){
		ball_speed = calc_first_term_geom_series( ball_move_dist,
				ServerParam::i().ballDecay(),
				ball_reach_step );
	}
	else{
		ball_speed = calc_first_term_geom_series_last( 1.4,
				ball_move_dist,
				ServerParam::i().ballDecay() );
		ball_reach_step
		= static_cast< int >( std::ceil( calc_length_geom_series( ball_speed,
				ball_move_dist,
				ServerParam::i().ballDecay() ) ) );
	}

	ball_speed = std::min( ball_speed, max_ball_speed );

	player->debugClient().addMessage( "ActForward%.3f", ball_speed );
	player->debugClient().setTarget( target_point );


	// Originally in bhv_set_play_kick_in.cpp
	/*Body_KickOneStep( target_point,
			ball_speed
	).execute( player );*/

	// TODO
	// What is the third argument? first_speed_thr?
	Body_SmartKick(target_point,ball_speed,ball_speed,10).execute( player );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActPass& a ){

    const WorldModel&  wm =player->getContext().getWorldModel();//JAF2014 Trying to Hold Ball before it passes by...
    dlog.addText( Logger::PLAN, __FILE__": SP: Entered ActPass: Ball Velocity = %f",wm.ball().vel().length() );

    if (wm.self().isKickable()&&wm.ball().vel().length()>0.1){//JAF2014 Trying to Hold Ball before it passes by...
        dlog.addText( Logger::PLAN, __FILE__": SP: ActPass: trying to acquire control of ball(HoldBall)" );

        Body_HoldBall().execute(player);
        return;
    }

    // Could not figure best way to do pass, therefore will simplify things,
    // and call ActForward to the position of first player in to_list
    const PlayerReference* pr= a.getPlayers().at(0);

    const Point * playerPoint = NULL;
    if (pr!=NULL)
       playerPoint = player->getContext().playerPos(*(pr->value()));
    //JAF2014 28032014 - Causing Crash????Accessing value of non instantiated PlayerRole???

	if(playerPoint!=NULL){

        player->debugClient().addMessage( "SP: ActPass to %d", a.getPlayers().at(0)->value()->number);
        if(wm.ball().vel().length()<1.0f)
            execute(ActForward(new RegPoint(playerPoint)));
        else
            dlog.addText( Logger::PLAN,	__FILE__"SP: ActPass to %d already in progress..waiting...", a.getPlayers().at(0)->value()->number);

    }
	else{// Pass to SetplayPosition, if possible
		playerPoint = player->getSetplayManager()->currentPositioning(a.getPlayers().at(0)->value()->number,
				player->getContext());
		if(playerPoint!=NULL){
                player->debugClient().addMessage( "SP:ActPass to %d's Setplay position", a.getPlayers().at(0)->value()->number);
                if(wm.ball().vel().length()<1.0f)
                    execute(ActForward(new RegPoint(playerPoint)));
                else
                    dlog.addText( Logger::PLAN,	__FILE__"SP: ActPass to %d's Setplay position already in progress..waiting...", a.getPlayers().at(0)->value()->number);

        }
		else{
            player->debugClient().addMessage( "SP:ActPass to %d FAILED: unknown position", a.getPlayers().at(0)->value()->number);
            dlog.addText( Logger::PLAN,	__FILE__"SP:  ActPass  to %d FAILED: unknown position", a.getPlayers().at(0)->value()->number);
		}
	}
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActMark& /*a*/ ){
    player->debugClient().addMessage( "SP:ActMark NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActMarkLinePlayer& /*a*/ ){
    player->debugClient().addMessage( "SP:ActMarkLinePlayer NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActMarkLineReg& /*a*/ ){
    player->debugClient().addMessage( "SP:ActMarkLineReg NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActDribble& a ){
	const Region* dest= a.getRegion();
	const PointSimple* destPoint=dest->getCentralPoint(player->getContext());

    player->debugClient().addMessage( "SP:ActDribble" );

	Body_Dribble(Vector2D(destPoint->getX()->value(),destPoint->getY()->value()),//target_point
			10,//dist_thr
			5,//dash_power
			5,//dash_count
			true//dodge
			).execute(player);

}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActOffsideLine& /*a*/ ){
    player->debugClient().addMessage( "SP:ActOffsideLine NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActClear& /*a*/ ){
    player->debugClient().addMessage( "SP:ActClear" );

	Body_ClearBall().execute(player);
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActShoot& /*a*/ ){
	const WorldModel&  wm =player->getContext().getWorldModel();
	// Must check if ball is kickable...
	if(! wm.self().isKickable()){
		// Code adapted from bhv_set_play_kick_in.cpp
		AngleDeg ball_place_angle = ( wm.ball().pos().y > 0.0
				? -90.0
						: 90.0 );

		player->debugClient().addMessage( "ActShoot: approaching ball");
        dlog.addText( Logger::PLAN,
                __FILE__":  SP: ActShoot: approaching ball" );

		Bhv_GoToStaticBall( ball_place_angle ).execute( player ) ;
		return;
	}

	// Regular shot
	player->debugClient().addMessage( "ActShoot" );
    dlog.addText( Logger::PLAN,	__FILE__":  SP: ActShoot ");

	Bhv_Shoot().execute(player);
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActHold& /*a*/ ){
	player->debugClient().addMessage( "ActHold" );
    dlog.addText( Logger::PLAN,	__FILE__": SP: ActHold ");

	Body_HoldBall().execute(player);
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActIntercept& /*a*/ ){
//    player->debugClient().addMessage( "ActIntercept NOT IMPLEMENTED!"  );
//    dlog.addText( Logger::PLAN,	__FILE__": SP: Intercept NOT IMPLEMENTED ");
    const WorldModel&  wm =player->getContext().getWorldModel();

    (new Neck_TurnToBall())->execute(player);//JAF2014 - Look to the ball to estimate distance!!!!
    player->debugClient().addMessage("Looking to Ball...");


	// Must check if ball is kickable... If it is near enough, try to approach it...
	if(!wm.self().isKickable() ){//&& wm.ball().distFromSelf()<10){//JAF2014 changed from ReceiveBall..

		player->debugClient().addMessage( "ActIntercept: actively approaching ball . Ball vel:%f.0",
				wm.ball().vel().length());

        Body_Intercept(false).execute(player);

        return;
    }
    else if (wm.self().isKickable()){
        dlog.addText( Logger::PLAN, __FILE__": SP: ActIntercept: trying to acquire control of ball(HoldBall)" );

        Body_HoldBall().execute(player);
        return;
    }

}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActTackle& /*a*/ ){
	player->debugClient().addMessage( "ActTackle NOT IMPLEMENTED!"  );

}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActStop& /*a*/ ){
	player->debugClient().addMessage( "ActStop NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActAttentionToReg& /*a*/ ){
	player->debugClient().addMessage( "ActAttentionToReg NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActAttentionToObj& /*a*/ ){
	player->debugClient().addMessage( "ActAttentionToObj NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActMarkGoal& /*a*/ ){
	player->debugClient().addMessage( "ActMarkGoal NOT IMPLEMENTED!"  );
}

void RoleFCPortugalSetplay::execute( const fcportugal::setplay::ActReceiveBall& /*a*/ ){
	const WorldModel&  wm =player->getContext().getWorldModel();
	// Must check if ball is kickable... If it is near enough, try to approach it...
	if(! wm.self().isKickable() && wm.ball().distFromSelf()<6){

    (new Neck_TurnToBall())->execute(player);//JAF2014 - Look to the ball to estimate distance!!!!
    player->debugClient().addMessage("Looking to Ball...");


		// Must check if ball is kickable... If it is near enough, try to approach it...
		if(!wm.self().isKickable() && wm.ball().distFromSelf()<10){//JAF2014 changed from 6 to 10...

		player->debugClient().addMessage( "ActReceiveBall: actively approaching ball . Ball vel:%f.0",
				wm.ball().vel().length());

        Body_Intercept(false).execute(player);

			return;
		}
		else if (wm.self().isKickable()){

			Body_HoldBall().execute(player);
			return;
		}
		player->debugClient().addMessage( "ActReceiveBall: passively approaching ball ");
	}
}

void RoleFCPortugalSetplay::execute(const fcportugal::setplay::ActMoveToOffsideLine& /*a*/){
	execute(ActPos(new const RegPoint(new PointSimple(new Decimal(player->world().offsideLineX()),
			new Decimal(player->world().self().pos().y)))));
}


// LMOTA Does not seem necessary.
//void
//RoleFCPortugalSetplay::doKick( PlayerAgent * agent )
//{
//	switch ( Strategy::get_ball_area( agent->world().ball().pos() ) ) {
//	case Strategy::BA_CrossBlock:
//	case Strategy::BA_Stopper:
//	case Strategy::BA_Danger:
//	case Strategy::BA_DribbleBlock:
//	case Strategy::BA_DefMidField:
//	case Strategy::BA_DribbleAttack:
//	case Strategy::BA_OffMidField:
//	case Strategy::BA_Cross:
//	case Strategy::BA_ShootChance:
//	default:
//		Bhv_BasicOffensiveKick().execute( agent );
//		break;
//	}
//}
//
///*-------------------------------------------------------------------*/
///*!
//
// */
// void
// RoleFCPortugalSetplay::doMove( PlayerAgent * agent )
// {
//	 switch ( Strategy::get_ball_area( agent->world() ) ) {
//	 case Strategy::BA_CrossBlock:
//	 case Strategy::BA_Stopper:
//	 case Strategy::BA_Danger:
//	 case Strategy::BA_DribbleBlock:
//	 case Strategy::BA_DefMidField:
//	 case Strategy::BA_DribbleAttack:
//	 case Strategy::BA_OffMidField:
//	 case Strategy::BA_Cross:
//	 case Strategy::BA_ShootChance:
//	 default:
//		 Bhv_BasicMove().execute( agent );
//		 break;
//	 }
// }
