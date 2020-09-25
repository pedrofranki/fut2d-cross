/* -*- Mode: C++ -*- */
/*
 *Copyright:

 Copyright (C) 2006 Luis Mota


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

/* This files defines a class region to specify regions for the coach messages
   It has been changed to cope with variables (LMOTA)
 */

#ifndef H_REGION
#define H_REGION

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <setplay/clang/vector.h>
#include <setplay/clang/visitor.h>
#include <setplay/clang/clangutil.h>
#include <setplay/clang/arithop.h>
#include <setplay/clang/hasa.h>
#include <list>

#include "parameter.h"
#include "playerReference.h"

using namespace std;

namespace fcportugal{
namespace setplay{
class PointSimple;
class Context;

class Point{
public:
	inline Point(){}

	inline virtual ~Point(){}

	virtual void print( std::ostream& out ) const = 0;

	virtual void
	printPretty( std::ostream& out,
			const std::string& line_header ) const = 0;

	virtual const string instantiationText(const Context& world,
			bool shortForm) const=0;

	virtual const PointSimple*
	asPointSimple(const Context& world) const=0;

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;



};


class PointVar : public Point, public SetplayParameter
{
public:
	inline PointVar(const string name):Point(),SetplayParameter(name),value_(NULL){}
	inline ~PointVar(){};

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	inline virtual char type() const{return 'p';};

	const PointSimple* asPointSimple(const Context& world) const;


	virtual void print(std::ostream& out) const;
	virtual void printPretty(std::ostream& out,
			const std::string& line_header) const;

	inline void set(const PointSimple* value){value_=value;
	m_instantiated=true;}

	virtual void uninstantiate(){value_=NULL;m_instantiated=false;}

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual SetplayParameter* deepCopy() const;

	virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	//virtual SetplayParameter* inversion() const;

	// Should do nothing: a var is a PointSimple, has no references to ball
	// or player positions
	virtual void
	substituteBallAndPlayersPositions(const PointSimple* /*ball*/,
			vector<const PointSimple* >* /*players*/,
			vector<const PointSimple* >* /*opponents*/){};

private:
	const PointSimple* value_;
};


class PointSimple : public Point
{
public:
	inline PointSimple(Decimal* x,Decimal* y ):Point(),M_x(x),M_y(y){}

	inline ~PointSimple() {}

	inline virtual const
	PointSimple* asPointSimple(const Context& /*world*/) const{
		return new PointSimple(*this); }

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	virtual void print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline Decimal* getX() const{return M_x;}

	inline Decimal* getY() const{return M_y;}

	inline void setVec(Decimal* x,Decimal* y){M_x=x;M_y=y;}

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new PointSimple(M_x->deepCopy(params,players),
			M_y->deepCopy(params,players));}

	inline virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new PointSimple(M_x->deepCopy(params,players),
			M_y->inversion(params,players));}


	inline virtual void
	substituteBallAndPlayersPositions(const PointSimple* /*ball*/,
			vector<const PointSimple* >* /*players*/,
			vector<const PointSimple* >* /*opponents*/){};

	double distance(const PointSimple* point) const;

private:
	Decimal* M_x;
	Decimal* M_y;
};
}
}

inline bool
operator== ( const fcportugal::setplay::PointSimple& a, 
		const fcportugal::setplay::PointSimple& b ){
	return *(a.getX())==  *(b.getX())
			&& *(a.getY())== *( b.getY());
}


namespace fcportugal{
namespace setplay{
class PointBall : public Point
{
public:
	inline PointBall():Point(){}

	inline ~PointBall(){}

	virtual const PointSimple*
	asPointSimple(const Context& world) const;

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	virtual void print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return deepCopy(params,players);}

};


class PointPlayer : public Point
{
public:
	inline PointPlayer():Point(),player(NULL){}

	inline PointPlayer(const PlayerReference* player_)
	:Point(),player(player_){}

	inline ~PointPlayer(){}

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	virtual const PointSimple*
	asPointSimple(const Context& world) const;


	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline Point*
	deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players)const{
		return new PointPlayer((PlayerReference*)player->deepCopy(params,
				players));}

	inline Point*
	inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players)const{
		return new PointPlayer((PlayerReference*)player->deepCopy(params,
				players));}

	/* class specific */
	bool isOurSide() const;

	bool isTheirSide() const;

	unsigned int getUNum() const;

	const PlayerReference* getPlayer() const { return player; }
private:
	const PlayerReference* player;
};



class PointRel : public Point{
public:

	//      PointRel()
	//    {}

	inline PointRel(Decimal* x,Decimal* y,Point* origin )
	: m_origin(origin),x_offset(x),y_offset(y){}

	inline virtual ~PointRel(){}

	virtual const PointSimple*
	asPointSimple(const Context& world) const;

	inline virtual const string instantiationText(const Context& world,
			bool shortForm) const
	{return asPointSimple(world)->instantiationText(world,shortForm);}

	virtual void print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	inline PointSimple getOffset() const{return PointSimple(x_offset,y_offset);}

	inline void set(const PointSimple& offset)
	{x_offset=offset.getX();y_offset=offset.getY();}

	inline const Point* getOrigin() const{return m_origin;}

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);


	virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;
private:
	Point*  m_origin;
	Decimal* x_offset;
	Decimal* y_offset;


};

class PointArith : public Point
{
public:
	inline PointArith():Point(),M_arith_op( NULL ),M_idx( 0 ){}

	inline PointArith(Point* pt1,Point* pt2,
			const rcss::util::ArithOp& arith_op ) 
	: Point(),M_arith_op( &arith_op ),M_idx( 0 )
	{M_points[ 0 ] = pt1;M_points[ 1 ] = pt2;}

	inline ~PointArith(){}

	virtual const PointSimple*
	asPointSimple(const Context& world) const;

	inline virtual const string instantiationText(const Context& world,
			bool shortForm) const
	{return asPointSimple(world)->instantiationText(world,shortForm);}


	virtual void print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	bool
	setPoint( const int& i, Point* pt );

	inline void
	setAllPoints(Point* pt0,
			Point*& pt1 ){M_points[0] = pt0;M_points[1] = pt1;}

	bool setPoint( Point* pt );

	inline const  Point*
	getPoint( const int& i ){return (i>=0 && i<=1)?M_points[i]:NULL;}

	inline const rcss::util::ArithOp*
	getOp() const{return M_arith_op;}

	inline const rcss::util::ArithOp&
	setOp(const rcss::util::ArithOp& arith_op){return *(M_arith_op=&arith_op);}

	static Point* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Point* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Point* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

private:
	Point* M_points[2];
	const rcss::util::ArithOp* M_arith_op;
	unsigned int M_idx;
};




class RegPoint;

class Region{
public:
	inline Region(){}

	inline virtual ~Region(){}

	virtual const string instantiationText(const Context& world,
			bool shortForm) const=0;

	virtual bool includesPoint(const Point* const p,
			const Context& world) const =0;

	virtual void print( std::ostream& out ) const=0;

	virtual void printPretty( std::ostream& out,
			const std::string& line_header ) const=0;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual void substituteNamedRegions(const Context& world) const=0;

	virtual const PointSimple* getCentralPoint(const Context& world) const=0;
};


class InstantiatedRegion:public Region
{
public:
	inline InstantiatedRegion():Region(){}

	inline virtual ~InstantiatedRegion(){}

	virtual bool includesPoint(const Point* const p,
			const Context& world) const =0;

	virtual void print(std::ostream& out) const=0;

	virtual void printPretty(std::ostream& out,
			const std::string& line_header) const=0;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const=0;

	virtual void substituteNamedRegions(const Context& world) const=0;
};

class RegVar : public Region, public SetplayParameter{
public:
	inline RegVar(string const name):Region(),
	SetplayParameter(name),value_(NULL){}
	inline ~RegVar(){};

	virtual bool includesPoint(const Point* const p,
			const Context& world) const;

	//reGion, to avoid confusion with playerRole
	inline virtual char type() const{return 'g';};

	inline const Region* value() const {return value_;};

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	virtual void print( std::ostream& out ) const;

	virtual void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	inline void set(Region* value) {value_=value; m_instantiated=true;}

	virtual void uninstantiate(){value_=NULL;m_instantiated=false;}

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual SetplayParameter* deepCopy() const;

	//virtual SetplayParameter* inversion() const;

	inline virtual void substituteNamedRegions(const Context& world) const{
		if(instantiated())value_->substituteNamedRegions(world);}

	virtual const PointSimple* getCentralPoint(const Context& world) const;

private:
	Region* value_;
};


class RegPoint : public InstantiatedRegion
{
public:
	inline RegPoint(const Point* point):InstantiatedRegion(), M_point( point ){}
	inline ~RegPoint(){}

	inline virtual bool includesPoint(const Point* const p,
			const Context& world) const{
		return p!=NULL && *(M_point->asPointSimple(world)) == *(p->asPointSimple(world));}


	inline virtual const string instantiationText(const Context& world,
			bool shortForm) const{
		return M_point->instantiationText(world,shortForm);}

	virtual void print( std::ostream& out ) const;

	virtual void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);


	inline const Point* getPoint() const{ return M_point; };

	inline void setPoint(Point* point){M_point=point;}

	inline virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const{
		return new RegPoint(M_point->deepCopy(params,players));}

	inline virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players)
	const{
		return new RegPoint(M_point->inversion(params,players));}

	inline virtual void
	substituteNamedRegions(const Context& /*world*/) const{};

	inline virtual
	const PointSimple* getCentralPoint(const Context& world) const
	{return M_point->asPointSimple(world);}
private:
	const Point* M_point;
};



class RegNamed : public InstantiatedRegion{
public:
	// Some named regions are seen as "canonical", and must therefore
	// exist in every impementation:
	// "our_back","our_middle","our_front","their_front","their_middle",
	// "their_back","far_left","mid_left","centre_left","centre_right",
	// "mid_right","far_right","sl_1","sl_2","sl_3","sl_4","sl_5","sl_6",
	// "sl_7","sl_8","sl_9","sr_9","sr_8","sr_7","sr_6","sr_5","sr_4",
	// "sr_3","sr_2","sr_1"
	static const unsigned int NUM_TRANSVERSE_REGIONS=6;
	static const char* TRANSVERSE_REGION_NAMES[];
	static const unsigned int NUM_LONGITUDINAL_REGIONS =6;
	static const char* LONGITUDINAL_REGION_NAMES[];
	static const unsigned int NUM_RADIAL_REGIONS =18;
	static const char* RADIAL_REGION_NAMES[];

	inline RegNamed( std::string name_ ):InstantiatedRegion(),
			name(name_),value(NULL){}

	inline ~RegNamed(){}

	inline const string& regionName() const{return name;}

	virtual bool includesPoint(const Point* const p,
			const Context& world) const;

	inline virtual const string instantiationText(const Context& world,
			bool shortForm) const{
		return value->instantiationText(world,shortForm);};

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out, const std::string& line_header ) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual void substituteNamedRegions(const Context& world) const;


	inline virtual
	const PointSimple* getCentralPoint(const Context& world) const
	{return value->getCentralPoint(world);}

	inline void setRegion(const Region* reg){value=reg;}

	// for Case Based Reaoning
	static const string
	getLongitudinalRegion(const PointSimple* const pos,
			const Context& world);

	static const string
	getTransversalRegion(const PointSimple* const pos,
			const Context& world);

	static const string
	getRadialRegion(const PointSimple* const pos,
			const Context& world);

	static unsigned int
	longitudinalRegionDistance(const std::string& reg1,const std::string& reg2);

	static unsigned int
	transverseRegionDistance(const std::string& reg1,const std::string& reg2);

	static unsigned int
	radialRegionDistance(const std::string& reg1,const std::string& reg2);

private:
	string name;
	mutable const Region* value;

	static unsigned int longitudinalRegionIndex(const std::string& reg);
	static unsigned int transverseRegionIndex(const std::string& reg);
	static unsigned int radialRegionIndex(const std::string& reg);
};




class RegArc : public InstantiatedRegion
{//Angle should be in range -180..180, angle span 0..360
public:

	RegArc( Point* center,
			Decimal* start_rad,
			Decimal* end_rad,
			Decimal* start_ang,
			Decimal* span_ang );
	RegArc( Point* center,
			Decimal* start_rad,
			Decimal* end_rad);
	RegArc( Point* center, // for circles...
			Decimal* rad);

	inline virtual ~RegArc(){};

	virtual bool includesPoint(const Point* const p,
			const Context& world) const;

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*> &parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	inline const Point* getCenter() const {return m_center;}

	inline const Decimal* getStartRad() const{return M_start_rad;}

	inline const Decimal* getEndRad() const{return M_end_rad;}

	inline const Decimal* getStartAng() const{return M_start_ang;}

	inline const Decimal* getSpanAng() const{return M_span_ang;}

	bool setRad( Decimal* start_rad, Decimal* end_rad );

	inline void setStartRad(Decimal* start_rad){M_start_rad=start_rad;};

	inline void setEndRad(Decimal* end_rad){M_end_rad=end_rad;};

	bool setAng( Decimal* start_ang, Decimal* span_ang);

	inline void setStartAng( Decimal* start_ang){M_start_ang=start_ang;};

	inline void setAngSpan( Decimal* span_ang){M_span_ang=span_ang;};

	inline virtual void
	substituteNamedRegions(const Context& /*world*/) const{}

	virtual const PointSimple* getCentralPoint(const Context& world) const;

private:
	/* start rad <= end_rad */
	Decimal* M_start_rad;
	Decimal* M_end_rad;
	Decimal* M_start_ang;
	Decimal* M_span_ang;
	Point* m_center;

};

class RegTri : public InstantiatedRegion
{
public:

	inline RegTri(){for(unsigned int i =0; i!=3;i++)m_points[i]=NULL;}

	inline RegTri(Point* pt0,Point* pt1,Point* pt2){
		m_points[0]=pt0;m_points[1]=pt1;m_points[2]=pt2;}

	inline virtual ~RegTri(){}

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	void print( std::ostream& out ) const;

	void
	printPretty( std::ostream& out, const std::string& line_header ) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual bool includesPoint(const Point* const p,
			const Context& world) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/) const{};

	virtual const PointSimple* getCentralPoint(const Context& world) const;


	inline const Point*
	getPt( unsigned int i ) const
	{if( i < 3 )return m_points[ i ];else return NULL;}

	void setAllPts(Point* pt0,Point* pt1,Point* pt2);

private:
	Point*  m_points[ 3 ];
};


class RegRec : public InstantiatedRegion{
public:

	RegRec( Point* pt0, Point* pt1 );

	inline virtual ~RegRec(){};

	void print( std::ostream& out ) const;

	void printPretty( std::ostream& out,
			const std::string& line_header ) const;

	virtual bool includesPoint(const Point* const p,
			const Context& world) const;

	virtual const string instantiationText(const Context& world,
			bool shortForm) const;

	static Region* parse(const string& in, string& out,
			const vector<SetplayParameter*>& parameters,
			const vector<PlayerReference*>& players);

	virtual Region* deepCopy(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	virtual Region* inversion(const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players) const;

	const Point* getPt( unsigned int& i ) const;

	void setAllPts( Point* pt0,
			Point* pt1 );

	virtual const PointSimple* getCentralPoint(const Context& world) const;

	inline virtual void
	substituteNamedRegions(const Context& /*world*/) const{};
private:
	Point* m_points[ 2 ];
};

}
}

#endif

