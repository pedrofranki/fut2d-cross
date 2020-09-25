// -*-c++-*-
#ifndef SETPLAYMESSAGE_H
#define SETPLAYMESSAGE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

namespace fcportugal{
  namespace setplay{

    class SetplayManager;
    class Context;

    class SetplayMessage{
    public:
      SetplayMessage(const double time):emissionTime_(time){};
      virtual ~SetplayMessage(){};
    
      double emissionTime() const;
    
      virtual string toString() const=0;
  
      virtual void getProcessedBy(SetplayManager& manager,
				  const Context& world) const =0;

      virtual bool isInit() const=0;    
    private:
      double emissionTime_;
  
    };
  
    class InitMessage:public SetplayMessage{
    public:
      InitMessage(double time,int setplayNumber_,
		  vector<const string*>* arguments_)
	:SetplayMessage(time),
	 setplayNumber(setplayNumber_),
	 arguments(arguments_){}
      InitMessage(double time, int setplayNumber_,
		  vector<unsigned int>* players);

      InitMessage(const double time,string message);
  
      virtual ~InitMessage(){// Estava a dar bronca, experiencia
	//delete arguments;
      };
  
      
      //First element is the letter 'S', then the setplay number, parameters 
      //and players, all separated by spaces
      virtual string toString() const;
  
      virtual void getProcessedBy(SetplayManager& manager,
				  const Context& world) const;
  
      int setplayNumber;
      vector<const string*>* arguments;

      bool isInit() const{return true;}
  
    };
  

    class StepMessage:public SetplayMessage{
    public:
      StepMessage(const double time,
		  int cur, int next)
	:SetplayMessage(time),currentStepNumber(cur),nextStepNumber(next){}

      StepMessage(const double time,string message);
  
      virtual ~StepMessage(){};
  
      //A space separates current and next steps
      virtual string toString() const;
  
      virtual void getProcessedBy(SetplayManager& manager,
				  const Context& world) const;
  
      int currentStepNumber;
      int nextStepNumber;
  
      bool isInit() const{return false;}
    };
  }
}
#endif
