// -*-c++-*-

#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include <setplay/clangaction.h>
#include <setplay/playerReference.h>


namespace fcportugal{
  namespace setplay{
    class Directive{
    public: 
      Directive(const vector<const PlayerReference*>& players,vector<Action*> *actions);
      virtual ~Directive(){};

      virtual void print(ostream &os) const=0;
      virtual void print2(ostream &os, unsigned int &indent) const=0;

      static Directive* parse(const string& in, string& out,
    		  const vector<SetplayParameter*>& parameters,
    		  const vector<PlayerReference*>& players);

     
      const vector<const PlayerReference*>& getPlayers() const;

      void setActions(vector<Action*> *actions);
      const vector<Action*> *getActions() const;

      void setActionsAsNotDone() const;

      virtual bool isDo() const =0;

      virtual Directive* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players_sub)const=0;

      virtual Directive* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players_sub)const=0;

      virtual void substituteNamedRegions(const Context& world);


    protected:
      const vector<const PlayerReference*> players;
      vector<Action*> *actions;
      void printAux(ostream &os) const;
      void printAux2(ostream &os, unsigned int &indent) const;
    };

    class Do:public Directive{
    public: 
      Do(const vector<const PlayerReference*>& players,vector<Action*> *actions);
      virtual ~Do(){};

      virtual void print(ostream &os) const;
      virtual void print2(ostream &os, unsigned int &indent) const;

      virtual bool isDo() const {return true;};

      virtual Directive* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      virtual Directive* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players_sub)const;

    };


    class Dont:public Directive{
    public: 
      Dont(const vector<const PlayerReference*>& players,vector<Action*> *actions);
      virtual ~Dont(){};

      virtual void print(ostream &os) const;
      virtual void print2(ostream &os, unsigned int &indent) const;

      virtual bool isDo() const {return false;};

      virtual Directive* deepCopy(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players) const;

      virtual Directive* inversion(const vector<SetplayParameter*>& params,
    		  const vector<PlayerReference*>& players_sub)const;

    };
  }
}
#endif 
