// -*-c++-*-

#ifndef SP_PARAMETER_H
#define SP_PARAMETER_H

#include <string>
#include <iostream>
#include <sstream>
#include <setplay/namesubstitutions.h>

using namespace std;
namespace fcportugal{
  namespace setplay{

    class PlayerReference;
    class Context;

    class SetplayParameter{
    public:
      SetplayParameter(string const name);

      inline virtual ~SetplayParameter(){};

      inline string const name() const {return name_;};

      virtual char type() const = 0;

      virtual const string instantiationText(const Context& world,
					     bool shortForm) const=0;

      virtual void print(ostream &os) const = 0;

      virtual void uninstantiate()=0;

      inline virtual bool instantiated() const{return m_instantiated;};

      static SetplayParameter* parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& parameters,
    		  const vector<PlayerReference*>& players);

     //  virtual SetplayParameter* deepCopy(vector<SetplayParameter*>* params, 
// 					 vector<PlayerReference*>* players) const =0; 

      virtual SetplayParameter* deepCopy() const =0;

      // Should not be needed: inversion is done before instantiation!
      //virtual SetplayParameter* inversion() const =0;
    protected:
      bool m_instantiated;
      string const name_;
    };


    class Decimal: public SetplayParameter{
    public:
      Decimal(string const name);
      Decimal(double const value);
      virtual ~Decimal(){};

      const string instantiationText(const Context& world,
				     bool shortForm) const;

      inline void set(const double& value_){_value=value_;m_instantiated=true;}

      virtual void uninstantiate(){m_instantiated=false;}

      inline const double& value() const {return _value;};
  
      inline virtual char  type() const{return 'd';}

      virtual void print(ostream &os) const ;

      static SetplayParameter* 
      parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& parameters);

      virtual Decimal* deepCopy(const vector<SetplayParameter*>& params,
				const vector<PlayerReference*>& players) const;

      virtual Decimal* deepCopy() const;

      //virtual Decimal* inversion() const;

      virtual Decimal* inversion(const vector<SetplayParameter*>& params,
      			 const vector<PlayerReference*>& players) const;
    private:
      double _value;
    };

    class Integer: public SetplayParameter{
    public:
      Integer(string const name);
      Integer(int const value);
      inline virtual ~Integer(){};

      const string instantiationText(const Context& world,
				     bool shortForm) const;

      inline void set(const int value_){_value=value_;m_instantiated=true;};

      virtual void uninstantiate(){m_instantiated=false;}

      inline const int& value() const {return _value;}
  
      inline virtual char type() const{return 'i';}

      virtual void print(ostream &os) const ;

      static SetplayParameter* 
      parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& parameters);

      virtual Integer* deepCopy(const vector<SetplayParameter*>& params,
				const vector<PlayerReference*>& players) const;

      virtual Integer* deepCopy() const;

      virtual Integer* inversion(const vector<SetplayParameter*>& params,
				 const vector<PlayerReference*>& players) const;

      //virtual Integer* inversion() const;
    private:
      int _value;
    };
  }
}


inline fcportugal::setplay::Decimal
operator+ ( const fcportugal::setplay::Decimal& a, 
	    const fcportugal::setplay::Decimal& b ){
  return fcportugal::setplay::Decimal(a.value() + b.value());
}

inline fcportugal::setplay::Decimal
operator- ( const fcportugal::setplay::Decimal& a, 
	    const fcportugal::setplay::Decimal& b ){
  return fcportugal::setplay::Decimal(a.value() - b.value());
}

inline fcportugal::setplay::Decimal
operator* ( const fcportugal::setplay::Decimal& a, 
	    const fcportugal::setplay::Decimal& b ){
  return fcportugal::setplay::Decimal(a.value() * b.value());
}

inline fcportugal::setplay::Decimal
operator/ ( const fcportugal::setplay::Decimal& a, 
	    const fcportugal::setplay::Decimal& b ){
  return fcportugal::setplay::Decimal(a.value() / b.value());
}

inline bool
operator== ( const fcportugal::setplay::Decimal& a, 
	     const fcportugal::setplay::Decimal& b ){
  return a.value() == b.value();
}


inline fcportugal::setplay::Integer
operator+ ( const fcportugal::setplay::Integer& a, 
	    const fcportugal::setplay::Integer& b ){
  return fcportugal::setplay::Integer(a.value() + b.value());
}

inline fcportugal::setplay::Integer
operator- ( const fcportugal::setplay::Integer& a, 
	    const fcportugal::setplay::Integer& b ){
  return fcportugal::setplay::Integer(a.value() - b.value());
}

inline fcportugal::setplay::Integer
operator* ( const fcportugal::setplay::Integer& a, 
	    const fcportugal::setplay::Integer& b ){
  return fcportugal::setplay::Integer(a.value() * b.value());
}

inline fcportugal::setplay::Integer
operator/ ( const fcportugal::setplay::Integer& a, 
	    const fcportugal::setplay::Integer& b ){
  return fcportugal::setplay::Integer(a.value() / b.value());
}

inline bool
operator== ( const fcportugal::setplay::Integer& a, 
	     const fcportugal::setplay::Integer& b ){
  return  a.value() == b.value();
}

// For sorting according to name
inline bool
nameBefore ( const fcportugal::setplay::SetplayParameter* a, 
	     const fcportugal::setplay::SetplayParameter* b ){
  return a->name()<b->name();
}
#endif
