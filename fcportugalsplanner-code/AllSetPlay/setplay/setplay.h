// -*-c++-*-
#ifndef SETPLAY_H
#define SETPLAY_H

#include <vector>
#include <string>

#include <setplay/playerReference.h>
#include <setplay/parameter.h>
#include <setplay/region.h>
#include <setplay/directive.h>
#include <setplay/setplaymessage.h>

// From clang
#include <setplay/cond.h>
#include <setplay/clangaction.h>
#include <setplay/clang/arithop.h>

namespace fcportugal{
  namespace setplay{
    class Step;
    class Transition;
    class Participation;

    class Setplay {
      friend class SetplayManager;
    public:
      Setplay(const string name_,const int setplayNumber_,
	      const bool commAllowed_,
	      const bool singleChannelComm_,
	      const float messageRepeatTime_);
      ~Setplay() {};

      //Parser
      static Setplay* parse(const string in, string& out, 
			    const bool useComm, const bool limitedComm,
			    const float messageRepeatTime);
      
      //Attributes
      string name;
      vector<SetplayParameter*> *parameters;
      vector<const PlayerReference*> players;
      vector<Step*> *steps;
      Cond* abortCond;	

  
      //Inspectors
      int id() const{return setplayNumber;};
      unsigned int initialFreeParticipantsAmount() const;
      inline bool isInvertible() const { return invertible; }

      //Modifiers
      bool instantiate(const InitMessage& message);
      bool instantiate(vector<const string*> *params);
      void substituteNamedRegions(const Context& world);
      inline void setInvertible(bool inv)  { invertible = inv; }
     
      //Runtime procedures
      void start(const Context& world);

      // For allocation of players to setplay...
      // Numbers in output will start at 1, like normal football...
      
      // Inactive... Not implemented
      vector<unsigned int>* 
      participatingPlayersDistanceFromMe(const Context& world) const;
      
      vector<unsigned int>* 
      participatingPlayersDistanceFromPositions(const Context& world) const;

      vector<unsigned int>* 
      participatingPlayersDistanceFromPosAsArg(const Context& world,
					       const vector<PointSimple>& 
					       positions ) const;

      // TODO
      // create this alternative method:
      //vector<unsigned int>* 
      //participatingPlayersGlobalDistanceFromPositions(const Context& world) const;
      vector<unsigned int>* 
      participatingPlayersGlobalDistanceFromPosAsArg(const Context& world,
						     const vector<PointSimple>& 
						     positions ) const;
      // A client program can ask for the possible
      // actions at a given moment through the method possibleActions
      // After that, it chooses one of the actions and than calls the method
      // chosenAction to inform the setplay.
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


      //considering the chosenActionNumber argument, first action should be 0
      void chosenAction(const unsigned int& chosenActionNumber,
			const Context& world); 

      //considering the chosenTransitionNumber argument, 
      // first trantition should be 0
      void chosenTransition(const unsigned int& chosenTransitionNumber,
			    const Context& world); 

      //update internal state: change current step and action list
      void updateInternalState(const Context& world);

      // alternative, yet to test properly: update internal state, make possible
      // action choices, and execute the current action, if any...
      void executeSetplay(const Context& world, Action::Executor& exec);

      void markAsSuccess();
      void markAsDone(){done=true;}

      //Runtime inspectors
      bool isDone() const;

      // step number inspectors are just for debug... Should not be used otherwise.
      int currentStepNumber() const;
      int nextStepNumber() const;

      //get own positioning in the current state of the Setplay
      const Point* myCurrentPositioning(const Context& world)const;
      //get the num player's positioning in the current state of the Setplay
      const Point* currentPositioning(unsigned int num,
				      const Context& world)const;

      double currentStepInitTime() const;
      bool isPossible(const Context& world) const;
      const Action* currentAction(const Context& world) const;
      bool playerParticipates(const PlayerID* id) const;
      bool amLeadPlayer(const Context& world) const;
      const PlayerID* leadPlayer() const;
      bool isNextStepAlreadyChosen() const;
      const PlayerID* nextStepLeadPlayer() const;

      bool waitTimeHasElapsed(const Context& world) const;

      bool willCommunicate(const Context& world) const;
      SetplayMessage* messageToSend(const Context& world) const;
      void processReceivedMessage(const StepMessage& m,
				  const Context& world);

      bool wasSuccessful() const{return successful;}
      // Output
      void write(ostream &os) const;
      void write2(ostream &os, unsigned int indent) const; 

      // Copy 
      Setplay* deepCopy() const;

      // Inverts all spacial references, mirroring them on the long axis
      Setplay* inversion() const;

    private:
      //inspectors
      Step* getStep(const unsigned int& stepID) const;
      SetplayParameter* getSetplayParameterWithName(const string& str) const;
      const PlayerReference* roleWithName(const string& name) const;
      PlayerID* existentPlayer(const PlayerID* p) const;


      // No longer necessary, supposedly, to be public, since player
      // instantiation is now internal
      vector<const Participation*>* initialParticipants() const;

      //Runtime modifiers
      void setCurrentStep(const unsigned int& stepID,const Context& world);

  
     
      //attributes
      int setplayNumber;// May be negative, for inverted setplays
      bool instantiated;
      bool invertible;

      //Runtime attributes
      //PlayerID* me; //The ID of the player. Was frequently being wrongly set, on initialization
      // Therefore, was moved to class Context...

      Step* currentStep;
      vector<const Action*>* currentActions;
      Step* nextStep;
      double setplayBegin;
      double stepBegin;
      bool done;
      bool wasLastLeadPlayer;
      bool successful;

      

      // communication settings 
      bool commAllowed;
      bool singleChannelComm;
      const float messageRepeatTime;
    };




    class Transition{
    public:
      Transition():cond(NULL),directives(NULL){};
      Transition(Cond* cond_,vector<Directive*>* directives_)
	:cond(cond_),directives(directives_){};
      virtual ~Transition(){};

      //Parser
      static Transition* parse(const string in, string& out,
    		  const vector<SetplayParameter*>& parameters,
    		  const vector<PlayerReference*>& players);
  
      virtual void print(ostream &os) const=0;
      virtual void print2(ostream &os, unsigned int &indent) const=0;
  
      inline void setCond(Cond* cond_){cond=cond_;}
      inline Cond* getCond() const{return cond;}
  
      inline void setDirectives(vector<Directive*>* directives_)
      {directives=directives_;}
  
      vector<const Action*>* getDoActionsForPlayer(const PlayerID& p);

      void setActionsAsNotDone() const;
  
      inline vector<Directive*>* getDirectives() const{return directives;};
  
      virtual bool isNextStep() const=0;
      virtual bool isFinish() const=0;

      virtual Transition* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual Transition* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;
      
      void substituteNamedRegions(const Context& world);
  
  
    protected:
      Cond* cond;
      vector<Directive*>* directives;
  
      void writeAux(ostream &os) const;
      void writeAux2(ostream &os, unsigned int &indent) const;
    };

    class Abort:public Transition{
    public:
      inline Abort(Cond* cond=NULL,vector<Directive*>* directives=NULL)
	:Transition(cond,directives){};
      ~Abort(){};

      inline void print(ostream &os) const{os<<"(abort";writeAux(os);os<<")";};
      inline void print2(ostream &os, unsigned int &/*indent*/) const{os<<"(abort";writeAux(os);os<<")";};
  
      inline virtual bool isNextStep() const{return false;}
      virtual bool isFinish() const{return false;}
  
      virtual Transition* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;
  
      virtual Transition* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;
    };

    class Finish:public Transition{
    public:
      inline Finish(Cond* cond=NULL,vector<Directive*>* directives=NULL)
	:Transition(cond,directives){};
      ~Finish(){};
    
      inline void print(ostream &os) const{os<<"(finish";writeAux(os);os<<")";}
      inline void print2(ostream &os, unsigned int &/*indent*/) const{os<<"(finish";writeAux(os);os<<")";};
	
      inline virtual bool isNextStep() const{return false;}
      virtual bool isFinish() const{return true;}

      virtual Transition* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      virtual Transition* inversion(const vector<SetplayParameter*>& params,
				   const vector<PlayerReference*>& players)	const;

    };

    class NextStep:public Transition{
    public:
      inline NextStep(int const nextStepNumber_)
	:Transition(),
	 nextStepNumber(nextStepNumber_){};
      
      
      inline NextStep(int const nextStepNumber_, Cond* condition=NULL,
		      vector<Directive*>* directives=NULL)
	:Transition(condition,directives),nextStepNumber(nextStepNumber_){};
  
      ~NextStep();
  
      int const nextStepNumber;
  
      void print(ostream &os) const;
      void print2(ostream &os, unsigned int &indent) const;
  
      inline virtual bool isNextStep() const{return true;}
      virtual bool isFinish() const{return false;}
  
      Transition* deepCopy(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const;
  
      Transition* inversion(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const;


    };

    class Participation{
    public:
      inline Participation(PlayerReference* participant)
	:participant_(participant),location_(NULL){}
      inline Participation(PlayerReference* participant, Region* location):
	participant_(participant),location_(location){}

      inline ~Participation(){}
      //Parser
      static Participation* parse(const string in, string& out,
    		  const vector<SetplayParameter*>& parameters,
    		  const vector<PlayerReference*>& players);

      void print(ostream &os) const;

      //Inspectors
      inline const PlayerReference* participant()const{return participant_;}
      inline const Region* location()const{return location_;}

      Participation* deepCopy(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const;

      Participation* inversion(const vector<SetplayParameter*>& params,
			      const vector<PlayerReference*>& players) const;

      void substituteNamedRegions(const Context& world);

    private:
      PlayerReference* participant_;
      Region* location_;
    };

   


    class Step{
    public:
      Step(int const order_, double const waitTime_, double const abortTime_);
      inline ~Step(){};
  
      //Parser
      static Step* parse(const string in, string& out,
    		  const vector<SetplayParameter*>& parameters,
    		  const vector<PlayerReference*>& players);

      //get the positioning in this Step of the Setplay
      const Point* currentPositioning(const PlayerReference* pid,
				      const Context& world)const;

      void setActionsAsNotDone() const;
  
      unsigned int const id;
      double const waitTime;
      double const abortTime;

      vector<Transition*> *transitions;
      vector<Participation*> *participants;
      PlayerReference* leadPlayer;

      // ATTENTION: in Step 0, the condition is the pre-condition for the whole 
      // setplay. As such, it should include no references to parameters or roles,
      // which are only decided later, at instantiation...
      Cond* condition;

      void print(ostream &os) const;
      void print2(ostream &os, unsigned int& indent) const;

      Step* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      Step* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      void substituteNamedRegions(const Context& world);

    
    };

  }
}
#endif //SETPLAY_H
