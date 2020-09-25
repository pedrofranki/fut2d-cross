// -*-c++-*-


#include "cbr.h"

using namespace fcportugal::setplay;
using namespace std;

int main(){
  //CaseBase cb;
  
  string rest;
  Case* c=Case::parse("(case :setplayName name :transverse our_front :longitudinal centre_left :radial sl_9 :gameMode ko_opp :adversary adversary :numTries 4 :numFail 2 :numTriesThisGame 1  :numFailThisGame 1)",rest);
  Case* c1=Case::parse("(case :setplayName name :transverse our_front :longitudinal centre_left :radial sl_9 :gameMode ko_opp :adversary adversary :numTries 5 :numFail 3 :numTriesThisGame 0  :numFailThisGame 0)",rest);
  //:gameMode koour
  Case* c2=Case::parse("(case :setplayName name :transverse our_front :longitudinal centre_left :radial sl_9 :gameMode ko_our :adversary adversary :numTries 4 :numFail 2 :numTriesThisGame 1  :numFailThisGame 1)",rest);

  c->write(cerr); cerr<<endl;
  c2->write(cerr); cerr<<endl;
  
  cerr<<endl<<"Cases equal (yes):"<<c->isSameCase(*c1)<<endl;
  cerr<<endl<<"Cases equal (no):"<<c->isSameCase(*c2)<<endl;

  Case::Evaluation e;

  cerr<<"Eval: "<<e<<endl;

}

  
