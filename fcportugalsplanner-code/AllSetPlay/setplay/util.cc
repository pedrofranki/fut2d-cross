#include "util.h"
#include <iostream>
#include <cassert>

using namespace std;

std::ostream& operator<<( std::ostream & os, const PlayerNumAndDistance& e ){
  for(unsigned int j=0; j!=e.playerNums->size();j++)
    os<<e.playerNums->at(j)<<" "; 
return os<<" Sum:"<<e.totalDist;
}

vector<PlayerNumAndDistance>*
appendToVector(const vector<PlayerNumAndDistance> &v1,
	       const PlayerNumAndDistance& v2){
  vector<PlayerNumAndDistance>* res=new vector<PlayerNumAndDistance>;
  
  for(unsigned int i=0; i!= v1.size();i++){
    vector<unsigned int>* toAppend
      =new vector<unsigned int>(*v1.at(i).playerNums);
    
    bool repeatedElements=false;
    //append
    for(vector<unsigned int>::iterator j=v2.playerNums->begin();
	j!=v2.playerNums->end();j++)
      if(containedInVector(*j,*toAppend)){
	repeatedElements=true;
	break;
      }
      else
	toAppend->push_back(*j);  

    if(!repeatedElements)
      res->push_back(*new PlayerNumAndDistance(toAppend,
					       v1.at(i).totalDist
					       +v2.totalDist));
  }
  
  
  // cout<<"DENTRO"<<endl;
  //   for(unsigned int i=0; i!=res->size();i++){
  //     for(unsigned int j=0; j!=res->at(i).playerNums->size();j++)
  //       cout<<res->at(i).playerNums->at(j)<<" ";
  //     cout<<" Soma:"<<res->at(i).totalDist<<endl;
  //   }
  
  return res;
}


bool containedInVector(unsigned int e,vector<unsigned int> v){
  for(vector<unsigned int>::iterator i=v.begin(); i!= v.end();i++)
    if(*i==e)
      return true;
  return false;
}


unsigned int fact(unsigned int num){
  assert(num>=0);

  unsigned int result =1;

  while(num>0)
    result*=num--;

  return result;
}
