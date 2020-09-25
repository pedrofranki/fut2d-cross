#include <setplay/setplaymanager.h>
#include <setplay/setplayexception.h>

#include <fstream>


using namespace fcportugal::setplay;

// no CBR
SetplayManager::SetplayManager(const string teamName,
			       const string conf_file,const bool useComm,
			       const bool limitedComm,
			       const float messageRepeatTime_,
			       const Context& world)
  :currentSetplay(NULL),
   lastSetplayEndTime(-1),participatedInLastSetplay(false),
   //wasLastLeadPlayer(false),
   lastSetplayWasSuccessful(false),
   messageRepeatTime(messageRepeatTime_),
   teamName(teamName){
	//read conf file
	vector<string> text;
	string line;
	ifstream textstream(conf_file.c_str());
	while (getline(textstream, line)) {
		text.push_back(line);
	}
	textstream.close();
	string alltext;
	for (unsigned int i=0; i < text.size(); i++){
		// substitute tabs by spaces
		size_t j = text[i].find('\t', 0);
		while(j!=string::npos){
			text[i].replace(j,1," ");
			j = text[i].find("\t", 0);
		}
		alltext += text[i];
	}


	string rest;

	Setplay* sp=Setplay::parse(alltext,rest,useComm,limitedComm,
			messageRepeatTime_);

	while(sp){
		if(getSetplay(sp->id())!=NULL)
			cerr<<"SP: duplicate id in setplay file!"<<endl;
		else{
			sp->substituteNamedRegions(world);

			setplays.push_back(sp);

			// Invertible?
			if(sp->invertible)
				setplays.push_back(sp->inversion());
		}

		sp=Setplay::parse(rest,rest,useComm,limitedComm,
				messageRepeatTime_);
	}

	// set random seed
	srand(time(NULL));

	if(rest.length()>0)
		cerr<<"\n Remainder of Setplay Config file("<<rest.length()<<" chars):"<<rest<<endl;
}

SetplayManagerWithCBR::SetplayManagerWithCBR(const string teamName,
					     const string conf_file,
					     const bool useComm,
					     const bool limitedComm,
					     const float messageRepeatTime_,
					     const Context& world,
					     const string cbrFile, 
					     bool writeCBRToFile, 
					     bool writeCBRToFileAfterExecution,
					     const double weightThisGame,
					     const double weightThisTeam,
					     const double weightOtherTeams)
  :SetplayManager(teamName,conf_file,useComm,limitedComm,messageRepeatTime_,
		 world),
  caseBase(new CaseBase(teamName,cbrFile,writeCBRToFile,
			writeCBRToFileAfterExecution,
			weightThisGame,weightThisTeam,
			weightOtherTeams)),
			initialBallPos(NULL),
			gm(NULL){

  // set random seed
  srand(time(NULL));

}


SetplayManagerWithCBR::~SetplayManagerWithCBR(){
  if(caseBase!=NULL) caseBase->writeToFile();
}

SetplayManager::~SetplayManager(){
  for(unsigned int i=0; i!=  setplays.size();i++)
    delete setplays[i];
}

unsigned int SetplayManager::numberOfSetplays() const{
  return setplays.size();
}

const Setplay* SetplayManager::getSetplay(int num) const{
  for(vector<Setplay*>::const_iterator i=setplays.begin();
      i!=setplays.end();i++)
    if((*i)->id()==num)
      return *i;

  return NULL;
}

double  SetplayManager::currentStepInitTime() const{
  if(currentSetplay==NULL ){
	  throw SetplayException("Trying to access setplay init time, but none is active!");
    assert(false);//tanga. Sera' que devia rebentar?
  }

  return currentSetplay->currentStepInitTime();
}

const Action* SetplayManager::currentAction(const Context& world) const{
  assert(currentSetplay);
    
  return currentSetplay->currentAction(world);
}


void SetplayManager::abort(const Context& world){
  assert(currentSetplay);
  currentSetplay->markAsDone();
  dealWithSetplayEnd(world);
}

void SetplayManagerWithCBR::abort(const Context& world){
  assert(currentSetplay);
  currentSetplay->markAsDone();
  
  dealWithSetplayEnd(world);
}




void SetplayManager::finish(const Context& world){
  assert(currentSetplay);
  currentSetplay->markAsDone();
  currentSetplay->markAsSuccess();
  dealWithSetplayEnd(world);

}


void SetplayManagerWithCBR::finish(const Context& world){
  assert(currentSetplay);
  currentSetplay->markAsDone();
  currentSetplay->markAsSuccess();
  
  dealWithSetplayEnd(world);
 
}

bool SetplayManager::isDone() const{
  assert(currentSetplay);

  return currentSetplay->isDone();
}

void SetplayManager::writeSetplayNum(ostream &os,
				     const unsigned int setplayNumber) const{
  if(setplayNumber>=setplays.size()){
    cerr<<"SP: inexistent Setplay in write number:"<<setplayNumber<<endl;
    return;
  }
  setplays[setplayNumber]->write(os);
}

void SetplayManager::writeSetplayById(ostream &os,
				  const int id) const{
  if(getSetplay(id)==NULL){
    cerr<<"SP: inexistent Setplay in write with id:"<<id<<endl;
    return;
  }
  getSetplay(id)->write(os);
}

void SetplayManager::writeCurrentSetplay(ostream &os) const{
  assert(currentSetplay);

  currentSetplay->write(os);
}



void SetplayManager::substituteNamedRegions(const Context& world){
  for(unsigned int i=0; i!=  setplays.size();i++)
    setplays[i]->substituteNamedRegions(world);
}

unsigned int
SetplayManagerWithCBR::selectSetplayByCBR(vector<CaseBase::SetplayID> possibleSetplays,
					  const Context& world) const{
  cerr<<"SP: choosing SP by CBR: caseBase null:"<<(caseBase==NULL)
      <<" number of possible SPs:"<<possibleSetplays.size()
      <<endl;
  // CBR select
  int chosenSPId=caseBase->selectSetplay(possibleSetplays,world);
  
  
  return chosenSPId;
}

void SetplayManagerWithCBR::autoActivateSetplay(const Context& world){
  // get all valid setplays
  vector<const Setplay*>* feasibleSPs=feasibleSetplays(world);

  // exit if no Setplay is available
  if(feasibleSPs->empty())
	  return;

  // Create vector with possible setplay names
  vector<CaseBase::SetplayID>feasibleSPIds;
  for(unsigned int i=0;feasibleSPs!=NULL && i!=feasibleSPs->size();i++)
    feasibleSPIds.push_back(CaseBase::SetplayID(feasibleSPs->at(i)->name,
						feasibleSPs->at(i)->id()));

  // CBR select
  int chosenSPId=selectSetplayByCBR(feasibleSPIds,world);

  // Check if no Setplay could be chosen
  if(chosenSPId==10000)
    return;

  const fcportugal::setplay::Setplay* chosenSetplay
    = getSetplay(chosenSPId);
  
  // Choose participating players
  vector<unsigned int>* participatingPlayers
    = chosenSetplay->participatingPlayersDistanceFromPositions(world);


  // Activate the setplay with chosen players
  // No other arguments for the time being
  activateSetplay(InitMessage(world.time(),
			      chosenSetplay->id(),
			      participatingPlayers),world);
  

  // for CBR, register start data
  initialBallPos=world.ballPos();
  gm=new const rcss::clang::PlayMode(world.playMode());

}

void SetplayManagerWithCBR::activateSetplay(const InitMessage init,
					    const Context& world){
  SetplayManager::activateSetplay(init,world);

  // for CBR, register start data
  initialBallPos=world.ballPos();
  gm=new const rcss::clang::PlayMode(world.playMode());

}

void SetplayManager::activateSetplay(const InitMessage init,
				     const Context& world){

  Setplay* intendedSetplay
    =(getSetplay(init.setplayNumber)!=NULL?
      getSetplay(init.setplayNumber)->deepCopy():NULL);

  bool instantiated 
    =(intendedSetplay!=NULL?intendedSetplay->instantiate(init):false);

  if(!instantiated){
    return;
  }


  // Setplay is always kept, even if  player does not participate
  // This is useful to know the participating players' positions.
  intendedSetplay->start(world);
  currentSetplay=intendedSetplay;
  
  
}




vector<const Setplay*>* 
SetplayManager::feasibleSetplays(const Context& world) const{
  vector<const Setplay*>* res= new vector<const Setplay*>();

  for(unsigned int i = 0; i!= setplays.size(); i++)
    if(setplays[i]->steps->at(0)->condition==NULL // No condition, always valid
       ||  setplays[i]->steps->at(0)->condition->eval(world))
      res->push_back(setplays[i]);

  return res;
}


bool SetplayManager::isSetplayActive() const{
  return currentSetplay!=NULL;
}


bool SetplayManager::thisPlayerParticipates(const Context& world) const{
  return (currentSetplay!=NULL &&
	  currentSetplay->playerParticipates(&(world.me())));
}

bool SetplayManager::playerParticipates(const PlayerID* pl) const{
  return currentSetplay && currentSetplay->playerParticipates(pl);
}

bool SetplayManager::amLeadPlayer(const Context& world) const{
  return currentSetplay!=NULL && currentSetplay->amLeadPlayer(world);
}

const PlayerID* SetplayManager::leadPlayer() const{
  assert(currentSetplay!=NULL);

  return currentSetplay->leadPlayer();
}


const PlayerID* SetplayManager::nextStepLeadPlayer() const{

  assert(currentSetplay!=NULL);

  return currentSetplay->nextStepLeadPlayer();
}
bool SetplayManager::waitTimeHasElapsed(const Context& world) const{
  assert(currentSetplay);

  return currentSetplay->waitTimeHasElapsed(world);
}

bool SetplayManager::isNextStepAlreadyChosen() const{
  assert(currentSetplay);

  return currentSetplay->isNextStepAlreadyChosen();
}

vector<vector<const Action*>*>* 
SetplayManager::possibleActions(const Context& world) const{
  if(currentSetplay!=NULL) {
    return currentSetplay->possibleActions(world);
  }
  throw SetplayException("Trying to check possible actions in Setplay, but none is active!");
  return NULL;
}

vector<const Transition*>*
SetplayManager::possibleTransitions(const Context& world) const{
  if(currentSetplay!=NULL)
    return currentSetplay->possibleTransitions(world);
  else
	  throw SetplayException("Trying to check possible Transitions in Setplay, but none is active!");
  return NULL;
}

//considering the chosenActionNumber argument, first action should be 0
void SetplayManager::chosenAction(const unsigned int& chosenActionNumber,
		  const Context& world){
  if(currentSetplay!=NULL)
    currentSetplay->chosenAction(chosenActionNumber,world);
  else
    // Launch exception
    cerr<<"Trying to choose action in Setplay, but none is active!"<<endl;

  //Check if setplay is over...
  if(currentSetplay->isDone()){

    dealWithSetplayEnd(world); 
  }
}

//considering the chosenTransitionNumber argument, 
// first trantition should be 0
void 
SetplayManager::chosenTransition(const unsigned int& chosenTransitionNumber,
				 const Context& world){
  if(currentSetplay!=NULL)

    currentSetplay->chosenTransition(chosenTransitionNumber,world);
  else
    // Launch exception
    cerr<<"Trying to choose action in Setplay, but none is active!"<<endl;

  //Check if setplay is over...
  if(currentSetplay->isDone()){

    dealWithSetplayEnd(world); 
  }
}



void SetplayManager::dealWithSetplayEnd(const Context& world){
  
  lastSetplayEndTime=world.time();
  if(currentSetplay->playerParticipates(&(world.me()))) participatedInLastSetplay=true;
  //if(currentSetplay->amLeadPlayer()) wasLastLeadPlayer=true;
  lastSetplayWasSuccessful=currentSetplay->wasSuccessful();
  delete  currentSetplay;
  currentSetplay=NULL;
}


void SetplayManagerWithCBR::dealWithSetplayEnd(const Context& world){
  // register CBR
  if(caseBase!=NULL){
    caseBase->registerCase(currentSetplay->name,
			   initialBallPos,
			   *gm,currentSetplay->wasSuccessful(),
			   world);
  }

  lastSetplayEndTime=world.time();
  lastSetplayWasSuccessful=currentSetplay->wasSuccessful();
  if(currentSetplay->playerParticipates(&(world.me()))) participatedInLastSetplay=true;
  //if(currentSetplay->amLeadPlayer()) wasLastLeadPlayer=true;
  delete  currentSetplay;
  currentSetplay=NULL;
}

void 
SetplayManager::updateInternalState(const Context& world){

  if(currentSetplay!=NULL)
    currentSetplay->updateInternalState(world);
  else
    // Launch exception
    cerr<<"Trying to update internal state in Setplay, but none is active!"
	<<endl;
  
  //Check if setplay is over...
  if(currentSetplay->isDone())
    dealWithSetplayEnd(world); 
}

//----------------------------------  Communication--------------------------
bool SetplayManager::willCommunicate(const Context& world) const{
  if(currentSetplay!=NULL)
    return currentSetplay->willCommunicate(world); 
  else {//Setplay is over
    // Every player will communicate, to broadcast this important information

    //if (//lastSetplayEndTime!=-1 && me->number >6)//
    //	wasLastLeadPlayer)

      // Check if repetition is already above messageRepeatTime
    if((world.time()-lastSetplayEndTime)>=messageRepeatTime){
      lastSetplayEndTime=-1; //No more repetitions
      lastSetplayWasSuccessful=false;  
      participatedInLastSetplay=false;
      //wasLastLeadPlayer=false;
      
      return false;
    }
    
    
    // Only participants in last Setplay will communicate Setplay end
    // This avoids messages from players that might be away from the action,
    // and therefore do not hear messages and easily let abort times elapse
    return //wasLastLeadPlayer && 
      lastSetplayEndTime!=-1 && participatedInLastSetplay
      && (world.time()-lastSetplayEndTime)<messageRepeatTime;
  }
}


SetplayMessage* SetplayManager::messageToSend(const Context& world){
  if(currentSetplay!=NULL && !currentSetplay->isDone())
    return currentSetplay->messageToSend(world);
  else
    return new StepMessage(lastSetplayEndTime,-1,lastSetplayWasSuccessful?1:-1);
}


void
SetplayManager::processReceivedMessage(const InitMessage& m,
				       const Context& world){
  if(currentSetplay!=NULL){//A setplay is active!
    if(currentSetplay->id()!= m.setplayNumber){
      dealWithSetplayEnd(world);
    }
    else if(currentSetplay->currentStepNumber()!=0 && !amLeadPlayer(world)){
      dealWithSetplayEnd(world);
    }
    else
      return; // Setplay is still in step 0, do nothing    
  }
  else
    activateSetplay(m,world);

}

void
SetplayManager::processReceivedMessage(const StepMessage& m,
				       const Context& world){


  if(currentSetplay!=NULL)
    currentSetplay->processReceivedMessage(m,world);
  else // Check for success in done setplay 
    if(m.currentStepNumber==-1 && m.nextStepNumber==1)
      lastSetplayWasSuccessful=true;
}

// CAUTION is this necessary?
// void
// SetplayManager::processReceivedMessage(const SetplayMessage* m,
// 				       const Context& world){
//   if(m->isInit())
//     processReceivedMessage((InitMessage*)m,world);
//   else
//     processReceivedMessage((StepMessage*)m,world);
// }


void SetplayManager::processReceivedMessage(const string m,
					    const Context& world){
  if(m[0]=='S'){//InitMessage
    InitMessage* msg= new InitMessage(world.time(),m);
    processReceivedMessage(*msg,world);
  }else{
    StepMessage* msg= new StepMessage(world.time(),m);
    processReceivedMessage(*msg,world);
  }
}

