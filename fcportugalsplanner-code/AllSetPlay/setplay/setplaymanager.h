// -*-c++-*-
#ifndef SETPLAYMANAGER_H
#define SETPLAYMANAGER_H

#include <vector>
#include <string>
#include <iostream>

#include <setplay/playerReference.h>
#include <setplay/setplay.h>
#include <setplay/setplaymessage.h>
#include <setplay/cbr.h>

using namespace std;

namespace fcportugal{
namespace setplay{
class Setplay;

class SetplayManager{
public:
	SetplayManager(const string teamName,const string conf_file,
			const bool useComm,
			const bool limitedComm,
			const float messageRepeatTime,
			const Context& world);

	virtual ~SetplayManager();

	//Inspectors

	unsigned int numberOfSetplays() const;

	void writeSetplayNum(ostream &os, const int unsigned setplayNumber) const;
	void writeSetplayById(ostream &os, const int id) const;

	void writeCurrentSetplay(ostream &os) const;

	const Setplay* getSetplay(int num) const;

	const Setplay& getCurrentSetplay() const{return *currentSetplay;}

	//Modifiers
	void substituteNamedRegions(const Context& world);

	//RUNTIME METHODS
	virtual void activateSetplay(const InitMessage init,
			const Context& world);

	vector<const Setplay*>*
	feasibleSetplays(const Context& world) const;

	// A client program can ask for the possible
	// actions at a given moment through the method possibleActions
	// After that, it chooses one of the actions and then calls the method
	// chosenAction to inform the setplay framework.
	vector<vector<const Action*>*>*
	possibleActions(const Context& world) const;

	// A client program can ask for the possible
	// transitions at a given moment through the method possibleTransitions
	// After that, it chooses one of the transitions and then calls the method
	// chosenTransition to inform the setplay framework.
	// Suited for use with a coach as lead player, where there are no
	// available actions, e.g. in Cambada
	vector<const Transition*>*
	possibleTransitions(const Context& world) const;

	// Get the action presently being executed
	const Action* currentAction(const Context& world) const;

	double currentStepInitTime() const;

	// get own positioning in the current state of the Setplay
	inline const Point* myCurrentPositioning(const Context& world) const{
		return currentSetplay->myCurrentPositioning(world);}

	// get own positioning in the current state of the Setplay
	inline const Point* currentPositioning(unsigned int num,
			const Context& world) const{
		return currentSetplay->currentPositioning(num,world);}

	//considering the chosenActionNumber argument, first action should be 0
	void chosenAction(const unsigned int& chosenActionNumber,
			const Context& world);

	//considering the chosenTransitionNumber argument,
	// first trantition should be 0
	void chosenTransition(const unsigned int& chosenTransitionNumber,
			const Context& world); 

	bool isNextStepAlreadyChosen() const;

	//When there is a situation that aborts the setplay
	virtual void abort(const Context& world);

	//When the setplay is gratiously ended
	virtual void finish(const Context& world);


	//update internal state
	void updateInternalState(const Context& world);

	bool isDone() const;

	bool isSetplayActive() const;

	bool thisPlayerParticipates(const Context& world) const;

	bool playerParticipates(const PlayerID* pl) const;

	bool amLeadPlayer(const Context& world) const;

	const PlayerID* leadPlayer() const;
	const PlayerID* nextStepLeadPlayer() const;

	bool waitTimeHasElapsed(const Context& world) const;

	bool willCommunicate(const Context& world) const;
	SetplayMessage* messageToSend(const Context& world);

	void processReceivedMessage(const InitMessage& m,const Context& world);
	void processReceivedMessage(const StepMessage& m,const Context& world);
	void processReceivedMessage(const string m,const Context& world);


protected:
	Setplay* currentSetplay;
	// Must know if setplay has ended, in order to send an end message
	mutable double lastSetplayEndTime;
	//mutable bool wasLastLeadPlayer;
	mutable bool participatedInLastSetplay;
	mutable bool lastSetplayWasSuccessful;


	virtual void dealWithSetplayEnd(const Context& world);
private:
	vector<Setplay*> setplays;
	string teamName;

	const float messageRepeatTime;


};

class SetplayManagerWithCBR:public SetplayManager{
public:
	SetplayManagerWithCBR(const string teamName,const string conf_file,const bool useComm,
			const bool limitedComm,const float messageRepeatTime,
			const Context& world,
			const string cbrFile,
			bool writeCBRToFile,
			bool writeCBRToFileAfterExecution,
			const double weightThisGame,
			const double weightThisTeam,
			const double weightOtherTeams);
	virtual ~SetplayManagerWithCBR();

	virtual void abort(const Context& world);

	virtual void finish(const Context& world);

	unsigned int
	selectSetplayByCBR(vector<CaseBase::SetplayID> possibleSetplays,
			const Context& world) const;

	void activateSetplay(const InitMessage init,
			const Context& world);

	void autoActivateSetplay(const Context& world);

	virtual void dealWithSetplayEnd(const Context& world);
private:
	// for CBR
	CaseBase* caseBase;
	const PointSimple* initialBallPos; // for CBR
	const rcss::clang::PlayMode *gm;
};
}
}
#endif
