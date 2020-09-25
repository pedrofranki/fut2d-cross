#include <setplay/setplaymessage.h>
#include <setplay/setplaymanager.h>
#include <setplay/setplayexception.h>

using namespace fcportugal::setplay;

//--------------------------------------------------------------------------
// MESSAGE
//--------------------------------------------------------------------------

double SetplayMessage::emissionTime() const{
  return emissionTime_;
}


InitMessage::InitMessage(double time,
			 string message)
  :SetplayMessage(time){
  if(message[0]!='S')// NOT InitMessage
    throw SetplayException("SP:Invalid Init message!");
  else{
    //pop initial 'S'
    message=message.substr(1);


    // CAUTION
    // if there are no arguments nor participants (very improbable...),
    // there will be no space-character, and there might be a problem...
    size_t pos_espaco=message.find(" ");
    
    stringstream stream(message.substr(0,pos_espaco));
    int number_in_message;
    stream>>number_in_message;
    string rest_arguments=message.substr(pos_espaco+1);

    vector<const string*>* arguments_= new vector<const string*>;
 
    while(pos_espaco!=string::npos){
      pos_espaco=rest_arguments.find(" ");

      if(pos_espaco!=string::npos){
	arguments_
	  ->push_back(new const string(rest_arguments.substr(0,pos_espaco)));
	rest_arguments=rest_arguments.substr(pos_espaco+1);
      }
      else
	arguments_->push_back(new const string(rest_arguments));
      
    }
    setplayNumber=number_in_message;
    arguments=arguments_;
  }
}

InitMessage::InitMessage(double time, int setplayNumber_,
		  vector<unsigned int>* players)
	:SetplayMessage(time),
	 setplayNumber(setplayNumber_){
  arguments= new vector<const string*>;
  for(unsigned int i=0; (players !=NULL && i!= players->size()); i++){
    stringstream out;
    out<<players->at(i);
    arguments->push_back(new string(out.str()));
  }

}

StepMessage::StepMessage(double time,string message)
  :SetplayMessage(time){
  //unsigned int pos_espaco=message.find(" ");
  stringstream stream(message);//.substr(0,pos_espaco));
  int currentStepNumber_;
  stream>>currentStepNumber_;
  int nextStepNumber_;
  stream>>nextStepNumber_;
  currentStepNumber=currentStepNumber_;
  nextStepNumber=nextStepNumber_;

}

string InitMessage::toString() const{
  stringstream res;

  res<<"S"<<setplayNumber;
  for(unsigned int i=0; i!=arguments->size();i++)
    res<<" "<<*arguments->at(i);
  
  return res.str();
}

void InitMessage::getProcessedBy(SetplayManager& manager,
				 const Context& world) const{
  manager.processReceivedMessage(*this,world);
}

string StepMessage::toString() const{
  stringstream res;
  res<<currentStepNumber<<" "<<nextStepNumber;
  return res.str();
}

void StepMessage::getProcessedBy(SetplayManager& manager,
				 const Context& world) const{
  manager.processReceivedMessage(*this,world);
}
