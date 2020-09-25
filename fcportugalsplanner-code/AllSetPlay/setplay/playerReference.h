// -*-c++-*-

#ifndef PLAYERREFERENCE_H
#define PLAYERREFERENCE_H


#include <string>
#include <iostream>
#include <vector>
#include <setplay/parameter.h>
#include <setplay/object.h>

using namespace std;

namespace fcportugal{
  namespace setplay{

    class PlayerID;

    class PlayerReference: public MobileObject{
    public:
      PlayerReference(){};
      virtual ~PlayerReference(){};

      virtual bool isRole() const=0;

      virtual char type() const{return 'p';}

      virtual bool equals(const PlayerReference* other)const =0;

      virtual void print(ostream &os) const=0;

      virtual const PlayerID* value() const =0;

      virtual const string instantiationText(const Context& world,
					     bool shortForm) const=0;

      // Convert vector<PlayerReference*> to vector<const PlayerReference*>
      static vector<const PlayerReference*> toVectorConstPlayerReference(const vector<PlayerReference*>& vec);

      static Object* parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      static vector<PlayerReference*>
      parsePlayerList(const string& in,
			string& out,
			const vector<SetplayParameter*>& params,
			const vector<PlayerReference*>& players);

      virtual PlayerReference* deepCopy() const=0;

      virtual Object* 
      deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const=0;

      // Utility static functions
      
      // @brief outputs a list with all players from "our", "opp" or "any" team
      // @pre team=="our" || team=="opp" || team=="any"
      static vector<PlayerReference*>* allPlayersFrom(string team);
      
    };


    class PlayerID:public PlayerReference{
    public:
      // number 0 stands for all players
      PlayerID(string team, unsigned int number);
      ~PlayerID(){};

      const string team;
      const unsigned int number;

      void print(ostream &os) const;

      virtual const PlayerID* value() const {return this;};
  
      virtual PlayerReference* deepCopy() const {return new PlayerID(team,number);}
  
      virtual PlayerReference* inversion() const {return new PlayerID(team,number);}

      virtual const string instantiationText(const Context& world,bool shortForm) const;

      static Object* parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      virtual Object* 
      deepCopy(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new PlayerID(team,number);}

      virtual Object* 
      inversion(const vector<SetplayParameter*>& /*params*/,
    		  const vector<PlayerReference*>& /*players*/) const{
    	  return new PlayerID(team,number);}
 
      virtual bool isRole() const;

      virtual bool equals(const PlayerReference* other)const;
    };

   


    class PlayerRole: public PlayerReference{
    public:
      PlayerRole(string roleName)
	:PlayerReference(),name(roleName),value_(NULL){};
      ~PlayerRole(){};

      void print(ostream &os) const;

      virtual const PlayerID* value() const;

      virtual bool isRole() const;

      virtual bool equals(const PlayerReference* other )const;

      inline const string roleName() const  {return name;}

      virtual const string instantiationText(const Context& world,
					     bool shortForm) const;

      void set(PlayerID* player) {value_=player;}

      void uninstantiate(){value_=NULL;}

      bool instantiated() const{return value_!=NULL;}

      static Object* parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players);

      virtual Object* 
      deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      virtual Object* 
      inversion(const vector<SetplayParameter*>& params,
		 const vector<PlayerReference*>& players) const;
      
      virtual PlayerReference* deepCopy() const;
      
     
      
    private:
      string name;
      PlayerID* value_;
    };
  }
}
#endif 
