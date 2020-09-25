// -*-c++-*-
#ifndef CBR_H
#define CBR_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <setplay/playerReference.h>
#include <setplay/parameter.h>
#include <setplay/region.h>
#include <setplay/clang/clangutil.h>
#include <setplay/namesubstitutions.h>

namespace fcportugal{
  namespace setplay{
    class Case {
    public:

      struct Evaluation{
	double evalThisGame;
	double weightThisGame;
	double evalThisTeam;
	double weightThisTeam;
	double evalOtherTeams;
	double weightOtherTeams;

	Evaluation():evalThisGame(0),weightThisGame(0),evalThisTeam(0),
		     weightThisTeam(0),evalOtherTeams(0),weightOtherTeams(0){};
      };

      Case(const std::string& name_, 
	   const std::string& transverse_ ,
	   const std::string& longitudinal_,
	   const std::string& radial_ ,
	   const rcss::clang::PlayMode& gameMode_, 
	   const std::string& adversary_, const unsigned int numTries_,
	   const unsigned int numFail_, const unsigned int numTriesThisGame_,
	   const unsigned int numFailThisGame_)
	:name(name_), transverseRegionName(transverse_),
	 longitudinalRegionName(longitudinal_),
	 radialRegionName(radial_),gameMode(gameMode_),adversary(adversary_),
	 numTries(numTries_),numFail(numFail_),
	 numTriesThisGame(numTriesThisGame_),
	 numFailThisGame(numFailThisGame_){}
      ~Case() {}
      
      //Parser
      static Case* parse(const std::string& in, std::string& out);
      
      // Output
      void write(std::ostream &os) const;
      
      // Inspectors      
      const std::string caseName() const{return name;}      
      
      // Comparison
      bool isSameCase(const Case& otherCase) const;
    
      const Evaluation evaluate(const Context& world) const;

      // Modifiers
      void addSuccess(){numTriesThisGame++;}
      void addFailure(){numTriesThisGame++;numFailThisGame++;}
      
    private:
      //Attributes
      const std::string name;
      // Region names
      const std::string transverseRegionName;
      const std::string longitudinalRegionName;
      const std::string radialRegionName;
      
      const rcss::clang::PlayMode gameMode; 
      const std::string adversary;
      const unsigned int numTries;
      const unsigned int numFail;
      unsigned int numTriesThisGame;
      unsigned int numFailThisGame;
      
      
    };
    
    class CaseBase{
    public:
      struct SetplayID{
	string name;
	unsigned int id;
	double eval;
	SetplayID(const string& name_, unsigned int id_):name(name_),id(id_),
							 eval(0){}
      };
      // Arguments:
      // ourTeam - our team name
      // fileName - file where cases should be read and written
      // writeToFileAfterExecution - if set, write to file after each 
      //   execution of a setplay
      // weightThisGame, weightThisTeam, weightOtherTeams - 
      //   wheights given to cases in this game, against this same team, 
      //   or against other teams
      // scoreIfZero - if a case has an evaluation of 0 (ie, was never run 
      //               successfully, it will surely not be chosen. When this
      //               happens, this argument will be the valid score
      CaseBase(const std::string& ourTeam,const string& fileName, 
	       bool writeToFile,
	       bool writeToFileAfterExecution, const double weightThisGame,
	       const double weightThisTeam,const double weightOtherTeams);
      inline ~CaseBase(){if(writeCasesToFile && !writeToFileAfterExecution)writeToFile();
	caseBase.empty();}
      
      //Parser
      static CaseBase* parse(const std::string& fileName,
			     const std::string& in, std::string& out);
      
      unsigned int
      selectSetplay(vector<SetplayID> possibleSetplayNums,
		    const Context& world) const;

      // Evaluates each setplay and inserts the value in the eval slot of 
      // SetplayID. Returns evaluation sum.
      double
      evalSetplays(vector<CaseBase::SetplayID>& possibleSetplays,
		   const Context& world) const;

      void registerCase(const std::string& setplayName,
			const PointSimple* const ballPos,
			const rcss::clang::PlayMode& gm,
			bool success,const Context& world);


      // Output
      void write(std::ostream &os) const;
      void writeToFile() const;

     
    private:
      //Attributes
      const std::string ourTeam;
      const std::string fileName;
      const bool writeCasesToFile;
      const bool writeToFileAfterExecution;
      const double weightThisGame;
      const double weightThisTeam;
      const double weightOtherTeams;
      std::map<std::string,std::vector<Case*> > caseBase;
    };

  }
}

std::ostream& operator<<( std::ostream & os, 
			  const fcportugal::setplay::Case::Evaluation& e );

#endif //CBR_H
