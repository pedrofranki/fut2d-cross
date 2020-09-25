// -*-c++-*-
#ifndef UTIL_H
#define UTIL_H


#include <vector>
#include <iostream>

//struct used as element in next procedure
struct PlayerNumAndDistance{
  std::vector<unsigned int>* playerNums;
  float totalDist;
  
 PlayerNumAndDistance():playerNums(new std::vector<unsigned int>),totalDist(0){}
 PlayerNumAndDistance(std::vector<unsigned int>* playerNums_,float totalDist_):
  playerNums(playerNums_),totalDist(totalDist_){}
};

std::ostream& operator<<( std::ostream & os, const PlayerNumAndDistance & e );

// Procedure that receives a PlayerNumAndDistance vector v1
// and a PlayerNumAndDistance v2.  v2 will be 
// appended to each element in v1. Distances will be summed.
// All these elements will be returned. If any element in v2 
// is present in an element of v1, this element will be discarded
std::vector<PlayerNumAndDistance>*
appendToVector(const std::vector<PlayerNumAndDistance>& v1, 
	       const PlayerNumAndDistance& v2);


// Check if an element is contained in a vector
bool containedInVector(unsigned int e,std::vector<unsigned int> v);
// factorial
unsigned int fact(unsigned int num);

#endif //UTIL_H
