/*
 * SetplayException.h
 *
 *  Created on: Oct 4, 2013
 *      Author: luismota
 */

#ifndef SETPLAYPARSEEXCEPTION_H_
#define SETPLAYPARSEEXCEPTION_H_


#include <exception>
#include <string>

namespace fcportugal{
  namespace setplay{
  class SetplayException: public std::exception {
  public:
	  SetplayException(std::string message){this->message=message;};
	  virtual const char* what() const throw()
	   {
	     return message.c_str();
	   }
	  virtual ~SetplayException() throw (){};
  private:
	  std::string message;
  };

  }}
#endif /* SETPLAYPARSEEXCEPTION_H_ */
