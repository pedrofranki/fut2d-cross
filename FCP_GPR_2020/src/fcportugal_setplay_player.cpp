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

#include "fcportugal_setplay_player.h"
#include "role_fcportugal_setplay.h"
#include <rcsc/player/free_message.h>
#include <rcsc/common/basic_client.h>
#include <setplay/setplaymessage.h>
#include <setplay/setplayexception.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

using namespace rcsc;
using namespace std;
using namespace fcportugal::setplay;
/*-------------------------------------------------------------------*/
/*!

 */
FCPortugalSetplayPlayer::FCPortugalSetplayPlayer(bool useCBR)
: SamplePlayer(),context(world()),useCBR(useCBR),
  roleSetplay(RoleFCPortugalSetplay::create()){
	if(useCBR)
		setplayManager=new fcportugal::setplay::SetplayManagerWithCBR(config().teamName(),//teamName
				SETPLAY_CONF_FILE,//conf_file
				true,// useComm
				true, //limitedComm
				5, //messageRepeatTime
				context, // world
				CBR_CONF_FILE, // cbrFile
				true,// writeCBRToFile
				true,//writeCBRToFileAfterExecution
				4,//weightThisGame
				2,//weightThisTeam
				1// weightOtherTeams
		);
	else
		setplayManager=new fcportugal::setplay::SetplayManager(config().teamName(),//teamName
				SETPLAY_CONF_FILE,//conf_file
				true,// useComm
				true, //limitedComm
				5, //messageRepeatTime
				context);
	cerr<<"SP: loaded "<<setplayManager->numberOfSetplays()<<" setplays"<<endl; //INFO SP
}

/*-------------------------------------------------------------------*/
/*!

 */
FCPortugalSetplayPlayer::~FCPortugalSetplayPlayer(){

}

// Aux function to decode messages, used in actionImpl
string decodeInitMessage(string ori){
	// first char is "S", to mark as Init message
	string res="S";
	// SetplayNumber will be sent as 2-digit number
	res.append(ori.substr(1,3));
	// Only considering players, not parameters
	// each player number will be a single char, either a single digit,
	// or 'a' for 10 and 'b' for 11
	for(unsigned int i=4;i<ori.size();i++)
		if(ori.at(i)=='a')
			res.append(" 10");
		else if(ori.at(i)=='b')
			res.append(" 11");
		else
			res.append(" ").append(ori.substr(i,1));

	return res;

	// Initial solution, using ostrstream
	// Was inserting a strange char at the end...
//	ostrstream oss;
//	// first char is "S", to mark as Init message
//	oss<<"S";
//	// SetplayNumber will be sent as 2-digit number
//	oss<<ori.substr(1,2);
//	// Only considering players, not parameters
//	// each player number will be a single char, either a single digit,
//	// or 'a' for 10 and 'b' for 11
//	for(unsigned int i=3;i<ori.size();i++)
//		if(ori.at(i)=='a')
//			oss<<" 10";
//		else if(ori.at(i)=='b')
//			oss<<" 11";
//		else
//			oss<<" "<<ori.at(i);
//
//	// was inserting a strange character at the end...
//	oss.flush();
//
//	return oss.str();
}

/*-------------------------------------------------------------------*/
/*!
  main decision
  virtual method in super class
 */
void
FCPortugalSetplayPlayer::actionImpl(){
	// Check for message: a Setplay might have been started by another player...
    if( (world().audioMemory().freeMessage().size()>0) &&
            world().audioMemory().freeMessageTime().cycle() == world().time().cycle() )//JAF2014 to stop repeating FreeMessage...
		for(unsigned int i=0; i<world().audioMemory().freeMessage().size();i++){

            //Process setplay message. Must trim due to trailing spaces...
			string message=world().audioMemory().freeMessage().at(i).message_;

            trim(message);
			if(message.at(0)=='S'){// Init message, must decode
				message=decodeInitMessage(message);

            }
			setplayManager->processReceivedMessage(message,
					context);

//            }
	}

	vector<const PlayerReference*> meInVector;
	meInVector.push_back(&(context.me()));

	// Will try to activate Setplay only if none is active and I am the ball owner...
    if(!setplayManager->isSetplayActive()
			&& this->world().self().distFromBall() < BALL_OWNER_DISTANCE_THRESHOLD
			&& CondBallOwner(meInVector).eval(context)){
		if(useCBR){
			((fcportugal::setplay::SetplayManagerWithCBR*)setplayManager)->autoActivateSetplay(context);
        }
		else{
			vector<const fcportugal::setplay::Setplay *>* feasibleSetplays
			= setplayManager->feasibleSetplays(context);

			// check if there is a Setplay to start
			if(feasibleSetplays->size()>0){
				// TODO activate some Setplay
			}
		}
	}

	if(setplayManager->isSetplayActive()){
		setplayManager->updateInternalState(context);
		// Setplay management
		// Check again: Setplay might have ended...
		if(setplayManager->isSetplayActive() && setplayManager->thisPlayerParticipates(context)){

			// Setplay management...

			// Will check if there are feasible transitions to choose from...
			vector<const Transition*>* transitions
			= setplayManager->possibleTransitions(context);

			// Will simply look at all the available transitions and choose the first
			// valid one...
			for(unsigned int i=0; transitions!=NULL && i != transitions->size(); i++)
				if(transitions->at(i)!=NULL && !setplayManager->isNextStepAlreadyChosen() ){
					setplayManager->chosenTransition(i,context);
					break; // Ignore remaining transitions...
				}
		}
	}

    // Setplay execution
	//Must check is setplay was not ended by message...
	if(setplayManager->isSetplayActive()){
        setplayManager->updateInternalState(context);


		if(setplayManager->thisPlayerParticipates(context)){
			roleSetplay->execute(this);
			return;
		}
    }

	// If Setplay is not defined for this player...
	SamplePlayer::actionImpl();

}

void FCPortugalSetplayPlayer::communicationImpl(){
	if(setplayManager->isSetplayActive()){
		//Communicate?
		if(setplayManager->willCommunicate(context)){
			// communicate the message obtained through setplays.messageToSend(world)
			SetplayMessage* mess=setplayManager->messageToSend(context);

			string content;
			// Must somehow encode init message, or else max size (9) will be easily exceded
			// TODO This must be optimized
			if(mess->isInit()){
				InitMessage* initMessage=(InitMessage*)mess;

				// first char is "S", to mark as Init message
				content="S";
				// SetplayNumber will be sent as 3-digit number, as there might be a '-' sign
				ostrstream oss;
				oss<<setfill(' ') << setw(3) <<initMessage->setplayNumber;
				//cerr<<"SP: oss:"<<oss.str()<<endl;
				// ostrstream was not being properly terminated. Forcing substring
				content.append(string(oss.str()).substr(0,3));
				// Only considering players, not parameters
				// each player number will be a single char, either a single digit,
				// or 'a' for 10 and 'b' for 11
				for(unsigned int i=0;i<initMessage->arguments->size();i++){
					const string arg=*(initMessage->arguments->at(i));
					if(arg.size()==1)
						content.append(arg);
					else if(arg=="10")
						content.append("a");
					else if(arg=="11")
						content.append("b");
					else
						throw SetplayException("Unexpected player number in message");
				}
			}
			else
				content=mess->toString();

			// TODO deal better with this
			// For the time being, will send FreeMessage with maximum length
			// Apparently, the message must really be 9 char long, not less
			// patch...
			while(content.length()<9)
				content.append(" ");

			FreeMessage<9>* fm = new FreeMessage<9>(content);
			addSayMessage(fm);
		}

	}
	else // regular comm...
		SamplePlayer::communicationImpl();

}
