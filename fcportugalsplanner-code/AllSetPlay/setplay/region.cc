/* -*- Mode: C++ -*- */
/*
 *Copyright:

 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintenance Group.
 Patrick Riley, Tom Howard, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler 

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/* This files defines a class region to specify regions for the coach messages */

#include <setplay/region.h>
#include <setplay/cond.h>
#include <setplay/setplayexception.h>
#include <math.h>
#include <cassert>
#include <limits>


#include "simpleParsers.h"

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace fcportugal::setplay;
using namespace std;

double PointSimple::distance(const PointSimple* point) const{

  

  return sqrt(pow(getX()->value()
		  - point->getX()->value(),2)
	      +pow(getY()->value()
		   - point->getY()->value(),2));
}

Point* Point::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  Point* pt;

  pt=PointSimple::parse(in,out,parameters,players);
  if(pt) return pt;

  pt=PointVar::parse(in,out,parameters,players);
  if(pt) return pt;

  pt=PointBall::parse(in,out,parameters,players);
  if(pt) return pt;

  pt=PointPlayer::parse(in,out,parameters,players);
  if(pt) return pt;

  pt=PointRel::parse(in,out,parameters,players);
  if(pt) return pt;

  pt=PointArith::parse(in,out,parameters,players);
  if(pt) return pt;

  return NULL;
}



void PointVar::print( std::ostream& out ) const{
  out << "(ptVar :name "<<name();
  if( instantiated() ){
    out<<" :value ";
    value_->print( out );
  }
  out<<")";
}
  

const string PointVar::instantiationText(const Context& world,
					 bool shortForm) const{
  if( instantiated() )
    return value_->instantiationText(world,shortForm);
  else
	  throw SetplayException("Trying to access instantiation text from not instantiated point");
  return "";
}

void PointVar::printPretty( std::ostream& out, 
			    const std::string& line_header ) const{
  if( instantiated() ){
    out << line_header << "point variable\n";
    out << line_header << name()<<"\n";
  }
  else
    value_->printPretty( out, line_header ); 
}

Point* PointVar::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string name;

  rule<> ptVar_p
    =  *space_p >> str_p("(ptVar") >> *space_p >> ":name" >> *space_p
		>> identifier_p[assign_a(name)] >> *space_p >> ")";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
						      ptVar_p, nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    PointVar* res=new PointVar(name);


    return res;
  }
  else {
    //Must check if it is simply one of the existent parameter names,
    //in case it does not start by '('
    result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
					   *space_p >> ch_p('('),
					   nothing_p);
    if(!result.hit){
      for(unsigned int i=0; i!= parameters.size(); i++){
	if(parameters.at(i)->type() == 'p'){
	  string parName=((PointVar*)parameters.at(i))->name();
	  ptVar_p
	    = *space_p >> str_p(parName.c_str());
	  
	  result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),ptVar_p,
						  nothing_p);
	  if(result.hit){
	    out=in.substr(result.length);
	  

	    PointVar* pt=(PointVar*)parameters.at(i);
	    ((SetplayParameter*)pt)->print(cout);
	    cout<<endl;
	  
	    return pt;

	  }
	}
      }
    } 
  }
  out=in;
  return NULL;
}

Point* PointVar::deepCopy(const vector<SetplayParameter*>& params,
			  const vector<PlayerReference*>& players) const{
  for(unsigned int i = 0; i!= params.size(); i++)
    if(params.at(i)->name()==name())
      return (PointVar*)params.at(i);
  throw SetplayException("Point parameter not found in parameter list:"+name());
  return NULL;
}

Point* PointVar::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  for(unsigned int i = 0; i!= params.size(); i++)
    if(params.at(i)->name()==name())
      return (PointVar*)params.at(i);
  throw SetplayException("Point parameter not found in parameter list:"+name());
  return NULL;
}

SetplayParameter* PointVar::deepCopy() const{
  PointVar* res=new PointVar(name());
  if(instantiated())
    // value_ must be instantiated, 
    // and therefore needs no parameter values...
    res->set((const PointSimple*)value_->deepCopy(*(new vector<SetplayParameter*>()),*(new vector<PlayerReference*>())));
  
  return res;
}




const PointSimple* PointVar::asPointSimple(const Context& world) const{
  assert(instantiated());
  return value_->asPointSimple(world);
}

const string PointSimple::instantiationText(const Context& world,
					    bool shortForm) const{
  if(shortForm)
    return M_x->instantiationText(world,shortForm)+";"
      +M_y->instantiationText(world,shortForm);
  else
    return "(pt :x "+ M_x->instantiationText(world,shortForm)
      +" :y "+M_y->instantiationText(world,shortForm)+")";
}

void PointSimple::print( std::ostream& out ) const{
  out << "(pt :x " ;
  M_x->print(out);
  out << " :y " ;
  M_y->print(out);
  out << ")";   
}

void PointSimple::printPretty( std::ostream& out, 
			       const std::string& line_header ) const{
  out << line_header << "Point(" ;
  M_x->print(out);
  out<< ", ";
  M_y->print(out);
  out << ")" << std::endl; 
}
    

Point* PointSimple::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  

  rule<> point_p
    = *space_p >> str_p("(pt")
	       >> *space_p >> ":x" >> *space_p ;
//  real_p[assign_a(x)] 
// 	       >>  >> *space_p >> real_p[assign_a(y)]
// 	       >> *space_p >> ")";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
						      point_p, nothing_p);

  if(result.hit){
    Decimal* x=NULL,*y=NULL;
    string rest;

    //Must parse x value
    x=(Decimal*)Decimal::parse(in.substr(result.length),rest,parameters);
    if(x!=NULL){
      //Check for ':y'
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					     *space_p >> ":y",
					     nothing_p);
      
      if(result.hit){
	//Must parse y value
	y=(Decimal*)Decimal::parse(rest.substr(result.length),rest,
				   parameters);

	//Check final ')'
	result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					       *space_p >> ')',
					       nothing_p);
	if(result.hit){
	  out=rest.substr(result.length);
	  return new PointSimple(x,y);
	}
      }
    }
  }
  out=in;
  return NULL;
}

const PointSimple* PointBall::asPointSimple(const Context& world) const{
  return new PointSimple(*world.ballPos());
}

const string PointBall::instantiationText(const Context& world,
					  bool shortForm) const{
 
  return world.ballPos()->instantiationText(world,shortForm);
}

void PointBall::print( std::ostream& out ) const{
  out << "(pt ball"<<")"; 
}

void PointBall::printPretty( std::ostream& out, 
			     const std::string& line_header ) const{
  out << line_header << "Point-Ball" << std::endl; 
}

Point* PointBall::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 
  rule<> point_p
    = *space_p >> str_p("(pt") >> *space_p >> "ball" >> *space_p >> ")";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),point_p, 
						      nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new PointBall();
  }
  else {
    out=in;
    return NULL;
  }
}

Point* PointBall::deepCopy(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const{
  PointBall* res = new PointBall(*this);
  
  return res;
}

void PointPlayer::print( std::ostream& out ) const{ 
  out << "(pt :player ";
  player->print(out) ;
  out<< ")";
}
    
void PointPlayer::printPretty( std::ostream& out, 
			       const std::string& line_header ) const{ 
  out << line_header << "Point-Player("; 
  player->print(out);
  out << ")" << std::endl;
}

const PointSimple* PointPlayer::asPointSimple(const Context& world) const{
  return world.playerPos(*player->value());
}


const string PointPlayer::instantiationText(const Context& world,
					    bool shortForm) const{
  return world.playerPos(*player->value())->instantiationText(world,shortForm);
   
}


Point* PointPlayer::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 
  rule<> point_p
    = *space_p >> str_p("(pt") >> *space_p >> str_p(":player") >> *space_p;
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),point_p, 
						      nothing_p);

  if(result.hit){
    //Must parse player reference
    string rest;
    PlayerReference* pl 
      = (PlayerReference*)PlayerReference::parse(in.substr(result.length),
						 rest,parameters,players);

    if(pl){
      // must check final ')'
      rule<> tail_p
	= *space_p >> str_p(")");

      result=BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),tail_p, nothing_p);

      if(result.hit){
	out=rest.substr(result.length);
	return new PointPlayer(pl);
      }
    }
  }
  out=in;
  return NULL;
}

bool PointPlayer::isOurSide() const{ 
  if(player->isRole() && !((PlayerRole*)player)->instantiated())
	  throw SetplayException("Accessing not instantiated player in PlayerPos!");
 
  return player->value()->team=="our";
}

bool PointPlayer::isTheirSide() const{
  if(player->isRole() && !((PlayerRole*)player)->instantiated())
	  throw SetplayException("Accessing not instantiated player in PlayerPos!");

  return player->value()->team=="opp";
}

unsigned int PointPlayer::getUNum() const{
  if(player->isRole() && !((PlayerRole*)player)->instantiated())
	  throw SetplayException("Accessing not instantiated player in PlayerPos!");

  return player->value()->number;
}


const PointSimple* PointRel::asPointSimple(const Context& world) const{
    const PointSimple* const origin_simple=m_origin->asPointSimple(world);
    return new PointSimple(new Decimal(*origin_simple->getX()+*x_offset),
			   new Decimal(*origin_simple->getY()+*y_offset));
 
}

void PointRel::print( std::ostream& out ) const{
  out << "(ptRel :x " ;
  x_offset->print(out);
  out << " :y "; 
  y_offset->print(out);
  out << " :pt ";;
  if( getOrigin() != NULL )
    getOrigin()->print(out);
  else throw SetplayException("Origin is NULL in PointRel");
  out << ")";
}

      
void PointRel::printPretty( std::ostream& out, 
			    const std::string& line_header ) const{
  out << line_header << "Point-Relative(" ;
  x_offset->print(out);
  out << ", " ;
  y_offset->print(out);
  out << " ";
  if( getOrigin() == NULL )
    out << "(null)";
  else
    getOrigin()->printPretty( out, line_header + " " );
  out << ")" << std::endl;
}

Point* PointRel::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  rule<> point_p
    = *space_p >> str_p("(ptRel") >> *space_p >> str_p(":x") >> *space_p;
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),point_p, 
						      nothing_p);
  
  if(result.hit){
    //Must parse x offset
    string rest;
    Decimal* x 
      = (Decimal*)Decimal::parse(in.substr(result.length),
				 rest,parameters);

    if(x){
      // must check ':y' 
      result=BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					    *space_p >> str_p(":y"),
					    nothing_p);

      if(result.hit){
	//Parse y offset
	Decimal* y
	  = (Decimal*)Decimal::parse(rest.substr(result.length),
				     rest,parameters);
	
	if(y){
	  // Check ':pt'
	  result=BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
						*space_p >> str_p(":pt"),
						nothing_p);
	  if(result.hit){
	    //parse point
	    Point* pt=Point::parse(rest.substr(result.length),
			   rest,parameters,players);

	    if(pt){
	      //parse final ')'
	      result=BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
						    *space_p >> str_p(")"),
						    nothing_p);
	      if(result.hit){
		out=rest.substr(result.length);
		return new PointRel(x,y,pt);
	      }
	    }
	  }
      }
    }
   }
  }
  out=in;
  return NULL;
}

Point* PointRel::deepCopy(const vector<SetplayParameter*>& params,
			  const vector<PlayerReference*>& players) const{
  return new PointRel(x_offset->deepCopy(params,players),
		      y_offset->deepCopy(params,players),
		      m_origin->deepCopy(params,players));
}

Point* PointRel::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  return new PointRel(x_offset->deepCopy(params,players),
		      y_offset->inversion(params,players),
		      m_origin->inversion(params,players));
}

const PointSimple* PointArith::asPointSimple(const Context& world) const{
  const PointSimple* const p1_simple=M_points[0]->asPointSimple(world);
  const PointSimple* const p2_simple=M_points[1]->asPointSimple(world);

  return 
    new PointSimple(new Decimal(M_arith_op->operate(*p1_simple->getX(),
						    *p2_simple->getX())),
		    new Decimal((M_arith_op->operate(*p1_simple->getY(),
						     *p2_simple->getY()))));
		    
};
  
void PointArith::print( std::ostream& out ) const{
  out << "(";
  
  if( M_arith_op == NULL )
    out << "(null)";
  else
    out << *M_arith_op;
  
  out << " ";
  
  if( M_points[ 0 ] == NULL )
    out << "(null)";
  else
    M_points[ 0 ]->print(out);
  
  out << " ";
  
  
  if( M_points[ 1 ] == NULL )
    out << "(null)";
  else
    M_points[ 1 ]->print(out);
  
  out << ")";
}
      
void PointArith::printPretty( std::ostream& out, 
			      const std::string& line_header ) const{ 
  out << line_header << "Point-Arith" << std::endl;
  
  if( M_points[ 0 ] == NULL )
    out << line_header << " (null)\n";
  else
    M_points[ 0 ]->printPretty( out, line_header + " " );
  
  if( M_points[ 1 ] == NULL )
    out << line_header << " (null)\n";
  else
    M_points[ 1 ]->printPretty( out, line_header + " " );
  
  if( M_arith_op == NULL )
    out << line_header << " (null)\n";
  else
    out << line_header << " operation: " << *M_arith_op << std::endl;
}

bool PointArith::setPoint( const int& i, Point* pt ){
  if (i < 0 || i > 1)
    return false;
  M_points[i] = pt;
  return true;
}

bool PointArith::setPoint( Point* pt ){
  M_points[ M_idx ] = pt;
  M_idx = (M_idx + 1) % 2;
  return true;
}

Point* PointArith::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	//TODO
	//Only prepared to deal with sum!!!
  
  rule<> point_p
    = *space_p >> str_p("(+");
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),point_p, 
						      nothing_p);
  
  if(result.hit){
    //Must parse first point
    string rest;
    Point* pt1
      = Point::parse(in.substr(result.length),
		     rest,parameters,players);

    if(pt1){
      //Parse second point
      Point* pt2
	= Point::parse(rest,
		       rest,parameters,players);
      
      if(pt2){
	//parse final ')'
	result=BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					      *space_p >> str_p(")"),
					      nothing_p);
	if(result.hit){
	  out=rest.substr(result.length);
	  return new PointArith(pt1,pt2,rcss::util::ArithOp::plus());
	}
      }
    }
  }
  out=in;
  return NULL;
}

Point* PointArith::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  //TODO: for now, only addition is allowed
  
  return new PointArith(M_points[0]->deepCopy(params,players),
			M_points[1]->deepCopy(params,players),
			rcss::util::ArithOp::plus());
}

Point* PointArith::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	//TODO: for now, only addition is allowed
	return new PointArith(M_points[0]->inversion(params,players),
			M_points[1]->inversion(params,players),
			rcss::util::ArithOp::plus());
}



Region* Region::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  Region* res;

  res=InstantiatedRegion::parse(in,out,parameters,players);
  if(res) return res;

  res=RegVar::parse(in,out,parameters,players);
  if(res) return res;

  return NULL;
}

Region* InstantiatedRegion::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  Region* res;

  res=RegPoint::parse(in,out,parameters,players);
  if(res) return res;

  res=RegNamed::parse(in,out,parameters,players);
  if(res) return res;

  res=RegArc::parse(in,out,parameters,players);
  if(res) return res;

  res=RegTri::parse(in,out,parameters,players);
  if(res) return res;

  res=RegRec::parse(in,out,parameters,players);
  if(res) return res;

  return NULL;
}

bool RegVar::includesPoint(const Point* const p,
			   const Context& world) const{
  if(!instantiated()){
	  throw SetplayException("Trying to check if non-instantiated region includes a point!");
    return false;
  }
  else
    return value_->includesPoint(p,world);
}


void RegVar::print( std::ostream& out ) const
{ 
  if( !instantiated() )
    out << "(regVar :name " << name()<<")";
  else
    value_->print( out );
}


const string RegVar::instantiationText(const Context& world,
				       bool shortForm) const{
  if( instantiated() )
    return value_->instantiationText(world,shortForm);
  else
	  throw SetplayException("Trying to access instantiation text from not instantiated region");
  return "";
}
 
  
void RegVar::printPretty( std::ostream& out, 
			  const std::string& line_header ) const
{
  if( !instantiated() ){
    out << line_header << "region variable\n";
    out << line_header << name()<<"\n";
  }
  else
    value_->printPretty( out, line_header ); 
}
    


Region* RegVar::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  string name;
  
  rule<> reg_p
    = *space_p >> str_p("(regVar") >> *space_p >> ":name" >> *space_p
	       >> identifier_p[assign_a(name)] >> *space_p >> ")";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),reg_p, 
						      nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new RegVar(name);
  }
  else {
    //Must check if it is simply one of the existent parameter names,
    //in case it does not start by '('
    result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
					   *space_p >> ch_p('('),
					   nothing_p);
    if(!result.hit){
      for(unsigned int i=0; i!= parameters.size();
	  i++){
	reg_p
	  = *space_p >> str_p(parameters.at(i)->name().c_str()) >> *space_p;
	
	result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),reg_p,
						nothing_p);
	if(result.hit && parameters.at(i)->type() == 'g'){
	  out=in.substr(result.length);
	  return (Region*)parameters.at(i);
	}
      } 
    }
  }
  out=in;
  return NULL;
}

Region* RegVar::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  if(name()=="subst"){//because of named regions which were set as 
    //RegVars with this name
    RegVar* ret = new RegVar(*this);
    ret->value_=value_->deepCopy(params,players);
    return ret;
  }
  for(unsigned int i = 0; i!= params.size(); i++)
    if(params.at(i)->name()==name())
      return (RegVar*)params.at(i);
  throw SetplayException("Region parameter not found in parameter list:"+name());
  return NULL;
}

Region* RegVar::inversion(const vector<SetplayParameter*>& params,
			 const vector<PlayerReference*>& players) const{
  if(name()=="subst"){//because of named regions which were set as 
    //RegVars with this name
    RegVar* ret = new RegVar(*this);
    ret->value_=value_->inversion(params,players);
    return ret;
  }
  for(unsigned int i = 0; i!= params.size(); i++)
    if(params.at(i)->name()==name())
      return (RegVar*)params.at(i);
  throw SetplayException("Region parameter not found in parameter list:"+name());
  return NULL;
}

SetplayParameter* RegVar::deepCopy() const{
  RegVar* res=new RegVar(name());
  if(instantiated())
    // value_ must be instantiated, and therefore needs no parameter values...
    res->set(value_->deepCopy(*(new vector<SetplayParameter*>()),*(new vector<PlayerReference*>())));
  
  return res;
}

const PointSimple* RegVar::getCentralPoint(const Context& world) const{
  if(instantiated())
    return value_->getCentralPoint(world);
  else throw SetplayException("Trying to access central point of not instantiated RegVar");
  return NULL;
}


    
void RegPoint::print( std::ostream& out ) const{ 
  if( M_point == NULL )
    out << "(null)";
  else
    M_point->print( out );
}
  
void RegPoint::printPretty( std::ostream& out, 
			    const std::string& line_header ) const
{
  out << line_header << "region point\n";
  if( M_point == NULL )
    out << line_header << " (null)\n";
  else
    M_point->printPretty( out, line_header ); 
}
    
Region* RegPoint::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  Point* pt = Point::parse(in,out,
			   parameters,players);

  if(pt) return new RegPoint(pt);

  out=in;
  return NULL;
}

const char* RegNamed::TRANSVERSE_REGION_NAMES[]={
  "our_back","our_middle","our_front",
  "their_front","their_middle","their_back"};
const char* RegNamed::LONGITUDINAL_REGION_NAMES[]={
  "far_left","mid_left","centre_left",
  "centre_right","mid_right","far_right"};
const char* RegNamed::RADIAL_REGION_NAMES[]={
  "sl_1","sl_2","sl_3","sl_4","sl_5","sl_6","sl_7","sl_8","sl_9",
  "sr_9","sr_8","sr_7","sr_6","sr_5","sr_4","sr_3","sr_2","sr_1"};


bool RegNamed::includesPoint(const Point* const p,
			     const Context& world) const{
  if(value!=NULL)
    return value->includesPoint(p,world);
  throw SetplayException( "Trying to check if a named region("+name+
		  ") includes a point, but this region has not yet been defined.");
  return false;
}

void RegNamed::print( std::ostream& out ) const{
  out << "(regNamed :name "<< name;
  if(value!=NULL){
    out <<"->value:" ;
    value->print(out);
  }
  out << ")";
}

void RegNamed::printPretty( std::ostream& out, 
			    const std::string& line_header ) const{ 
  out << line_header << "region named \"" 
      << name << "\"" ;
  if(value!=NULL){
    out <<" with value: ";
    value->print(out);
  }
  out<< std::endl;
}

Region* RegNamed::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
 string name;
  
  rule<> reg_p
    = *space_p >> str_p("(regNamed") >> *space_p >> ":name" >> *space_p 
	       >> identifier_p[assign_a(name)]  >> *space_p >> ")";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),reg_p, 
						      nothing_p);

  if(result.hit){
    out=in.substr(result.length);
    return new RegNamed(name);
  }
  else{
    out=in;
    return NULL;
  }
}

Region* RegNamed::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
  RegNamed* res= new RegNamed(name);
  if(value!=NULL)
    res->value=value->deepCopy(params,players);
  
  return res;
}

Region* RegNamed::inversion(const vector<SetplayParameter*>& params,
			   const vector<PlayerReference*>& players) const{
  // Check for "canonical" special cases
  if(name=="our_back"||name=="our_middle"||name=="our_front"
     ||name=="their_front"||name=="their_middle"||name=="their_back"
     ||name=="our_penalty_box"||name=="their_penalty_box"
     ||name=="our_middle_field"||name=="their_middle_field")
    // No inversion here
    return deepCopy(params,players);
  if(name=="left"){
    RegNamed* res= new RegNamed("right");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="right"){
    RegNamed* res= new RegNamed("left");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="far_left"){
    RegNamed* res= new RegNamed("far_right");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="mid_left"){
    RegNamed* res= new RegNamed("mid_right");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="centre_left"){
    RegNamed* res= new RegNamed("centre_right");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="centre_right"){
    RegNamed* res= new RegNamed("centre_left");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="mid_right"){
    RegNamed* res= new RegNamed("mid_left");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="far_right"){
    RegNamed* res= new RegNamed("far_left");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_1"){
    RegNamed* res= new RegNamed("sr_1");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_2"){
    RegNamed* res= new RegNamed("sr_2");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_3"){
    RegNamed* res= new RegNamed("sr_3");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_4"){
    RegNamed* res= new RegNamed("sr_4");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_5"){
    RegNamed* res= new RegNamed("sr_5");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_6"){
    RegNamed* res= new RegNamed("sr_6");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_7"){
    RegNamed* res= new RegNamed("sr_7");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_8"){
    RegNamed* res= new RegNamed("sr_8");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sl_9"){
    RegNamed* res= new RegNamed("sr_9");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_9"){
    RegNamed* res= new RegNamed("sl_9");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_8"){
    RegNamed* res= new RegNamed("sl_8");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_7"){
    RegNamed* res= new RegNamed("sl_7");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_6"){
    RegNamed* res= new RegNamed("sl_6");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_5"){
    RegNamed* res= new RegNamed("sl_5");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_4"){
    RegNamed* res= new RegNamed("sl_4");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_3"){
    RegNamed* res= new RegNamed("sl_3");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_2"){
    RegNamed* res= new RegNamed("sl_2");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  if(name=="sr_1"){
    RegNamed* res= new RegNamed("sl_1");
    if(value!=NULL)
      res->value=value->inversion(params,players);
    return res;
  }
  string newName("Inv_");
  newName.append(name);
  RegNamed* res= new RegNamed(newName);
  if(value!=NULL)
    res->value=value->inversion(params,players);
  
  return res;
}

void RegNamed::substituteNamedRegions(const Context& world) const{
  const Region* reg;
  if((reg=world.nameSubstitutions().getSubstitutionRegion(name))!=NULL){
      value = reg;
      return;
    }
  
  throw SetplayException("SP: Area name not found in NameSubstitutions:"+name);
}

const string
RegNamed::getLongitudinalRegion(const PointSimple* const pos,
				const Context& world){

  for(unsigned int i=0; i!=NUM_LONGITUDINAL_REGIONS;i++)
    if(world.nameSubstitutions()
	.getSubstitutionRegion(LONGITUDINAL_REGION_NAMES[i])
       &&(world.nameSubstitutions()
	  .getSubstitutionRegion(LONGITUDINAL_REGION_NAMES[i]))
       ->includesPoint(pos,world))
      return LONGITUDINAL_REGION_NAMES[i];

  throw SetplayException("SP: unexpected LongitudinalRegion for point.");

  return "";
}

const string
RegNamed::getTransversalRegion(const PointSimple* const pos,
			       const Context& world){
 for(unsigned int i=0; i!=NUM_TRANSVERSE_REGIONS;i++)
   if(world.nameSubstitutions()
      .getSubstitutionRegion(TRANSVERSE_REGION_NAMES[i])
      &&(world.nameSubstitutions()
	 .getSubstitutionRegion(TRANSVERSE_REGION_NAMES[i]))
      ->includesPoint(pos,world))
      return TRANSVERSE_REGION_NAMES[i];


 throw SetplayException("SP: unexpected TransversalRegion for point,");

 return "";
}

const string  
RegNamed::getRadialRegion(const PointSimple* const pos,
		const Context& world){

	for(unsigned int i=0; i!=NUM_RADIAL_REGIONS;i++){

		if(world.nameSubstitutions().getSubstitutionRegion(RADIAL_REGION_NAMES[i])&&
				(world.nameSubstitutions().getSubstitutionRegion(RADIAL_REGION_NAMES[i]))
				->includesPoint(pos,world)){

			return RADIAL_REGION_NAMES[i];
		}
	}

	std::ostringstream o;
	o<<"SP: no RadialRegion found for point:";
	pos->print(o);
	o<<", considering "<<NUM_RADIAL_REGIONS<<" regions.";
	throw SetplayException(o.str());

	return "";
}

// To make distance increase with separation, index difference must be 
// calculated to some power, originally 4 for longitudinal and transversal
// and 2 for radial. I thought this was too high, and changed to 2 and 1.5.
unsigned int
RegNamed::longitudinalRegionDistance(const std::string& reg1,
				     const std::string& reg2){
  int index1=(int)longitudinalRegionIndex(reg1);
  int index2=(int)longitudinalRegionIndex(reg2);


  return (unsigned int)pow((double)index1-index2,2);
}

unsigned int
RegNamed::transverseRegionDistance(const std::string& reg1,
				   const std::string& reg2){
  int index1=(int) transverseRegionIndex(reg1);
  int index2=(int) transverseRegionIndex(reg2);


  return (unsigned int)pow((double)index1-index2,2);
}

unsigned int
RegNamed::radialRegionDistance(const std::string& reg1,
			       const std::string& reg2){ 
  int index1=(int)radialRegionIndex(reg1);
  int index2=(int)radialRegionIndex(reg2);

  
  return (unsigned int)pow((double)index1-index2,1.5);
}

unsigned int RegNamed::longitudinalRegionIndex(const std::string& reg){
  for(unsigned int i=0; i!=NUM_LONGITUDINAL_REGIONS;i++)
    if(LONGITUDINAL_REGION_NAMES[i]==reg)
      return i;

  return NUM_LONGITUDINAL_REGIONS;
}

unsigned int RegNamed::transverseRegionIndex(const std::string& reg){
  for(unsigned int i=0; i!=NUM_TRANSVERSE_REGIONS;i++)
    if(TRANSVERSE_REGION_NAMES[i]==reg)
      return i;
 cerr<<"SP: CBR unknown transverse region:"<<reg<<endl;
  return NUM_TRANSVERSE_REGIONS;
}
unsigned int RegNamed::radialRegionIndex(const std::string& reg){
  for(unsigned int i=0; i!=NUM_RADIAL_REGIONS;i++)
    if(RADIAL_REGION_NAMES[i]==reg)
      return i;

  cerr<<"SP: CBR unknown radial region:"<<reg<<endl;
  return NUM_RADIAL_REGIONS;
}

RegArc::RegArc( Point*  center, 
		Decimal* start_rad, Decimal* end_rad, 
		Decimal* start_ang, 
		Decimal* span_ang )
  : M_start_rad( start_rad ),
    M_end_rad( end_rad ),
    M_start_ang( start_ang ),
    M_span_ang( span_ang ),
    m_center( center )        
{}
RegArc::RegArc( Point*  center, 
		Decimal* start_rad, Decimal* end_rad)
  : M_start_rad( start_rad ),
    M_end_rad( end_rad ),
    M_start_ang( new Decimal(0) ),
    M_span_ang( new Decimal(360) ),
    m_center( center )        
{}


RegArc::RegArc( Point* center, // for circles...
		Decimal* rad): M_start_rad( new Decimal(0) ),
			       M_end_rad( rad ),
			       M_start_ang( new Decimal(0) ),
			       M_span_ang( new Decimal(360) ),
			       m_center( center ) 
{}


const string RegArc::instantiationText(const Context& world,
				       bool shortForm) const{
  if(m_center==NULL ||M_start_rad ==NULL || M_end_rad==NULL
     || M_start_ang==NULL || M_span_ang==NULL){
	  throw SetplayException("Trying to access instantiation text from Arc with NULL components!");
    return "";
  }
  stringstream ss;
  if(shortForm)
    ss<<m_center->instantiationText(world,shortForm)
      <<";"<<M_start_rad->instantiationText(world,shortForm)
      <<";"<<M_end_rad->instantiationText(world,shortForm)
      <<";"<<M_start_ang->instantiationText(world,shortForm)
      <<";"<<M_span_ang->instantiationText(world,shortForm);
  else
    ss << "(arc :center "
       << m_center->instantiationText(world,shortForm)
       << " :radius_small "
       << M_start_rad->instantiationText(world,shortForm)
       << " :radius_large " 
       <<  M_end_rad->instantiationText(world,shortForm)
       << " :angle_begin "
       <<  M_start_ang->instantiationText(world,shortForm)
       << " :angle_span "
       <<  M_span_ang->instantiationText(world,shortForm)
       << ")";

  return ss.str();
}

void
RegArc::print( std::ostream& out ) const{
  out << "(arc";
  out << " :center ";
  m_center->print(out);
  out << " :radius_small ";
  M_start_rad->print(out);
  out << " :radius_large " ;
  M_end_rad->print(out);
  out << " :angle_begin ";
  M_start_ang->print(out);
  out << " :angle_span ";
  M_span_ang->print(out);
  out << ")";
}

void
RegArc::printPretty( std::ostream& out, const std::string& line_header ) const{
  out << line_header
      << "Arc: "
      << "center=";
  m_center->printPretty( out, line_header + " " );
  out << "\tradius=";
  M_start_rad->print(out);
  out << " to ";
  M_end_rad->print(out);
  out << "\tangle=";
  M_start_ang->print(out);
  out << " for ";
  M_span_ang->print(out);
  out << std::endl;
}
Region* RegArc::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  rule<> reg_p = *space_p >> str_p("(arc") >> *space_p 
			  >> ":center" >> *space_p;
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
						      reg_p, nothing_p);
  
  if(result.hit){
    //Must parse point
    string rest;
    Point* ctr=Point::parse(in.substr(result.length),rest,parameters,players);

    if(ctr){ 
      Decimal *dec, *rad_min= new Decimal(0),// for circles...
	*rad_max=NULL;
      
      //Point was parsed, must parse the rest
      //start radius
      rule<> rule_p= *space_p >> ":radius_small" >> *space_p;
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					     rule_p,
					     nothing_p);
      if(result.hit) {
	rad_min=(Decimal*)Decimal::parse(rest.substr(result.length),rest,
					 parameters);
	  if(!rad_min){out=in;
	    return NULL; }
      }
      
      
      //end radius
      rule_p=*space_p >> ":radius_large" ;
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					     rule_p,
					     nothing_p);
      if(result.hit){
	rad_max=(Decimal*)Decimal::parse(rest.substr(result.length),rest,
					 parameters);
	  if(!rad_max){out=in;
	    return NULL; }
      }
      
      //Create new Arc
      RegArc* res=new RegArc(ctr,rad_min,rad_max);


      //angle start
      rule_p=*space_p >> ":angle_begin" >> *space_p;
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					     rule_p,
					     nothing_p);
      if(result.hit){
	dec=(Decimal*)Decimal::parse(rest.substr(result.length),rest,
				     parameters);
	  if(dec)
	    res->setStartAng(dec);
	  else{out=in;
	    return NULL; }
      }

     
      //angle span
      rule_p=*space_p >> ":angle_span" >> *space_p;
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					     rule_p,
					     nothing_p);
      if(result.hit){
	dec=(Decimal*)Decimal::parse(rest.substr(result.length),rest,
				     parameters);
	  if(dec)
	    res->setAngSpan(dec);
	  else{out=in;
	    return NULL; }
      }
      
     
      rule_p=*space_p >> ")";
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),rule_p, nothing_p);
     
      if(result.hit){
	out=rest.substr(result.length);
	return res;
	}
      else {
	out=in;
	return NULL;
      }
    }
    else {
      out=in;
      return NULL;
    }
  }
  else {
    out=in;
    return NULL;
  }
}

Region* RegArc::deepCopy(const vector<SetplayParameter*>& params,
			 const vector<PlayerReference*>& players) const{
  return new RegArc( m_center->deepCopy(params,players),
		     M_start_rad->deepCopy(params,players), 
		     M_end_rad->deepCopy(params,players),
		     M_start_ang->deepCopy(params,players),
		     M_span_ang->deepCopy(params,players) ); 
}

Region* RegArc::inversion(const vector<SetplayParameter*>& params,
			  const vector<PlayerReference*>& players) const{
  // CAUTION this is a bit complex, not sure it is right...
  return new RegArc( m_center->inversion(params,players),
		     M_start_rad->deepCopy(params,players), 
		     M_end_rad->deepCopy(params,players),
		     // sum 180deg to angle?  Think so...
		     // If not instantiated (ie, if a parameter...) then keep
		     // it equal...
		     (M_start_ang->name()=="UNNAMED"?
		      new Decimal(M_start_ang->value())
		      :M_start_ang->deepCopy(params,players)),
		     M_span_ang->deepCopy(params,players) ); 
}

bool 
RegArc::setRad( Decimal* start_rad, Decimal* end_rad)
{
  if (start_rad > end_rad)
    return false;
  M_start_rad = start_rad;
  M_end_rad = end_rad;
  return true;
}

bool
RegArc::setAng( Decimal* start_ang, Decimal* span_ang )
{
  if ( start_ang->value() > 180 || start_ang->value() < -180 
       || span_ang->value() <0 ||span_ang->value() >360)
    return false;
  M_start_ang = start_ang;
  M_span_ang = span_ang;
  return true;
}


bool RegArc::includesPoint(const Point* const p,
			   const Context& world) const{
  double distance 
    = sqrt(pow(m_center->asPointSimple(world)->getX()->value()
	       - p->asPointSimple(world)->getX()->value(),2)
	   +pow(m_center->asPointSimple(world)->getY()->value()
		- p->asPointSimple(world)->getY()->value(),2));
  
  float anglePX=angle(rcss::geom::Vector2D(1,0),
		      rcss::geom::Vector2D(p->asPointSimple(world)
					   ->getX()->value(),
					   p->asPointSimple(world)
					   ->getY()->value()))/M_PI*180;
  
  float min1=-180;
  float max1=180;
  float min2=-180;
  float max2=180;
  if(M_start_ang->value()+M_span_ang->value()>180){
    max1=M_start_ang->value()+M_span_ang->value()-360;
    min2=M_start_ang->value();
  }
  else{
    min1=M_start_ang->value();
    max1=M_start_ang->value()+M_span_ang->value();
    min2=0;
    max2=-1;
  }
  return distance>=M_start_rad->value() 
    && (distance<=M_end_rad->value()|| M_end_rad->value()== -1)
    && (((anglePX>=min1) && (anglePX<=max1)) || 
	((anglePX>=min2) && (anglePX<=max2)));
};


const PointSimple* RegArc::getCentralPoint(const Context& world) const{
  if(M_span_ang->value()==180 && M_start_rad->value()==0)
    return m_center->asPointSimple(world);
  else{
    double mid_rad=(M_start_rad->value()+M_end_rad->value())/2;
    double mid_ang=M_start_ang->value()+M_span_ang->value()/2;

    return 
      new PointSimple(new Decimal(mid_rad*cos(mid_ang)
				  + *m_center->asPointSimple(world)->getX()),
		      new Decimal(mid_rad*sin(mid_ang)
				  + *m_center->asPointSimple(world)->getY()));
  }
}



const string RegTri::instantiationText(const Context& world,
				       bool shortForm) const{
  if(m_points[0]==NULL || m_points[1]==NULL
     || m_points[2]==NULL){
	  throw SetplayException("Trying to access instantiation text from Tri with NULL components!");
    return "";
  }
  stringstream ss;
  if(shortForm)
    ss<<m_points[0]->instantiationText(world,shortForm)
      <<";"<<m_points[1]->instantiationText(world,shortForm)
      <<";"<<m_points[2]->instantiationText(world,shortForm);
  else
    ss<< "(triang :points (list "<<m_points[0]->instantiationText(world,
								  shortForm)
      <<" "<<m_points[1]->instantiationText(world,shortForm)
      <<" "<<m_points[2]->instantiationText(world,shortForm)<<"))";

  return ss.str();
}

void RegTri::print( std::ostream& out ) const{
  out<<"(triang :points (list ";
  for(unsigned int i =0; i!=3;i++){
    m_points[i]->print(out);
    out<<" ";
  }
  out<<"))";
}
    
void RegTri::printPretty( std::ostream& out, 
			  const std::string& line_header ) const{
  out<<line_header;
  print(out);
}

bool RegTri::includesPoint(const Point* const p,
		const Context& world) const{
	// the point is inside the triangle if the angles between
	// each side and the point have the same sign 

	rcss::geom::Vector2D pp(p->asPointSimple(world)->getX()->value(),
			p->asPointSimple(world)->getY()->value());
	rcss::geom::Vector2D p0(m_points[0]->asPointSimple(world)->getX()->value(),
			m_points[0]->asPointSimple(world)->getY()->value());
	rcss::geom::Vector2D p1(m_points[1]->asPointSimple(world)->getX()->value(),
			m_points[1]->asPointSimple(world)->getY()->value());
	rcss::geom::Vector2D p2(m_points[2]->asPointSimple(world)->getX()->value(),
			m_points[2]->asPointSimple(world)->getY()->value());

	return angle(pp-p0,p1-p0)*angle(pp-p1,p2-p1)>=0
			&& angle(pp-p1,p2-p1)*angle(pp-p2,p0-p2)>=0
			&& angle(pp-p0,p1-p0)*angle(pp-p2,p0-p2)>=0;
};

Region* RegTri::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const
{
  Point* pts[ 3 ];
  for( int i = 0; i < 3; ++i ){
    if( m_points[ i ] )
      pts[ i ] = m_points[ i ]->deepCopy(params,players);
  }
  return  new RegTri( pts[ 0 ],
		      pts[ 1 ],
		      pts[ 2 ] ) ; 
}

Region* RegTri::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const
{
  Point* pts[ 3 ];
  for( int i = 0; i < 3; ++i ){
    if( m_points[ i ] )
      pts[ i ] = m_points[ i ]->inversion(params,players);
  }
  return  new RegTri( pts[ 0 ],
		      pts[ 1 ],
		      pts[ 2 ] ) ; 
}


const PointSimple* RegTri::getCentralPoint(const Context& world) const{
  return new 
    PointSimple(new Decimal((m_points[0]->asPointSimple(world)->getX()->value()
			     +m_points[1]->asPointSimple(world)->getX()->value()
			     +m_points[2]->asPointSimple(world)->getX()->value())/3),
		new Decimal((m_points[0]->asPointSimple(world)->getY()->value()
			     +m_points[1]->asPointSimple(world)->getY()->value()
			     +m_points[2]->asPointSimple(world)->getY()->value())/3));
}


Region* RegTri::parse(const string& in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
  rule<> reg_p = *space_p >> str_p("(triang") >> *space_p >> ":points" 
			  >> *space_p >> "(list";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),reg_p, 
						      nothing_p);
  
  if(result.hit){
    string rest;

    //Must parse points
    Point* pt1=Point::parse(in.substr(result.length),rest,parameters,players);
    Point* pt2=Point::parse(rest,rest,parameters,players);
    Point* pt3=Point::parse(rest,rest,parameters,players);

    if(pt1 && pt2 && pt3){ 
      //Create new Arc
      RegTri* res=new RegTri(pt1,pt2,pt3);
      
      //Check final ')'
      rule<> rule_p=*space_p >> ")">>*space_p >> ")";
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),rule_p, space_p);
     
      if(result.hit){
	out=rest.substr(result.length);
	return res;
	}
    }
  }  
  out=in;
  return NULL;
}

RegRec::RegRec( Point* pt0, Point* pt1 ):InstantiatedRegion(){
  m_points[0]=pt0;
  m_points[1]=pt1;
}


const string RegRec::instantiationText(const Context& world,
				       bool shortForm) const{
  if(m_points[0]==NULL || m_points[1]==NULL){
	  throw SetplayException("Trying to access instantiation text from Rec with NULL components!");
    return "";
  }
  stringstream ss;
  if(shortForm)
    ss<<m_points[0]->instantiationText(world,shortForm)
      <<";"<<m_points[1]->instantiationText(world,shortForm);
  else
    ss<< "(rec "<<m_points[0]->instantiationText(world,shortForm)
      <<" "<<m_points[1]->instantiationText(world,shortForm)<<")";

  return ss.str();
}

void RegRec::print( std::ostream& out ) const{
  out << "(rec :points (list ";
  for( unsigned int i = 0; i < 2; ++i ){
    if( m_points[ i ] == NULL )
      out << "(null) ";
    else{
      m_points[ i ]->print(out);
      out<<" ";
    }
  }
  out << "))";
}

void RegRec::printPretty( std::ostream& out, 
			  const std::string& line_header ) const{
  out << line_header
      << "Rectangle: ";
  for( unsigned int i = 0; i < 2; ++i )
    {
      if( m_points[ i ] == NULL )
	out << "(null) ";
      else
	{
	  m_points[ i ]->printPretty( out, line_header + " " );
	  out << " ";
	}
    }
  out << std::endl;
}

Region* RegRec::parse(const string& in, string& out,
		const vector<SetplayParameter*> &parameters,
		const vector<PlayerReference*>& players){
  rule<> reg_p = *space_p >> str_p("(rec") >> *space_p >> ":points"
			  >> *space_p >> "(list";
  
  parse_info<> result =BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),
						      reg_p, nothing_p);
  
  if(result.hit){
    string rest;

    //Must parse points
    Point* pt1=Point::parse(in.substr(result.length),rest,parameters,players);
    Point* pt2=Point::parse(rest,rest,parameters,players);

    if(pt1 && pt2){ 
      //Create new Arc
      RegRec* res=new RegRec(pt1,pt2);
      
      //Check final ')'
      rule<> rule_p=*space_p >> ")">>*space_p >> ")";
      result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),rule_p, space_p);
     
      if(result.hit){
	out=rest.substr(result.length);
	return res;
	}
    }
  }
  out=in;
  return NULL;
}


bool RegRec::includesPoint(const Point* const p,const Context& world) const{
  double min_x, max_x,min_y,max_y;
  if(m_points[0]->asPointSimple(world)->getX()->value()
     < m_points[1]->asPointSimple(world)->getX()->value()){
    min_x=m_points[0]->asPointSimple(world)->getX()->value();
    max_x=m_points[1]->asPointSimple(world)->getX()->value();
  }
  else{
    min_x=m_points[1]->asPointSimple(world)->getX()->value();
    max_x=m_points[0]->asPointSimple(world)->getX()->value();
  }

  if(m_points[0]->asPointSimple(world)->getY()->value()
     < m_points[1]->asPointSimple(world)->getY()->value()){
    min_y=m_points[0]->asPointSimple(world)->getY()->value();
    max_y=m_points[1]->asPointSimple(world)->getY()->value();
  }
  else{
    min_y=m_points[1]->asPointSimple(world)->getY()->value();
    max_y=m_points[0]->asPointSimple(world)->getY()->value();
  }
  const PointSimple* const simple=p->asPointSimple(world);
 
  return min_x<=simple->getX()->value() 
    && simple->getX()->value()<=max_x
    && min_y<=simple->getY()->value() 
    && simple->getY()->value()<=max_y;
}

void RegRec::setAllPts( Point* pt0,Point* pt1 ){
  m_points[0]=pt0;
  m_points[1]=pt1;
}

const PointSimple* RegRec::getCentralPoint(const Context& world) const{
  return new const 
    PointSimple(new Decimal((m_points[0]->asPointSimple(world)->getX()->value()
			     +m_points[1]->asPointSimple(world)->getX()->value())/2),
		new Decimal((m_points[0]->asPointSimple(world)->getY()->value()
			     +m_points[1]->asPointSimple(world)->getY()->value())/2));
}

Region* RegRec::deepCopy(const vector<SetplayParameter*>& params,
			 const vector<PlayerReference*>& players) const{
  Point* pts[ 2 ];
  for( int i = 0; i < 2; ++i )
    {
      pts[ i ] = m_points[ i ]->deepCopy(params,players);
    }
  return new RegRec( pts[ 0 ], pts[ 1 ] ); 
}

Region* RegRec::inversion(const vector<SetplayParameter*>& params,
			  const vector<PlayerReference*>& players) const{
  // There seems to be no specific order on the two points: must be any 
  // opposite corners. Therefore, it suffices to invert each of the points...
  // I think...
  Point* pts[ 2 ];
  for( int i = 0; i < 2; ++i )
    {
      pts[ i ] = m_points[ i ]->inversion(params,players);
    }
  return new RegRec( pts[ 0 ], pts[ 1 ] ); 
}

const Point* RegRec::getPt (unsigned int& i) const{
  if( i < 2 )
    return m_points[ i ];
  else
    return NULL;
}



