// -*-c++-*-

/***************************************************************************
                                object.h  
                       Abstract base class for objects on the field
                             -------------------
    begin                : 04-Mar-2009
    copyright            : (C) 2006 Luis Mota
    email                : luis.mota@iscte.pt
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include <ostream>
#include <string>

#include <setplay/parameter.h>

namespace fcportugal{
  namespace setplay{

    class PlayerReference;

    class Object{
    public:
      virtual ~Object(){};

      virtual void print(std::ostream &os) const=0;

      static Object* parse(const std::string& in, std::string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      virtual Object* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector< PlayerReference*>& players) const=0;

      virtual Object* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual char type() const=0; 
    };

    class StaticObject : public Object
    { 
    public:
      virtual ~StaticObject(){};

      virtual void print(std::ostream &os) const=0;

      static Object* parse(const std::string& in, std::string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      virtual Object* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual Object* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual char type() const=0; 
    };

    class MobileObject : public Object
    {
    public:
      virtual ~MobileObject(){};

      virtual void print(std::ostream &os) const=0;

      static Object* parse(const std::string& in, std::string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      virtual Object* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual Object* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      virtual char type() const=0; 
    };

    class Ball : public MobileObject
    {
    public:
      Ball(){};

      ~Ball(){};

      virtual void print(std::ostream &os) const{os<<"(ball)";};

      static Object* parse(const std::string& in, std::string& out);

      virtual Object* deepCopy(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new Ball();}

      virtual Object* inversion(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new Ball();}

      virtual char type() const{return 'b';}; 

    };

    class OppGoal : public StaticObject
    {
    public:
      OppGoal(){};

      ~OppGoal(){};

      virtual void print(std::ostream &os) const{os<<"(oppGoal)";};

      static Object* parse(const std::string& in, std::string& out);

      virtual Object* deepCopy(const vector<SetplayParameter*>& /*params*/,
			       const vector<PlayerReference*>& /*players*/) const{
    	  return new OppGoal();}

      virtual Object* inversion(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new OppGoal();}

      virtual char type() const{return 'g';}; 

    };

    class OwnGoal : public StaticObject{
    public:
      OwnGoal(){};

      ~OwnGoal(){};

      virtual void print(std::ostream &os) const{os<<"(ownGoal)";};

      static Object* parse(const std::string& in, std::string& out);

      virtual Object* deepCopy(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new OwnGoal();}

      virtual Object* inversion(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new OwnGoal();}

      virtual char type() const{return 'o';};

    };
  }
}
#endif
