// -*-c++-*-
#include "cbr.h"

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <fstream>
#include "simpleParsers.h"
#include "cond.h"

using namespace boost;
using namespace BOOST_SPIRIT_CLASSIC_NS;


using namespace fcportugal::setplay;
using namespace std;

Case* Case::parse(const string& in, string& out){
	string name,regTrans,regLong,regRad,mode,adversary;
	int numTries,numFail,numTriesThisGame,numFailThisGame;


	rule<> case_p
	= *space_p >> str_p("(case") >> *space_p >> ":setplayName" >> *space_p
	>> identifier_p[assign_a(name)] >> *space_p >> ":transverse"
	>> *space_p >> identifier_p[assign_a(regTrans)]
	                            >> *space_p >> ":longitudinal"
	                            >> *space_p >> identifier_p[assign_a(regLong)]
	                                                        >> *space_p >> ":radial"
	                                                        >> *space_p >> identifier_p[assign_a(regRad)]
	                                                                                    >> *space_p>> ":gameMode"
	                                                                                    >> *space_p>> identifier_p[assign_a(mode)]
	                                                                                                               >> *space_p>> ":adversary"
	                                                                                                               >> *space_p>> identifier_p[assign_a(adversary)]
	                                                                                                                                          >> *space_p>> ":numTries">>*space_p>>int_p[assign_a(numTries)]
	                                                                                                                                                                                     >> *space_p>> ":numFail">>*space_p>>int_p[assign_a(numFail)]
	                                                                                                                                                                                                                               >> *space_p>> ":numTriesThisGame"
	                                                                                                                                                                                                                               >> *space_p>> int_p[assign_a(numTriesThisGame)]
	                                                                                                                                                                                                                                                   >> *space_p>> ":numFailThisGame"
	                                                                                                                                                                                                                                                   >> *space_p>> int_p[assign_a(numFailThisGame)]
	                                                                                                                                                                                                                                                                       >> *space_p>> ")";

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),case_p,
			nothing_p);


	rcss::clang::PlayMode* gm=CondPlayMode::parsePlayMode(mode);

	if(result.hit && gm!=NULL){
		out=in.substr(result.length);
		return new Case(name,regTrans,regLong,regRad,
				*gm,adversary,
				numTries+numTriesThisGame,
				// Sum the results in the last game to the total
				numFail+numFailThisGame,0,0);
	}

	cerr<<"SP: Bad case parse!"<<endl;
	out=in;
	return NULL;
}

const Case::Evaluation Case::evaluate(const Context& world) const{  
	double weight=0;
	// When kick-off, location of ball is irrelevant, therefore areas will
	// not be compared...
	if(gameMode==rcss::clang::KickOff_Our ||gameMode==rcss::clang::KickOff_Opp
			||gameMode==rcss::clang::GoalieCatch_Our
			|| gameMode==rcss::clang::GoalieCatch_Opp)
		weight=1;
	else{
		const string  transverseCase =
				RegNamed::getTransversalRegion(world.ballPos(),world);

		const string longitudinalCase=
				RegNamed::getLongitudinalRegion(world.ballPos(),world);
		const string radialCase=
				RegNamed::getRadialRegion(world.ballPos(),world);


		// weight should have a maximum of 1 and minimum of 0, and be inversely
		// proportional to the square of the distance. Therefore: 0.5^d
		weight
		=pow(0.5,
				(int)(RegNamed::longitudinalRegionDistance(longitudinalCase,
						longitudinalRegionName)
		+RegNamed::transverseRegionDistance(transverseCase,
				transverseRegionName)
		+RegNamed::radialRegionDistance(radialCase,
				radialRegionName)));

	}

	// Determine where this case should be counted: if it is against this team,
	// then both in this game and this team. Else, just in other teams
	// General rule for evaluation: when no tries are recorded in this game,
	// 0 will be awarded. When no fails, then the Setplay has been
	// highly successful, therefore should get a high score. The standard
	// calculation cannot be used, since it would be a division by 0.
	// Therefore, a "0.5 imaginary fail" will be considered, which will make
	// the score go high as it is tried with no further fails. Else, the ratio
	// (total/fails)-0.9 will be  awarded, to ensure that cases with all fails
	// will  have a low value (0.4 - better than Setplay with all fails),
	// which can still be selected, in order to ensure (infrequent) retrial of
	// these cases.
	Evaluation eval;
	if(world.opponentName()==adversary && world.opponentName()!="unknown"){
		if(numTriesThisGame==0)
			eval.evalThisGame=0;
		else if(numFailThisGame==0)
			eval.evalThisGame=(double)numTriesThisGame/0.5;
		else
			eval.evalThisGame=((double)numTriesThisGame/numFailThisGame)-0.8;
		eval.weightThisGame=weight;

		if(numTries==0)
			eval.evalThisTeam=0;
		else if(numFail==0)
			eval.evalThisTeam=(double)numTries/0.5;
		else
			eval.evalThisTeam=((double)numTries/numFail)-0.8;
		eval.weightThisTeam=weight;

		eval.evalOtherTeams=0;
		eval.weightOtherTeams=0;
	}
	else{
		eval.evalThisGame=0;
		eval.weightThisGame=0;
		eval.evalThisTeam=0;
		eval.weightThisTeam=0;

		if(numTries==0)
			eval.evalOtherTeams=0;
		else if(numFail==0)
			eval.evalThisTeam=(double)numTries/0.5;
		else
			eval.evalThisTeam=((double)numTries/numFail)-0.8;
		eval.weightOtherTeams=weight;
	}

	return eval;
}


bool Case::isSameCase(const Case& otherCase) const{
	//cerr<<"SB: CBR game mode comparison, "<<gameMode<<"=="<<KickOff_Our<<":"
	//  <<gameMode==KickOff_Our<<endl;
	// When kick-off or goalie catch, location of ball is irrelevant, therefore
	// areas will not be compared...
	return name==otherCase.name && adversary==otherCase.adversary &&
			gameMode==otherCase.gameMode &&
			(gameMode==rcss::clang::KickOff_Our || gameMode==rcss::clang::KickOff_Opp
					||gameMode==rcss::clang::GoalieCatch_Our
					|| gameMode==rcss::clang::GoalieCatch_Opp
					||(transverseRegionName==otherCase.transverseRegionName &&
							longitudinalRegionName==otherCase.longitudinalRegionName &&
							radialRegionName==otherCase.radialRegionName)) ;
}

void Case::write(ostream &os) const{
	os <<"(case :setplayName "<< name <<" :transverse "<<transverseRegionName
			<<" :longitudinal "<<longitudinalRegionName<<" :radial "<<radialRegionName
			<<" :gameMode " <<gameMode <<" :adversary "<<adversary
			<<" :numTries " <<numTries<<" :numFail "<<numFail
			<<" :numTriesThisGame "<<numTriesThisGame<<" :numFailThisGame "
			<<numFailThisGame<<") ";
}


std::ostream& operator<<( std::ostream & os, const Case::Evaluation& e ){
	return os<<"(eval :thisGame "<<e.evalThisGame<<"*"<<e.weightThisGame
			<<" :thisTeam "<<e.evalThisTeam<<"*"<<e.weightThisTeam
			<<" :otherTeams "<<e.evalOtherTeams<<"*"<<e.weightOtherTeams<<")";
}


CaseBase::CaseBase(const std::string& ourTeam_,const string& fileName_, 
		bool writeToFile_, bool writeToFileAfterExecution_,
		const double weightThisGame_,
		const double weightThisTeam_,
		const double weightOtherTeams_)
:ourTeam(ourTeam_),fileName(fileName_),writeCasesToFile(writeToFile_),
 writeToFileAfterExecution(writeToFileAfterExecution_),
 weightThisGame(weightThisGame_),weightThisTeam(weightThisTeam_),
 weightOtherTeams(weightOtherTeams_){
	/// read conf file
	vector<string> text;
	string line;
	ifstream textstream(fileName.c_str());


	while (getline(textstream, line)) {
		text.push_back(line);
	}
	textstream.close();
	string alltext;
	for (unsigned int i=0; i < text.size(); i++){
		// substitute tabs by spaces
		size_t j = text[i].find('\t', 0);
		while(j!=string::npos){
			text[i].replace(j,1," ");
			j = text[i].find("\t", 0);
		}
		alltext += text[i];
	}


	// Check if nothing was read...
	if(alltext.length()==0)
		cerr<<"SP: empty CaseBase file!"<<endl;
	else{
		string rest;

		fcportugal::setplay::Case* caseTemp=Case::parse(alltext,rest);

		while(caseTemp){
			caseBase[caseTemp->caseName()].push_back(caseTemp);

			caseTemp=Case::parse(rest,rest);
		}


		if(rest.length()>0)
			cerr<<"\nSP ("<<rest.length()<<" chars): Remainder of CaseBase Config file:"<<rest<<endl;
	}
}

double
CaseBase::evalSetplays(vector<CaseBase::SetplayID>& possibleSetplays,
		const Context& world) const{

	double sumEvaluations=0;

	for(unsigned int i=0;i!=possibleSetplays.size();i++){
		double totalEval=0;
		map<std::string,std::vector<Case*> >::const_iterator it
		= caseBase.find(possibleSetplays.at(i).name);
		if(it==caseBase.end())// Award standard value for Setplays with
			//no registered executions
			//0.4: double as for Setplays with all fails
			totalEval=0.4*weightThisGame;
		else{
			double thisGameEvalSum=0,thisGameWeightSum=0,
					thisTeamEvalSum=0,thisTeamWeightSum=0,
					otherTeamsEvalSum=0,otherTeamsWeightSum=0;
			for(unsigned int j=0;j!=it->second.size();j++){
				Case::Evaluation thisEval
				=it->second.at(j)->evaluate(world);



				thisGameEvalSum+=thisEval.evalThisGame*thisEval.weightThisGame;
				thisGameWeightSum+=thisEval.weightThisGame;
				thisTeamEvalSum+=thisEval.evalThisTeam*thisEval.weightThisTeam;
				thisTeamWeightSum+=thisEval.weightThisTeam;
				otherTeamsEvalSum+=thisEval.evalOtherTeams*thisEval.weightOtherTeams;
				otherTeamsWeightSum+=thisEval.weightOtherTeams;
			}

			totalEval
			// In this game, the eval can be 0, since the case may refer to tries
			// with other opponents
			=(thisGameWeightSum==0?0:thisGameEvalSum/thisGameWeightSum)
			*weightThisGame
			+(thisTeamWeightSum==0?0:thisTeamEvalSum/thisTeamWeightSum)
			*weightThisTeam
			+(otherTeamsWeightSum==0?0:otherTeamsEvalSum/otherTeamsWeightSum)
			*weightOtherTeams;

		}
		possibleSetplays.at(i).eval=totalEval;
		sumEvaluations+=totalEval;
	}
	return sumEvaluations;

}

unsigned int
CaseBase::selectSetplay(vector<SetplayID> possibleSetplayNums,
		const Context& world) const{
	// computes and puts evaluation in each of the vector's members
	double sumEvaluations=evalSetplays(possibleSetplayNums,world);


	// Choose the setplay by a random number between 0 and sumEvaluations:
	// this way the evaluation scores will be the relative probability of
	// a setplay being chosen
	double randDouble = rand() * sumEvaluations / RAND_MAX;


	double evaluationAccum=0;
	for(unsigned int i=0; i!=possibleSetplayNums.size();i++)
		if((evaluationAccum+=possibleSetplayNums[i].eval)>=randDouble)
			return possibleSetplayNums[i].id;


	cerr<<"SP: CBR Bad Setplay chosen by CBR!"<<endl;
	return 10000;

}


void CaseBase::registerCase(const string& setplayName,
		const PointSimple* const ballPos,
		const rcss::clang::PlayMode& gm,
		bool success,
		const Context& world){

	// if ball position is relevant, determine it. Irrelevant for kick-offs and
	// goalie catchs...
	bool setplayStartPositionIrrelevant=(gm==rcss::clang::KickOff_Our
			|| gm==rcss::clang::KickOff_Opp
			|| gm==rcss::clang::GoalieCatch_Our
			|| gm==rcss::clang::GoalieCatch_Opp);

	// for kick ins and corner kicks, radial positions are irrelevant
	bool setplayRadialStartPositionIrrelevant=(gm==rcss::clang::KickIn_Our
				|| gm==rcss::clang::KickIn_Opp
				|| gm==rcss::clang::CornerKick_Our
				|| gm==rcss::clang::CornerKick_Opp);

	// for corner kicks, radial and transverse positions are irrelevant
	bool setplayRadialAndTransverseStartPositionIrrelevant=(gm==rcss::clang::CornerKick_Our
					|| gm==rcss::clang::CornerKick_Opp);

	PointSimple* normalizedPoint=(PointSimple*)ballPos->deepCopy(*(new vector<SetplayParameter*>()),
			*(new vector<PlayerReference*>()));
	// Normalize the point if it is outside the field, for correct classification
	if(!setplayStartPositionIrrelevant){
		if(normalizedPoint->getX()->value() > world.fieldLength()/2)
			normalizedPoint->setVec(new Decimal(world.fieldLength()/2),
					normalizedPoint->getY());
		if(normalizedPoint->getX()->value() < -world.fieldLength()/2)
			normalizedPoint->setVec(new Decimal(-world.fieldLength()/2),
					normalizedPoint->getY());
		if(normalizedPoint->getY()->value() > world.fieldWidth()/2)
			normalizedPoint->setVec(normalizedPoint->getX(),
					new Decimal(world.fieldWidth()/2));
		if(normalizedPoint->getY()->value() < -world.fieldWidth()/2)
			normalizedPoint->setVec(normalizedPoint->getX(),
					new Decimal(-world.fieldWidth()/2));
	}


	const string transverse
	=(setplayStartPositionIrrelevant || setplayRadialAndTransverseStartPositionIrrelevant?
			"irrelevant":RegNamed::getTransversalRegion(normalizedPoint,world));
	const string longitudinal
	=(setplayStartPositionIrrelevant?"irrelevant":RegNamed::getLongitudinalRegion(normalizedPoint,
			world));
	const string radial
	=(setplayStartPositionIrrelevant|| setplayRadialStartPositionIrrelevant
			|| setplayRadialAndTransverseStartPositionIrrelevant?
					"irrelevant":RegNamed::getRadialRegion(normalizedPoint,
							world));


	// Check if region names were not found...
	if(transverse==""||longitudinal==""|| radial==""){
		cerr<<"CBR: ignoring case that could not be localised!"<<endl;
		return;
	}

	Case* thisCase=new Case(setplayName,transverse,longitudinal, radial,
			gm,world.opponentName(),(unsigned int)0,
			(unsigned int)0,(unsigned int)1,
			(unsigned int)(success?0:1));

	bool insertion=false;

	// Look for similar cases
	for(unsigned int i=0; i!=caseBase[setplayName].size();i++)
		if(thisCase->isSameCase(*caseBase[setplayName][i])){
			if(success)
				caseBase[setplayName][i]->addSuccess();
			else
				caseBase[setplayName][i]->addFailure();

			insertion=true;
			break;
		}

	if(!insertion){
		caseBase[setplayName].push_back(thisCase);
	}

	if(writeCasesToFile && writeToFileAfterExecution)
		writeToFile();

}

void CaseBase::write(std::ostream &os) const{
	for(std::map<std::string,std::vector<Case*> >::const_iterator it
			= caseBase.begin(); it != caseBase.end(); ++it)
		for(unsigned int i=0;i!=it->second.size();i++){
			it->second.at(i)->write(os);
			os<<endl;
		}
}

void CaseBase::writeToFile() const{
	ofstream textstream(fileName.c_str());

	write(textstream);
}
