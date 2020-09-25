#include <setplay/setplay.h>
#include <setplay/util.h>
#include <setplay/setplayexception.h>
#include <setplay/clang/clangutil.h>

#include <cassert>
#include <algorithm>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/algorithm/string.hpp>



#include "simpleParsers.h"


using namespace boost;
using namespace boost::algorithm;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace fcportugal::setplay;



//--------------------------------------------------------------------------
// SETPLAY CONSTRUCTOR
//--------------------------------------------------------------------------

Setplay::Setplay(const string name_,const int setplayNumber_,
		const bool commAllowed_, const bool singleChannelComm_,
		const float messageRepeatTime_)
:name(name_),parameters(NULL),steps(NULL),abortCond(NULL),
 setplayNumber(setplayNumber_),instantiated(false),invertible(false),
 currentStep(NULL),
 currentActions(NULL),nextStep(NULL),setplayBegin(-1.0),
 stepBegin(-1.0),done(false),wasLastLeadPlayer(false),successful(false),
 commAllowed(commAllowed_),singleChannelComm(singleChannelComm_),
 messageRepeatTime(messageRepeatTime_)
{assert(messageRepeatTime_>=0);}



//--------------------------------------------------------------------------
// INSPECTORS
//--------------------------------------------------------------------------


// To consult existent parameters
SetplayParameter* Setplay::getSetplayParameterWithName(const string& name) const{
	for(unsigned int i = 0; i!= parameters->size();i++)
		if(parameters->at(i)->name()==name)
			return parameters->at(i);
	return NULL;

}


const PlayerReference* Setplay::roleWithName(const string& name) const{
	for(unsigned int i = 0; i!= players.size();i++)
		if(players.at(i)->isRole()
				&& ((PlayerRole*)players.at(i))->roleName()==name)
			return players.at(i);

	throw SetplayException("Participant not found in players:"+name);
	return NULL;
}

PlayerID* Setplay::existentPlayer(const PlayerID* p) const{
	for(unsigned int i = 0; i!= players.size(); i++)
		if(!players.at(i)->isRole())
			if(((PlayerID*)players.at(i))->team == p->team
					&& ((PlayerID*)players.at(i))->number == p->number)
				return (PlayerID*)players.at(i);

	return NULL;


}

Step* Setplay::getStep(const unsigned int& stepID) const{
	for(unsigned int i = 0; i!= steps->size(); i++)
		if(steps->at(i)->id==stepID)
			return steps->at(i);
	// If we got here, then the step was not found
	throw SetplayException("SP: (get) Step number not found in Setplay:"+stepID);
	assert (false);
	return NULL;
}

unsigned int Setplay::initialFreeParticipantsAmount() const{
	if(getStep(0)->participants==NULL) return 0;

	int amount=0;
	for(vector<Participation*>::const_iterator
			i=getStep(0)->participants->begin();
			i!=getStep(0)->participants->end();i++)
		if((*i)->participant()->isRole())
			amount++;
	return amount;
}


vector<const Participation*>* Setplay::initialParticipants() const{
	vector<const Participation*>* res= new vector<const Participation*>();

	for(unsigned int i=0; i!= getStep(0)->participants->size(); i++)
		res->push_back(getStep(0)->participants->at(i));

	return res;
}




//---------------------------------------------------------------------------
// OUTPUT METHODS
//---------------------------------------------------------------------------
void Setplay::write(ostream &os) const{

	os<< "(setplay :name "<< name<<" :id "<<setplayNumber;

	os<< " :invertible ";
	if(invertible)
		os<< "true";
	else
		os<< "false";


	// Write parameters
	if(instantiated){
		os << " :parameters (list ";
		for(unsigned int i = 0; i!= parameters->size();i++){
			parameters->at(i)->print(os);
			os << " ";
		}
		os  << ")";//(list of parameters
	}

	// Write players
	os << " :players (list ";
	for(unsigned int i = 0; i!= players.size();i++){
		players.at(i)->print(os);
		os << " ";
	}
	os  << ") ";//(list of players

	// Write abortCond
	if(abortCond){
		os<<" :abortCond ";
		abortCond->print(os);
		os<<" ";
	}

	// Write steps
	os<< ":steps (seq ";
	for(unsigned int i = 0; i!= steps->size();i++){
		steps->at(i)->print(os);
		os << " ";
	}
	os  << ")"//(list of steps

			<< ")" ;//(setplay
}

void Setplay::write2(ostream &os, unsigned int indent) const{

	os<< "(setplay :name "<< name<<" :id "<<setplayNumber;

	os<< " :invertible";
	if(invertible)
		os<< " true";
	else
		os<< " false";
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";

	// Write parameters
	if(instantiated){
		os << " :parameters (list ";
		for(unsigned int i = 0; i!= parameters->size();i++){
			parameters->at(i)->print(os);
			os << " ";
		}
		os  << ")";//(list of parameters

		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
	}

	// Write players
	os << " :players" << "\n";
	indent++;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << "(list";
	indent++;
	for(unsigned int i = 0; i!= players.size();i++){
		os << "\n";
		for(unsigned int t= 0; t < indent; t++)
			os << "\t";
		players.at(i)->print(os);
	}
	indent--;
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << ") " << "\n";
	indent--;

	// Write abortCond
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	if(abortCond)
	{
		os<<" :abortCond ";
		abortCond->print(os);

		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
	}

	// Write steps
	os << " :steps" << "\n";
	indent++;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << "(seq";
	indent++;
	for(unsigned int i = 0; i!= steps->size();i++){
		os << "\n";
		for(unsigned int t= 0; t < indent; t++)
			os << "\t";
		steps->at(i)->print2(os, indent);
	}
	indent--;
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os  << ")"; //(list of steps
	indent-=2;
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << ")" ;//(setplay
	os << "\n" << std::flush;

}

//-------------------------------------------------------------------------
// PARSE
//-------------------------------------------------------------------------
 

Setplay* Setplay::parse(const string in, string& out, 
		const bool useComm,
		const bool limitedComm,
		const float messageRepeatTime){

	// Must trim argument 'in', in order to ignore trailing spaces and new lines.
	string toBeProcessed(in);
	trim_left(toBeProcessed);
	if(toBeProcessed.empty()){
		out=toBeProcessed;
		return NULL;
	}

	string name;
	unsigned int id;


	rule<> sp_p = *space_p >> "(setplay" >> *space_p >> ":name" >>
			*space_p >>identifier_p[assign_a(name)] >>
			*space_p >>":id">> *space_p >>uint_p[assign_a(id)];





	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(toBeProcessed.c_str(),sp_p,
			nothing_p);


	if(result.hit){
		bool invertible=false;
		string rest=toBeProcessed.substr(result.length);


		rule<> inv_p = *space_p >>":invertible";
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),inv_p,nothing_p);

		if(result.hit){
			rest=rest.substr(result.length);
			inv_p = *space_p >>"true";
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),inv_p,nothing_p);
			if(result.hit){
				rest=rest.substr(result.length);
				invertible=true;
			}
			else{
				inv_p = *space_p >>"false";
				result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),inv_p,nothing_p);
				if(result.hit)
					rest=rest.substr(result.length);
				else{ //unexpected value
					throw SetplayException("Illegal value in invertible!");
					return NULL;
				}
			}
		}

		//Must ignore :version
		rule<> ver_p = *space_p >> ":version" >>
				*space_p >> *~(ch_p(' '));
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				ver_p,
				nothing_p);

		if(result.hit) rest=rest.substr(result.length);

		//Must ignore :comment
		rule<> com_p = *space_p >> ":comment" >>
				*space_p >> "(" >> *~(ch_p(')')) >>
				*space_p >> ")";
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				com_p,
				nothing_p);

		if(result.hit) rest=rest.substr(result.length);

		// Check if there are parameters, not mandatory
		rule<> param_p =  *space_p >> ":parameters"
				>> *space_p >> "(list";

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),param_p,
				nothing_p);

		vector<SetplayParameter*>* params
		= new vector<SetplayParameter*>;

		if(result.hit){
			rest=rest.substr(result.length);
			//Parse parameters
			SetplayParameter* param=
					SetplayParameter::parse(rest,rest,*(new vector<SetplayParameter*>()),
							*(new vector<PlayerReference*>()));

			while(param){
				params->push_back(param);

				//Parse a new param
				param= SetplayParameter::parse(rest, rest,*(new vector<SetplayParameter*>()),
						*(new vector<PlayerReference*>()));
			}

			//Parse final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
					*space_p >> ')',
					nothing_p);

			if(!result.hit){
				throw SetplayException("error parsing parameters!");
				return NULL;
			}
			rest=rest.substr(result.length);
		}
		// CAREFUL!
		// since parameters were in no particular order, there were some
		// problems when parsing their names: if we had sth like par and
		// par1, par would always be accepted. Therefore, we must sort the
		// vector in descending order
		// HOWEVER: the original order must be maintained for instantiation.
		// Therefore, in the end of the parse process, the original order
		// must be reset
		vector<SetplayParameter*>* params_copy
		=new vector<SetplayParameter*>(params->size());
		copy(params->begin(),params->end(),params_copy->begin());
		sort(params_copy->begin(),params_copy->end(),nameBefore);
		reverse(params_copy->begin(),params_copy->end());


		//Parse participants, mandatory
		rule<> pl_p = *space_p >> ":players" ;

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),pl_p,
				nothing_p);

		if(result.hit){
			rest=rest.substr(result.length);

			// To store the players
			vector<PlayerReference*> players
			= PlayerReference::parsePlayerList(rest,rest,*(new vector<SetplayParameter*>()),
					*(new vector<PlayerReference*>()));

			if(players.size()>0){
				// Parse abortCond, optional
				Cond* abort=NULL;

				rule<> abort_p = *space_p >> ":abortCond" ;

				result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
						abort_p,nothing_p);
				if(result.hit){
					rest=rest.substr(result.length);
					abort=Cond::parse(rest,rest,*params_copy,players);

					if(!abort){
						throw SetplayException("Failed parsing abort condition");
						out=in;
						return NULL;
					}
				}

				//Parse steps, mandatory
				rule<> step_p =  *space_p >> ":steps"
						>> *space_p >> "(seq";

				result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),step_p,
						nothing_p);

				if(result.hit){
					rest=rest.substr(result.length);

					Step* st=Step::parse(rest,rest,*params_copy,players);

					if(st){
						// To store the steps
						vector<Step*>* steps = new vector<Step*>;

						while(st){
							steps->push_back(st);

							//Parse a new step
							st=Step::parse(rest,rest,*params_copy,players);
						}

						// Check the final "))"
						rule<> final_p = *space_p >> ')' >> *space_p >> ')';

						result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),final_p,
								nothing_p);

						if(result.hit){
							out=rest.substr(result.length);
							Setplay* res =
									new Setplay(name,id,useComm,limitedComm,
											messageRepeatTime);
							res->invertible=invertible;
							res->parameters=params;
							// create a const copy of players

							res->players=PlayerReference::toVectorConstPlayerReference(players);
							res->steps=steps;
							res->abortCond=abort;
							return res;
						}
					}
				}
			}
		}
	}
	else{

		throw SetplayException( "Invalid Setplay header:->"+in+"<-");

	}
}


//-------------------------------------------------------------------------
// DEEP COPY & INVERSION
//-------------------------------------------------------------------------

Setplay* Setplay::deepCopy() const{
	vector<PlayerReference*>* players_copy
	= new vector<PlayerReference*>;

	for(unsigned int i = 0; i!= players.size(); i++)
		players_copy->push_back((PlayerReference*)players.at(i)->deepCopy());


	vector<SetplayParameter*>* parameters_copy
	= new vector<SetplayParameter*>;

	for(unsigned int i = 0; i!= parameters->size(); i++)
		parameters_copy->push_back(parameters->at(i)->deepCopy());



	Cond* abort_copy = (abortCond? abortCond->deepCopy(*parameters_copy,
			*players_copy):NULL);

	vector<Step*>* steps_copy = new vector<Step*>;

	for(unsigned int i = 0; i!= steps->size(); i++)
		steps_copy->push_back(steps->at(i)->deepCopy(*parameters_copy,
				*players_copy));


	Setplay* setPlay_copy = new Setplay(name,setplayNumber,commAllowed,
			singleChannelComm,
			messageRepeatTime);
	setPlay_copy->steps=steps_copy;
	setPlay_copy->parameters=parameters_copy;

	setPlay_copy->players=PlayerReference::toVectorConstPlayerReference(*players_copy);
	setPlay_copy->abortCond=abort_copy;

	return setPlay_copy;
}

Setplay* Setplay::inversion() const{
	assert(invertible);

	vector<PlayerReference*>* players_copy
	= new vector<PlayerReference*>;

	// No spatial references here...
	for(unsigned int i = 0; i!= players.size(); i++)
		players_copy->push_back((PlayerReference*)players.at(i)->deepCopy());


	vector<SetplayParameter*>* parameters_copy
	= new vector<SetplayParameter*>;

	// Parameters are only instantiated afterwards, therefore no inversion needed
	for(unsigned int i = 0; i!= parameters->size(); i++)
		parameters_copy->push_back(parameters->at(i)->deepCopy());



	Cond* abort_copy = (abortCond? abortCond->inversion(*parameters_copy,
			*players_copy):NULL);

	vector<Step*>* steps_copy = new vector<Step*>;

	for(unsigned int i = 0; i!= steps->size(); i++)
		steps_copy->push_back(steps->at(i)->inversion(*parameters_copy,
				*players_copy));


	// The setplay number will be it's complement, which is an intuitive way
	// of refering to the new Setplay
	// Cannot be 0!
	assert(setplayNumber!=0);
	string newName("INV_");
	Setplay* setPlay_copy = new Setplay(newName.append(name),-setplayNumber,
			commAllowed,singleChannelComm,
			messageRepeatTime);
	setPlay_copy->steps=steps_copy;
	setPlay_copy->parameters=parameters_copy;

	setPlay_copy->players=PlayerReference::toVectorConstPlayerReference(*players_copy);
	setPlay_copy->abortCond=abort_copy;

	return setPlay_copy;
}

//---------------------------------------------------------------------------
// MODIFIERS
//---------------------------------------------------------------------------
bool Setplay::instantiate(const InitMessage& message){
	//Must scan all parameters...

	return instantiate(message.arguments);
}

bool Setplay::instantiate(vector<const string*> *paramsReceived){
try
	{
	if(paramsReceived==NULL
			|| paramsReceived->size()<(parameters==NULL?0:parameters->size())){
		throw SetplayException("SP: bad amount of parameters in instantiation! Got "
				+paramsReceived->size());
		return false;
	}

	parse_info<> result;

	bool failed=false;
	//Must deal with all parameters...
	for(unsigned int i=0; i<parameters->size();i++)
		if(parameters->at(i)->type()=='d'){ //Decimal
			double d;
			result = BOOST_SPIRIT_CLASSIC_NS::parse(paramsReceived->at(i)->c_str(),
					real_p[assign_a(d)],
					nothing_p);
			if(result.hit)
				((Decimal*)parameters->at(i))->set(d);
			else
				failed=true;
		}
		else if(parameters->at(i)->type()=='i'){ //Integer
			int j;
			result = BOOST_SPIRIT_CLASSIC_NS::parse(paramsReceived->at(i)->c_str(),
					int_p[assign_a(j)],
					nothing_p);
			if(result.hit)
				((Integer*)parameters->at(i))->set(j);
			else
				failed=true;
		}
		else if(parameters->at(i)->type()=='p'){ //Point
			string rest;
			PointSimple* p
			=(PointSimple*)PointSimple::parse(*paramsReceived->at(i),rest,
					*(new vector<SetplayParameter*>()),
					*(new vector<PlayerReference*>()));
			if(p)
				((PointVar*)parameters->at(i))->set(p);
			else
				failed=true;
		}
		else if(parameters->at(i)->type()=='g'){ //Region
			string rest;
			Region* r=Region::parse(*paramsReceived->at(i),rest,*(new vector<SetplayParameter*>()),
					*(new vector<PlayerReference*>()));
			if(r)
				((RegVar*)parameters->at(i))->set(r);
			else
				failed=true;
		}

	//Deal with players..
	unsigned int dealt_params=parameters->size();
	for(unsigned int i=parameters->size();
			i<parameters->size()+players.size()
					&& dealt_params<paramsReceived->size();i++){

		if(players.at(i-parameters->size())->isRole()){
			string rest;

			PlayerID* p=(PlayerID*)PlayerID::parse(*paramsReceived->at(dealt_params++),
					rest,*(new vector<SetplayParameter*>()),*(new vector< PlayerReference*>()));
			if(p)
				((PlayerRole*)players.at(i-parameters->size()))->set(p);
			else{
				failed=true;
				break;
			}
		}
	}
	if(dealt_params<paramsReceived->size()){
		std::ostringstream o;
		o << "SP: bad or wrong number of parameters in instantiate: " << paramsReceived->size()
				<< " expected: "<< (parameters->size()+players.size());
		cerr<<"SP: Arguments dealt:"<<dealt_params<<" failed:"<<failed<<endl;
		cerr<<"SP: Args:";
		for(int i=0; i<paramsReceived->size(); i++)
			cerr<<*(paramsReceived->at(i))<<", ";
		cerr<<endl;
		throw SetplayException( o.str());

		failed=true;
	}
	//test vector size
	if(failed){
		//uninstantiate all parameters and players
		for(unsigned int i=0; i<parameters->size();i++)
			if(parameters!=NULL)
				if(parameters->at(i)!=NULL)
					parameters->at(i)->uninstantiate();
		for(unsigned int i=0; i<players.size();i++)
			if(players.at(i)!=NULL)
				if(players.at(i)->isRole())
					((PlayerRole*)players.at(i))->uninstantiate();
		return false;
	}
	else{
		instantiated=true;
		return true;
	}
	} catch(SetplayException())
	{	
		return false;
	}
}


void Setplay::substituteNamedRegions(const Context& world){

	if(abortCond!=NULL)
		abortCond->substituteNamedRegions(world);

	for(unsigned int i = 0; i!= steps->size(); i++)
		steps->at(i)->substituteNamedRegions(world);

}


//--------------------------------------------------------------------------
// RUNTIME METHODS
//--------------------------------------------------------------------------


void Setplay::start(const Context& world){
	if(!instantiated){
		throw SetplayException( "Trying to start a uninstantiated setplay");
		return;
	}


	// substitute named regions
	//!!! CAREFUL  !!!
	//this is being done by setplay manager...

	setCurrentStep(0,world);
	nextStep=NULL;
	setplayBegin=world.time();
	stepBegin=world.time();

	//Selection of actions will be done in execute method.
	currentActions=NULL;

}


vector<unsigned int>* 
Setplay::participatingPlayersDistanceFromMe(const Context& world) const{
	// TODO
	// still to be implemented... Or deleted?
	assert(false);

	return NULL;
}

vector<unsigned int>* 
Setplay::participatingPlayersDistanceFromPositions(const Context& world) const{

	// Will output player numbers starting with 1, up to max 11...

	//Jersey number of players participating
	vector<unsigned int>* participatingPlayers =new vector<unsigned int>;

	vector<const Participation*>* participations=initialParticipants();

	//Go through all players and choose closest player
	for(unsigned int i = 0; i!=players.size();i++){
		// If participation is the leadplayer, or is a playerId with this player's
		// number, then it will be taken by this player
		if(players.at(i)->isRole()
				&& ((PlayerRole*)players.at(i))->equals(steps->at(0)->leadPlayer))
			participatingPlayers->push_back(world.me().number);
		else if(! players.at(i)->isRole()){
			 //Not a role, should not be included in instantiation...
		}
		else{
			float distance=2000;//initial distance very high
			int chosenPlayer=0;
			PointSimple* pos=NULL;

			for(unsigned int j=1; j!= world.numPlayersPerTeam(); j++){
				// Will check all players, except the goalie, which can only be part
				// of a setplay when starting it
				// Must first check if player has already been chosen, or if it's me,
				// in which case it cannot be chosen
				pos=NULL;
				bool alreadyChosen=false;
				for(unsigned int k=0; k!= participatingPlayers->size();k++)
					if(j+1==participatingPlayers->at(k) || j+1==world.me().number){
						alreadyChosen=true;
						break;
					}
				if(!alreadyChosen){ //check if distance is smaller than current
					//Must first get corresponding location in participation vector
					for(unsigned int k=0; k!= participations->size();k++)
						if(players.at(i)->equals(participations->at(k)->participant()))
							if(participations->at(k)->location()!=NULL)
								pos=new PointSimple(*participations->at(k)->location()
										->getCentralPoint(world)
										->asPointSimple(world));

					// if location is not defined, this player will be chosen,
					// since there is no other criterion...
					if(pos==NULL){
						chosenPlayer=j;
						break;
					}else
						if(world.playerPos(PlayerID("our",j+1))!= NULL
								&& pos->distance(world.playerPos(PlayerID("our",j+1)))<distance){
							distance=pos->distance(world.playerPos(PlayerID("our",j+1)));
							chosenPlayer=j;
						}
				}
			}

			participatingPlayers->push_back(chosenPlayer+1);
		}
	}


	return participatingPlayers;
}

vector<unsigned int>* Setplay::
participatingPlayersDistanceFromPosAsArg(const Context& world,
		const vector<PointSimple>&
		teammatePos ) const{

	// Verify that array size is correct:
	assert(teammatePos.size() == world.numPlayersPerTeam());

	//Jersey number of players participating
	vector<unsigned int>* participatingPlayers =new vector<unsigned int>;

	vector<const Participation*>* participations=initialParticipants();

	//Go through all players and choose closest player
	for(unsigned int i = 0; i!=players.size();i++){
		// If participation is the leadplayer, or is a playerId with this player's
		// number, then it will be taken by this player
		if(players.at(i)->isRole()
				&& ((PlayerRole*)players.at(i))->equals(steps->at(0)->leadPlayer))
			participatingPlayers->push_back(world.me().number);
		else if(! players.at(i)->isRole()){
			 //Not a role, should not be included in instantiation...
		}
		else{
			float distance=2000;//initial distance very high
			int chosenPlayer=0;
			PointSimple* pos=NULL;

			for(unsigned int j=1; j!= world.numPlayersPerTeam(); j++){
				// Will check all players, except the goalie, which can only be part
				// of a setplay when starting it
				// Must first check if player has already been chosen, or if it's me,
				// in which case it cannot be chosen
				pos=NULL;
				bool alreadyChosen=false;
				for(unsigned int k=0; k!= participatingPlayers->size();k++)
					if(j+1==participatingPlayers->at(k) || j+1==world.me().number){
						alreadyChosen=true;
						break;
					}
				if(!alreadyChosen){ //check if distance is smaller than current
					//Must first get corresponding location in participation vector
					for(unsigned int k=0; k!= participations->size();k++)
						if(players.at(i)->equals(participations->at(k)->participant()))
							if(participations->at(k)->location()!=NULL)
								pos=new PointSimple(*participations->at(k)->location()
										->getCentralPoint(world)
										->asPointSimple(world));

					// if location is not defined, this player will be chosen,
					// since there is no other criterion...
					if(pos==NULL){
						chosenPlayer=j;
						break;
					}else
						if(pos->distance(&teammatePos[j])<distance){
							distance=pos->distance(&teammatePos[j]);
							chosenPlayer=j;
						}
				}
			}

			participatingPlayers->push_back(chosenPlayer+1);
		}
	}


	return participatingPlayers;

}

// Using permutations from the C++ Standard Template Library <algorithm>
vector<unsigned int>* 
Setplay::
participatingPlayersGlobalDistanceFromPosAsArg(const Context& world,
		const vector<PointSimple>&
		teammatePos ) const{

	// Verify that array size is correct:
	assert(teammatePos.size() == world.numPlayersPerTeam());


	//Jersey number of players participating

	vector<const Participation*> participations=*initialParticipants();

	// will store the participations that can be assigned according to a
	// positioning: not constrained by either
	// a PlayerID, or a lead player, but with a determined
	// positioning
	vector<const Participation*> freePositionedParticipations;

	// will store the participations that can be freely assigned: neither
	// restricted by a PlayerID, nor the lead player, nor having a determined
	// positioning
	vector<const Participation*> freeParticipations;

	// will store the number of the players allocated to fixed participations,
	// e.g lead player
	vector<unsigned int>* fixedParticipationsInstantiation
	=new vector<unsigned int>;

	// will store the number of the players allocated to free participations,
	// ie. player roles with no positioning
	vector<unsigned int>* freeParticipationsInstantiation
	=new vector<unsigned int>;

	// will store the number of the players allocated to free participations,
	// ie. player roles with a determined positioning
	vector<unsigned int>* freePositionedParticipationsInstantiation
	=NULL;

	// will cycle through the  participations vector and move to
	// freeParticipations all the free participations
	for(vector<const Participation*>::iterator i=participations.begin();
			i!=participations.end();)// done inside
		// If participation is the leadplayer, then it will be taken
		// by this player
		if((*i)->participant()->isRole()
				&&((PlayerRole*)(*i)->participant())->equals(steps->at(0)->leadPlayer)){
			fixedParticipationsInstantiation->push_back(world.me().number);
			i++;
		}
		else if(!(*i)->participant()->isRole()){
			fixedParticipationsInstantiation
			->push_back(((PlayerID*)(*i)->participant())->number);
			i++;
		}
		else if((*i)->location()==NULL){
			//it's a free participation, will move to adequate vector
			freeParticipations.push_back(*i);
			i=participations.erase(i);
		}
		else{ //it's a participation with position, will move to adequate vector
			freePositionedParticipations.push_back(*i);
			i=participations.erase(i);
		}


	vector<unsigned int>* universe=new vector<unsigned int>;

	// Populate universe of free players
	// Goalie is excluded! Only participates when lead player!

	for(unsigned int j=2; j!= world.numPlayersPerTeam()+1; j++)
		// Will check all players, except the goalie, which can only be part
		// of a setplay when starting it
		// Must first check if player has already been chosen, or if it's me,
		// in which case it cannot be chosen.
		// Will simply check if player is in fixedParticipationsInstantiation
		if(!containedInVector(j,*fixedParticipationsInstantiation)){
			universe->push_back(j);
		}


	// setup of vector with results...
	vector<PlayerNumAndDistance>*  vecFreePositionedParticipationsInstantiation
	=new vector<PlayerNumAndDistance>;

	vector<unsigned int>* ints= new vector<unsigned int>;

	PlayerNumAndDistance p(ints,0);
	vecFreePositionedParticipationsInstantiation->push_back(p);

	unsigned int numPlayers=universe->size();

	unsigned int dim=freePositionedParticipations.size();
	for(unsigned int l=0;l!=dim;l++){
		vector<PlayerNumAndDistance>* vStep=new vector<PlayerNumAndDistance>;
		for(unsigned int j=0;j!=numPlayers;j++){
			vector<unsigned int>* ints2= new vector<unsigned int>;

			ints2->push_back(universe->at(j));
			// Put calculated distance
			PlayerNumAndDistance
			p2(ints2,
					PointSimple(*freePositionedParticipations.at(l)->location()
							->getCentralPoint(world)
							->asPointSimple(world)).distance(&teammatePos[universe->at(j)-1]));

			vector<PlayerNumAndDistance>* vTemp
			=appendToVector(*vecFreePositionedParticipationsInstantiation,p2);

			// Append
			vStep->reserve( vStep->size() + vTemp->size());
			vStep->insert( vStep->end(), vTemp->begin(), vTemp->end());
		}

		delete vecFreePositionedParticipationsInstantiation;
		vecFreePositionedParticipationsInstantiation=vStep;
	}


	// Look for the smallest distance...
	// Initial sum of distances very high
	float minimalGlobalDistance=1000000;

	for(unsigned int r=0;r!=vecFreePositionedParticipationsInstantiation->size();
			r++){
		if(vecFreePositionedParticipationsInstantiation->at(r).totalDist
				<minimalGlobalDistance){

			minimalGlobalDistance
			=vecFreePositionedParticipationsInstantiation->at(r).totalDist;
			freePositionedParticipationsInstantiation
			=new vector<unsigned int>(*vecFreePositionedParticipationsInstantiation
					->at(r).playerNums);
		}
	}

	// Deal with the freeParticipations
	if(freeParticipations.size()>0){
		vector<unsigned int>* newUniverse=new vector<unsigned int>(*universe);
		for(vector<unsigned int>::iterator it=newUniverse->begin();
				it!=newUniverse->end();)//done inside
			if(containedInVector(*it,*freePositionedParticipationsInstantiation))
				it=newUniverse->erase(it);
			else
				it++;

		// there must be  available players for participations...
		assert(freeParticipations.size()<=universe->size());

		// Simply choose the available players at the top
		// Could be chosen by distance to ball...
		for(unsigned int i=0; i!=freeParticipations.size();i++)
			freeParticipationsInstantiation->push_back(universe->at(i));
	}

	// Deal with response, must put players into order

	// Concatenate the different participation and instantiation vectors...
	// fixed with FreePositioned
	participations.reserve(participations.size()
			+freePositionedParticipations.size());
	participations.insert(participations.end(),
			freePositionedParticipations.begin(),
			freePositionedParticipations.end());
	// All participations appended to fixedParticipationsInstantiation
	fixedParticipationsInstantiation
	->reserve(fixedParticipationsInstantiation->size()
			+freePositionedParticipationsInstantiation->size());
	fixedParticipationsInstantiation
	->insert(fixedParticipationsInstantiation->end(),
			freePositionedParticipationsInstantiation->begin(),
			freePositionedParticipationsInstantiation->end());


	// fixed with Free
	participations.reserve(participations.size()
			+freeParticipations.size());
	participations.insert(participations.end(),
			freeParticipations.begin(),
			freeParticipations.end());
	// All participations appended to fixedParticipationsInstantiation
	fixedParticipationsInstantiation
	->reserve(fixedParticipationsInstantiation->size()
			+freeParticipationsInstantiation->size());
	fixedParticipationsInstantiation
	->insert(fixedParticipationsInstantiation->end(),
			freeParticipationsInstantiation->begin(),
			freeParticipationsInstantiation->end());




	// Check the order of resulting vector... players may be in different order
	// as participants
	vector<unsigned int>* res=new vector<unsigned int>;
	// Will cycle players
	for(unsigned int pl=0; pl!=players.size(); pl++)
		// will look for this player in participants
		if(players.at(pl)->isRole())
			for(unsigned int pa=0; pa!=participations.size(); pa++)
				if(players.at(pl)->equals(participations.at(pa)->participant()))
					res->push_back(fixedParticipationsInstantiation->at(pa));

	return res;
}

// Original atempt. Some bug, did not work.
// It was also very memory consuming...
// vector<unsigned int>* 
// Setplay::
// participatingPlayersGlobalDistanceFromPosAsArg(const Context& world,
// 					       const vector<PointSimple>& 
// 					       teammatePos ) const{
//   // Verify that array size is correct:
//   assert(teammatePos.size() == world.numPlayersPerTeam());


//   //Jersey number of players participating

//   vector<const Participation*>* participations=initialParticipants();
//   // To store the running selection
//   vector<PlayerNumAndDistance>* runningParticipations
//     =new vector<PlayerNumAndDistance>;

//   runningParticipations->push_back(PlayerNumAndDistance());

//   // Variables to store the final selection
//   // Initial sum of distances very high
//   float minimalGlobalDistance=1000000;
//   vector<unsigned int>* chosenParticipatingPlayers=NULL;

//   //Go through all participations and choose closest player
//   for(unsigned int i = 0; i!=participations->size();i++){
//     // If participation is the leadplayer, then it will be taken by this player
//     if(participations->at(i)->participant()->isRole()
//        &&((PlayerRole*)participations->at(i)->participant())
//        ->equals(steps->at(0)->leadPlayer)){
//       PlayerNumAndDistance* p=new PlayerNumAndDistance();
//       p->playerNums->push_back(me->number);
//       p->totalDist=0;
//       //APPEND
//       vector<PlayerNumAndDistance>* temp
// 	=appendToVector(*runningParticipations,*p);
//       delete runningParticipations;
//       runningParticipations=temp;
//     }
//     else // If participation is a playerId, the corresponding player 
//       //will be used
//       if(!participations->at(i)->participant()->isRole()){
// 	PlayerNumAndDistance* p=new PlayerNumAndDistance();
// 	p->playerNums->
// 	  push_back(((PlayerID*)participations->at(i)->participant())->number);
// 	p->totalDist=0;
// 	//APPEND
// 	vector<PlayerNumAndDistance>* temp
// 	  =appendToVector(*runningParticipations,*p);
// 	delete runningParticipations;
// 	runningParticipations=temp;
//       }
//       else{

// 	vector<PlayerNumAndDistance>* newRunningParticipations
// 	  =new vector<PlayerNumAndDistance>;
// 	for(unsigned int j=1; j!= world.numPlayersPerTeam(); j++){
// 	  // Will check all players, except the goalie, which can only be part
// 	  // of a setplay when starting it
// 	  // Must first check if player has already been chosen, or if it's me,
// 	  // in which case it cannot be chosen. Must check in all possible
// 	  // running participations
// 	  if(j+1!=me->number){
// 	    for(unsigned int r=0;r!=runningParticipations->size();r++){
// 	      bool alreadyChosen=false;
// 	      for(unsigned int k=0; 
// 		  k!= runningParticipations->at(r).playerNums->size();
// 		  k++)
// 		if(j+1==runningParticipations->at(r).playerNums->at(k)){
// 		  alreadyChosen=true;
// 		  break;}
// 	      if(!alreadyChosen){
// 		PlayerNumAndDistance* p1=new PlayerNumAndDistance();
// 		p1->playerNums->push_back(j+1);
// 		p1->totalDist
// 		  =(participations->at(i)->location()==NULL?
// 		    0:PointSimple(*participations->at(i)->location()
// 				  ->getCentralPoint(world)
// 				  ->asPointSimple(world)).distance(&teammatePos[j]));
// 		//APPEND
// 		vector<PlayerNumAndDistance>* temp
// 		  =appendToVector(*runningParticipations,*p1);
// 		for(unsigned int t=0;t!=temp->size();t++)
// 		  newRunningParticipations->push_back(temp->at(t));
// 	      }
// 	    }
// 	  }
// 	}
// 	// Substitute running participants
// 	cerr<<"runningParticipations:"<<runningParticipations->size()<<" "<<newRunningParticipations->size()<<endl;
// 	delete runningParticipations;
// 	runningParticipations=newRunningParticipations;
//       }
//   }
//   // Look for best set of players 
//   for(unsigned int r=0;r!=runningParticipations->size();r++){

//     if(runningParticipations->at(r).totalDist<minimalGlobalDistance){
//       minimalGlobalDistance
// 	=runningParticipations->at(r).totalDist;
//       if(chosenParticipatingPlayers!=NULL)
// 	delete chosenParticipatingPlayers;
//       chosenParticipatingPlayers
// 	=new vector<unsigned int>(*runningParticipations->at(r).playerNums);
//     }
//   }


//   // This is no longer needed...
//   delete runningParticipations;

//   // Check the order of resulting vector... players may be in different order
//   // as participants
//   vector<unsigned int>* res=new vector<unsigned int>;
//   // Will cycle players
//   //  for(unsigned int pl=0; pl!=players->size(); pl++)
//   //     // will look for this player in participants
//   //     for(unsigned int pa=0; pa!=participations->size(); pa++)
// //       if(players->at(pl)->equals(participations->at(pa)->participant()))
// // 	res->push_back(chosenParticipatingPlayers->at(pa));

//   return res;
//}

void Setplay::setCurrentStep(const unsigned int& stepID,const Context& world){
	for(unsigned int i = 0; i!= steps->size(); i++)
		if(steps->at(i)->id==stepID){
			currentStep=steps->at(i);
			currentStep->setActionsAsNotDone();
			stepBegin=world.time();
			return;
		}
	// If we got here, then the step was not found
	throw SetplayException("SP: (set) Step number not found in Setplay:"+stepID);
}


void Setplay::markAsSuccess(){
	successful=true;
}


vector<vector<const Action*>*>* 
Setplay::possibleActions(const Context& world) const{

	// will output all the Do actions in the valid transitions.
	// Some of the members of the returned vector may be empty, when the
	// corresponding directive has no Do directives.
	// When a transition is not currently valid, the inner vector will be
	// set to NULL. This way, the outer vector has exacly the same number of
	// members as the possible transitions. This is made for consistency with
	// the chosenAction method, which receives the number of the chosen action

	vector<vector<const Action*>*>* out = new vector<vector<const Action*>*>;


	//There are two different situations: if I am the lead player and the
	// nextStep is still not defined , I will send all the possible
	// actions in the possible transitions.
	// If the nextStep is already defined, then only the actions in this
	// transition will be returned

	if(currentStep->leadPlayer->value()->number==world.me().number
			&& nextStep==NULL){

		if(currentStep->transitions!=NULL && currentStep->transitions->size()>0)
			for(unsigned int i=0;i!=currentStep->transitions->size();i++)
				if(currentStep->transitions->at(i)->getCond()==NULL
						||currentStep->transitions->at(i)->getCond()
						->eval(world))// transition is currently valid
					out->push_back(currentStep->transitions->at(i)
							->getDoActionsForPlayer(world.me()));
				else // transition is not valid
					out->push_back(NULL);
	}
	else if(nextStep!=NULL)// Will simply send the presently chosen nextStep
		// as result... Always a vector with a single element
		for(unsigned int i=0;i!=currentStep->transitions->size();i++)
			if(currentStep->transitions->at(i)->isNextStep()
					&& ((NextStep*)currentStep->transitions->at(i))->nextStepNumber
					==(int)nextStep->id)
				out->push_back(currentStep->transitions->at(i)
						->getDoActionsForPlayer(world.me()));

	// Must check if waitTime has elapsed: only then can actions be executed
	// If not, all actions must be set an not active
	if(world.time()<(stepBegin+currentStep->waitTime)){
		for(unsigned int i=0; i!= out->size(); i++)
			for(unsigned int j=0; (out->at(i)!= NULL && j!=out->at(i)->size()); j++)
				out->at(i)->at(j)->active=false;
	}
	else{
		// Apparently the active attribute was not being set as true, will force
		// it here...
		for(unsigned int i=0; i!= out->size(); i++)
			for(unsigned int j=0; (out->at(i)!= NULL && j!=out->at(i)->size()); j++)
				out->at(i)->at(j)->active=true;
	}
	return out;
}

vector<const Transition*>* 
Setplay::possibleTransitions(const Context& world) const{
	// will output all the valid transitions.
	// When a transition is not currently valid, the inner vector will be
	// set to NULL. This way, the outer vector has exacly the same number of
	// members as the possible transitions. This is made for consistency with
	// the chosenTransition method, which receives the number of the chosen action

	vector<const Transition*>* out = new vector<const Transition*>;


	// There are two different situations: if I am the lead player and the
	// nextStep is still not defined , I will send all the possible
	// transitions.
	// If the transition is already defined, then only this transition will
	// be returned

	if(currentStep->leadPlayer->value()->number==world.me().number
			&& nextStep==NULL){

		if(currentStep->transitions!=NULL && currentStep->transitions->size()>0)
			for(unsigned int i=0;i!=currentStep->transitions->size();i++)
				if(currentStep->transitions->at(i)->getCond()==NULL
						||currentStep->transitions->at(i)->getCond()
						->eval(world))// transition is currently valid
					out->push_back(currentStep->transitions->at(i));
				else // transition is not valid
					out->push_back(NULL);
	}
	else if(nextStep!=NULL) // Will simply send the presently chosen nextStep
		// as result... Always a vector with a single element
		for(unsigned int i=0;i!=currentStep->transitions->size();i++)
			if(currentStep->transitions->at(i)->isNextStep()
					&& ((NextStep*)currentStep->transitions->at(i))->nextStepNumber
					==(int)nextStep->id)
				out->push_back(currentStep->transitions->at(i));


	return out;
}


void Setplay::chosenAction(const unsigned int& chosenActionNumber,
		const Context& world){
	// Consistency checks
	if(currentStep->transitions==NULL
			|| chosenActionNumber>=currentStep->transitions->size()){
		throw SetplayException("SP: Invalid transition number: outside of range");
		return;
	}

	// Sera' remendo?
	// Se o nextStep ja' estiver escolhido, nao devia ser chamado...
	//    if(nextStep!=NULL){
	//      cerr<<"SP: chosenAction is being called, but next step was already"
	// 	 <<" chosen..."<<endl;
	//    }

	if(nextStep!=NULL && chosenActionNumber==0)
		return;

	if(currentStep->transitions->at(chosenActionNumber)->getCond()!=NULL
			&&!currentStep->transitions->at(chosenActionNumber)->getCond()
			->eval(world)){
		throw SetplayException(" Invalid transition: not currently verified");
		return;
	}
	else{
		//if(chosenActionNumber>0)
		//cout<<"SP: foi-me comunicada uma transicao:"<<chosenActionNumber<<" de:"
		//   <<currentStep->transitions->size()<<endl;

		//get the transition
		Transition* chosenTransition
		=currentStep->transitions->at(chosenActionNumber);

		//check if setplay is ended by a finish or abort
		if(!chosenTransition->isNextStep()){
			done=true;

			// register success
			if(chosenTransition->isFinish())
				successful=true;
		}
		else{// There is a next step

			if(nextStep==NULL
					||((NextStep*)chosenTransition)->nextStepNumber!= (int)nextStep->id){
				nextStep=getStep(((NextStep*)chosenTransition)
						->nextStepNumber);

				currentActions=NULL;
			}
		}
	}
}

void Setplay::chosenTransition(const unsigned int& chosenTransitionNumber,
		const Context& world){
	// Consistency checks
	if(currentStep->transitions==NULL
			|| chosenTransitionNumber>=currentStep->transitions->size()){
		throw SetplayException(" Invalid transition number: outside of range");
		return;
	}

	// If nextStep is already chosen, this should not be called...
	if(nextStep!=NULL){
		// CAUTION
		cerr<<"SP: chosenTransition is being called, but next step was already"
				<<" chosen..."<<endl;
	}

	if(currentStep->transitions->at(chosenTransitionNumber)->getCond()!=NULL
			&&!currentStep->transitions->at(chosenTransitionNumber)->getCond()->eval(world)){
		throw SetplayException("Invalid transition: not currently verified");
		return;
	}
	else{
		//if(chosenActionNumber>0)
		//cout<<"SP: foi-me comunicada uma transicao:"<<chosenActionNumber<<" de:"
		//   <<currentStep->transitions->size()<<endl;

		//get the transition
		Transition* chosenTransition
		=currentStep->transitions->at(chosenTransitionNumber);

		//check if setplay is ended by a finish or abort
		if(!chosenTransition->isNextStep()){
			cerr<<"SP: Out' ("<<world.me().number<<") of Setplay! End transition..."<<endl;
			done=true;

			// register success
			if(chosenTransition->isFinish())
				successful=true;      }
		else{// There is a next step

			if(nextStep==NULL
					||((NextStep*)chosenTransition)->nextStepNumber!= (int)nextStep->id){
				nextStep=getStep(((NextStep*)chosenTransition)
						->nextStepNumber);

				currentActions=NULL;
			}
		}
	}
}

void Setplay::updateInternalState(const Context& world){

	// If there is no currentStep, sth went wrong.
	if(currentStep==NULL){
		throw SetplayException("SP: CurrentStep is NULL in setplay execution!");
		done=true;
		return;
	}

	//Update wasLastLeadPlayer
	if(wasLastLeadPlayer && world.time()-stepBegin > messageRepeatTime){
		//cout<<"SP: passei ("<<me->number<<") wasLastLeadPlayer para false em"
		//<<currentTime<<endl;
		wasLastLeadPlayer=false;
	}

	//Check if abortCond is satisfied
	//LMOTA 2011: will be done only by the lead player, to avoid
	// different behaviours in different players. Only the lead player will
	// decide this, all other players must wait for corresponding message
	if(currentStep->leadPlayer->value()->number==world.me().number
			&& abortCond!=NULL && abortCond->eval(world)){

		done=true;
		successful=false;

		return;
	}

	//Check if step abort time has been reached
	// Will be done by all players, since it is similar to all
	if(currentStep->abortTime!=0 &&
			(world.time()>(stepBegin+currentStep->abortTime))){
		done=true;
		return;
	}

	// Step change
	// Will be done only by the leadPlayer if there is communication
	// If there is no Communication, everyone does it

	// Vector only with my reference, in order to see if am Ball Owner
	//const PlayerID* arrayMe[]={new const PlayerID("our",me->number)};


	vector<const PlayerReference*> vectMe ;
	vectMe.push_back(new const PlayerID("our",world.me().number));

	if(!commAllowed || currentStep->leadPlayer->value()->number==world.me().number
			// will also check if I am ball owner
			|| CondBallOwner(vectMe).eval(world)){
		if(nextStep!=NULL)
			//Verify if a new step has been reached
			if(waitTimeHasElapsed(world)
					&& (nextStep->condition==NULL
							||nextStep->condition->eval(world))
					&& (nextStep->leadPlayer->value()->number==world.me().number
							||currentStep->leadPlayer->value()->number==world.me().number)){
				// Change is only done if I am the current, or the next, leadPlayer...
				// This is used for situations when the coach is a fixed agent, like
				// the coach in the middle-size league, and other players should not
				// be changing steps on their own...

				bool wasLeadPlayer=currentStep->leadPlayer->value()->number==world.me().number;

				//change the settings...
				setCurrentStep(nextStep->id,world);
				nextStep=NULL;
				stepBegin=world.time();

				// Check if this new step can be trivially finished through a
				// finish/abort with satisfied conditions..
				for(unsigned int i=0; i!=currentStep->transitions->size();i++)
					if(!currentStep->transitions->at(i)->isNextStep() &&
							(currentStep->transitions->at(i)->getCond()==NULL||
									currentStep->transitions->at(i)->getCond()->eval(world))){
						done=true;


						if(currentStep->transitions->at(i)->isFinish()){
							successful=true;
						}
						else{
							successful=false;
						}
					}
				if(wasLeadPlayer
						&& currentStep->leadPlayer->value()->number!=world.me().number)
					wasLastLeadPlayer=true;
				else
					wasLastLeadPlayer=false;
			}
	}

	//Check if a nextStep exists...
	if(nextStep!=NULL){
		//Check if actions are yet to set
		if(currentActions==NULL
				||currentActions->size()==0){

			//check if there are transitions...
			if(currentStep->transitions==NULL
					||currentStep->transitions->size()==0){
				// no transitions... will exit
				done=true;
				return;
			}
			else{
				for(unsigned int i=0;
						i!=currentStep->transitions->size();i++)
					if(currentStep->transitions->at(i)->isNextStep()
							&&((NextStep*)currentStep->transitions->at(i))
							->nextStepNumber
							==(int)nextStep->id){
						currentActions=
								((NextStep*)currentStep->transitions->at(i))
								->getDoActionsForPlayer(world.me());

					}
				// Check if currentActions are enabled, and set corresponding attribute
				if(currentActions!=NULL)
					if(!waitTimeHasElapsed(world))
						for(unsigned int i=0;i!=currentActions->size();i++)
							currentActions->at(i)->active=false;
					else
						for(unsigned int i=0;i!=currentActions->size();i++)
							currentActions->at(i)->active=true;
			}
		}
	}
}

// Was never really used, must be properly tested
void Setplay::executeSetplay(const Context& world, 
		Action::Executor& exec){
	updateInternalState(world);

	//Check if there are actions to execute
	vector<vector<const fcportugal::setplay::Action* > *> * spActions=
			possibleActions(world);

	// Will only choose action if this has not been done before
	// and if I am the lead player
	if(spActions && (int)spActions->size()>0 && !isNextStepAlreadyChosen()
			&& amLeadPlayer(world)){
		// action chosen is the first which is not NULL
		int chosenActionNum=-1;
		for(unsigned int a=0; spActions!=NULL && a!=spActions->size(); a++)
			if(spActions->at(a)!=NULL){
				chosenActionNum=a;
				break;
			}
		if(chosenActionNum!=-1
				&& spActions->at(chosenActionNum)
				&& spActions->at(chosenActionNum)->size()>0)
			chosenAction(chosenActionNum,world);
	}

	currentAction(world)->getExecuted(exec);
}

// Check if the setplay is over
bool Setplay::isDone() const{
	if(!instantiated)
		throw SetplayException("Trying to see if a non instantiated plan is done!");
	return done;
}

bool  Setplay::isNextStepAlreadyChosen() const{
	return nextStep!=NULL;
}


const PlayerID* Setplay::nextStepLeadPlayer() const{
	if(!isNextStepAlreadyChosen())
		return NULL;
	return nextStep->leadPlayer->value();
}

int Setplay::currentStepNumber() const{
	if(currentStep==NULL){
		throw SetplayException("CurrentStep should not be null!");
		assert(false); //Bust
		return 0;
	}
	else
		return currentStep->id;
}

int Setplay::nextStepNumber() const{
	if(nextStep==NULL)
		return -1;
	return nextStep->id;
}

bool Setplay::isPossible(const Context& world) const{
	assert(instantiated);
	return steps->at(0)->condition->eval(world);
}

double Setplay::currentStepInitTime() const{
	assert(instantiated);
	assert(currentStep);

	return stepBegin;
}

const Point* Setplay::myCurrentPositioning(const Context& world)const{
	assert(instantiated);
	assert(currentStep);

	return currentStep->currentPositioning(&(world.me()),world);
}

const Point* Setplay::currentPositioning(unsigned int num,
		const Context& world)const{
	assert(instantiated);
	assert(currentStep);


	return currentStep->currentPositioning(new PlayerID("our",num),world);
}

const Action* Setplay::currentAction(const Context& world) const{
	assert(currentStep);

	if(currentActions!= NULL && currentActions->size()>0)
		for(unsigned int i=0; i!=currentActions->size();i++){
			const Action* a=
					currentActions->at(i)->necessaryAction(world.me(),world);


			if(a!=NULL){
				if(!waitTimeHasElapsed(world))
					a->active=false;
				else
					a->active=true;

				return a;
			}
		}

	return NULL;
}

bool Setplay::playerParticipates(const PlayerID* id) const{
	assert(instantiated);

	// At Setplay beginning, current Step will not be set. In this case, will
	// look at the player list
	if(currentStep==NULL){
		for(unsigned int i =0; i!=players.size();i++){
			if(players.at(i)->value()->team=="our"
					&&players.at(i)->value()->number==id->number)
				return true;
		}
		return false;
	}
	else{

		assert(currentStep->participants);
		for(unsigned int i =0; i!=currentStep->participants->size();i++){
			if(currentStep->participants->at(i)->participant()->value()->team=="our"
					&&currentStep->participants->at(i)->participant()->value()->number
					==id->number)
				return true;
		}
		return false;
	}
}



bool Setplay::amLeadPlayer(const Context& world) const{
	assert(instantiated);

	return currentStep!=NULL && currentStep->leadPlayer->value()->team=="our"
			&& currentStep->leadPlayer->value()->number==world.me().number;
}

const PlayerID*  Setplay::leadPlayer() const{
	assert(instantiated && currentStep!=NULL);

	return currentStep->leadPlayer->value();
}

bool Setplay::waitTimeHasElapsed(const Context& world) const{
	assert(instantiated && currentStep);

	return world.time()-stepBegin>=currentStep->waitTime;
}



// COMMUNICATION
bool Setplay::willCommunicate(const Context& world) const{

	//Use ifs to write debug messages...
	if(currentStep==NULL || !commAllowed || !playerParticipates(&(world.me())))
		// FA 22/06/2010 - Added playerParticipates(me) to avoid non participating
		// players to interfere setplays
		return false;

	//Repeat step message
	if(currentStep->id!=0 && wasLastLeadPlayer && world.time()!=0
			&& (world.time()-stepBegin)<messageRepeatTime){
		return true;
	}

	if(world.time()==0||amLeadPlayer(world)){
		return true;
	}
	if(!singleChannelComm){
		return true;
	}

	return false;
}

SetplayMessage* Setplay::messageToSend(const Context& world) const{
	//If message is done, send according message
	if(done)
		return new StepMessage(world.time(),-1,successful?1:-1);

	if((currentStep->id==0 && nextStep==NULL)// Init Message...
			|| (world.time()-setplayBegin)<messageRepeatTime){
		// To assure repetition of message...
		vector<const string*>* args=new vector<const string*>;

		//insert parameters in instantiation vector
		for(unsigned int i=0; i!= parameters->size();i++)
			args->push_back(new string(parameters->at(i)->instantiationText(world,
					true)));

		//insert players in instantiation vector
		for(unsigned int i=0; i!= players.size();i++)
			if(players.at(i)->isRole())
				args->push_back(new string(players.at(i)->instantiationText(world,
						true)));

		return new InitMessage(stepBegin,setplayNumber,args);
	}
	else
		return new StepMessage(stepBegin,currentStep->id,
				(nextStep!=NULL?(int)nextStep->id:-1));
}

void Setplay::processReceivedMessage(const StepMessage& m,
		const Context& world){

	// LMOTA : inactivated this! the lead player was not moving
	// to next step when it received a message from the next lead player
	// and was later aborting, due to abortTime...
	// When I am the lead player, I will ignore all messages.
	//if(currentStep->leadPlayer->value()->number==me->number)
	//return;

	// Check if message is announcing the ending of Setplay: currentStep==-1
	if(m.currentStepNumber==-1){
		done=true;
		// Check if it was successful
		if(m.nextStepNumber==1)
			successful=true;
		return;
	}

	// Check if message contains already known information and can therefore
	// be discarded
	if((int)currentStep->id==m.currentStepNumber &&
			((nextStep==NULL && m.nextStepNumber==-1)
					||(nextStep!=NULL && (int)nextStep->id==m.nextStepNumber))){
		//cerr<<"SP:"<<me->number<<" discarding known info"<<endl;
		return;
	}

	// CAUTION think about this
	// Is it relevant if Message is from leadPlayer or not?
	// As far as I can see, even if the message comes from another player, the
	// decision must have come from the lead player. Therefore, the agent will
	// abide to it, if it is equal to the current nextStep, or the
	// current nexrStep is null
	if((int)currentStep->id!=m.currentStepNumber){
		if(nextStep == NULL ||
				(int)nextStep->id==m.currentStepNumber){//Change in current step

			setCurrentStep(m.currentStepNumber,world);
			nextStep=NULL;
			if(currentActions!=NULL)
				delete currentActions;
			currentActions=NULL;
		}
		else{// Stray message
			return;
		}
	}
	if(m.nextStepNumber!=-1
			&& (nextStep==NULL || (int)nextStep->id!=m.nextStepNumber)){
		nextStep=getStep(m.nextStepNumber);
		if(currentActions!=NULL)
			delete currentActions;
		currentActions=NULL;
	}
}


//--------------------------------------------------------------------------
// STEP CLASS
//--------------------------------------------------------------------------


Step::Step(int const id_, double const waitTime_, double const abortTime_)
:id(id_),waitTime(waitTime_),abortTime(abortTime_),transitions(NULL),
 leadPlayer(NULL),condition(NULL),participants(NULL){
}

void Step::print(ostream &os) const{
	os<<"(step :id " << id
			<<" :waitTime " << waitTime
			<<" :abortTime " << abortTime
			<<" :participants (list ";
	for(unsigned int i=0; i!= participants->size();i++){
		participants->at(i)->print(os);
		os<<" ";
	}
	os<<") ";// fim participants

	//condition
	if(condition!=NULL){
		os<<":condition ";
		condition->print(os);
	}

	//leadPlayer
	if(leadPlayer!=NULL){
		os<<" :leadPlayer ";
		leadPlayer->print(os);
	}

	//transitions
	if(transitions!=NULL){
		os<<" :transitions (list ";
		for(unsigned int i=0; i!= transitions->size();i++){
			transitions->at(i)->print(os);
			os<<" ";
		}
		os<<"))";
	}
}

void Step::print2(ostream &os, unsigned int& indent) const
{
	os<<"(step :id " << id
			<<" :waitTime " << waitTime
			<<" :abortTime " << abortTime;

	os << "\n";
	indent++;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";

	os <<" :participants" << "\n";
	indent++;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << "(list";
	indent++;
	for(unsigned int i=0; i!= participants->size();i++){
		os << "\n";
		for(unsigned int t= 0; t < indent; t++)
			os << "\t";
		participants->at(i)->print(os);
	}
	os << "\n";
	indent--;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << ")" << "\n"; // end participants
	indent--;

	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	//condition
	if(condition!=NULL)
	{
		os<<" :condition ";
		condition->print(os);
		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
	}

	//leadPlayer
	if(leadPlayer!=NULL){
		os<<" :leadPlayer ";
		leadPlayer->print(os);
		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
	}

	//transitions
	if(transitions!=NULL){
		os<< " :transitions" << "\n";
		indent++;
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
		os << "(list ";
		indent++;
		for(unsigned int i=0; i!= transitions->size();i++){
			os << "\n";
			for(unsigned int t= 0; t < indent; t++)
				os << "\t";
			transitions->at(i)->print2(os, indent);
		}
		indent--;
		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
		os<< ")" << "\n";
		indent-=2;
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
		os << ")";
	}
}

const Point* Step::currentPositioning(const PlayerReference* pid,
		const Context& world)const{

	for(unsigned int i = 0; participants && i!=participants->size();i++){

		if(participants->at(i)->participant()->equals(pid)
				&& participants->at(i)->location()!=NULL)
			return participants->at(i)->location()->getCentralPoint(world);
	}
	return NULL;
}

void Step::setActionsAsNotDone() const{
	for(unsigned int i=0; i!= transitions->size(); i++)
		transitions->at(i)->setActionsAsNotDone();
}


Step* Step::parse(const string in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	unsigned short id;

	rule<> step_p
	= *space_p >> str_p("(step") >> *space_p >> ":id" >> *space_p
	>> uint_p[assign_a(id)];


	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),step_p,
			nothing_p);

	if(result.hit){
		string rest=in.substr(result.length);

		//time attributes
		int  waitTime=0, abortTime=0;

		rule<> time_p
		=  *space_p >> str_p(":waitTime") >> *space_p
		>> uint_p[assign_a(waitTime)];

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),time_p,
				nothing_p);

		if(result.hit)
			rest=rest.substr(result.length);

		time_p
		= *space_p >> str_p(":abortTime") >> *space_p
		>> uint_p[assign_a(abortTime)] >> *space_p;

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),time_p,
				nothing_p);

		if(result.hit)
			rest=rest.substr(result.length);

		//Check for participants' labels
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				*space_p>>":participants"
				>>*space_p>>"(list",
				nothing_p);

		if(result.hit){
			//Must parse Participations. Amount is not pre-defined
			//Will parse the first one, is mandatory
			Participation* pl=
					Participation::parse(rest.substr(result.length),
							rest,parameters,players);

			if(pl){
				// To store the players
				vector<Participation*>* participants = new vector<Participation*>;

				while(pl){
					participants->push_back(pl);

					//Parse a new player
					pl= Participation::parse(rest,rest,parameters,players);
				}

				//Check ')'
				result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
						*space_p >> ')',
						nothing_p);

				if(result.hit){
					rest=rest.substr(result.length);

					Cond* cond=NULL;
					//Check :cond, not mandatory
					rule<> cond_p = *space_p >> ":condition";
					result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),cond_p,
							nothing_p);
					if(result.hit){
						rest=rest.substr(result.length);

						cond=Cond::parse(rest,rest,parameters,players);
						if(!cond){
							cerr<<"\n Missing cond in Step:"<<id <<endl;
							out=in;
							return NULL;
						}
					}

					//parse leadPlayer, mandatory
					rule<> lp_p = *space_p >> ":leadPlayer" >> *space_p ;

					result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),lp_p,
							nothing_p);
					if(result.hit){
						rest=rest.substr(result.length);

						PlayerReference* lp
						= (PlayerReference*)PlayerReference::parse(rest,rest,
								parameters,players);

						if(lp){
							//check ":trasitions"
							result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
									*space_p >> ":transitions" >>
									*space_p >> "(list",
									nothing_p);

							if(result.hit){
								vector<Transition*>* transitions = new vector<Transition*>;

								//Must parse transition
								Transition* tran=Transition::parse(rest.substr(result.length),
										rest,parameters,
										players);

								while(tran){
									transitions->push_back(tran);

									tran= Transition::parse(rest,rest,parameters,players);
								}
								//Check for the final '))'
								result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
										*space_p>>')'
										>>*space_p>>')',
										nothing_p);
								if(result.hit){
									out=rest.substr(result.length);
									Step* res =  new Step(id, waitTime,abortTime);
									res->transitions=transitions;
									res->condition=cond;
									res->leadPlayer=lp;
									res->participants=participants;

									return res;
								}
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

Step* Step::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{

	Step* step_copy= new Step(id, waitTime, abortTime);


	step_copy->transitions = new vector<Transition*>;

	for(unsigned int i = 0; i!= transitions->size(); i++)
		step_copy->transitions
		->push_back(transitions->at(i)->deepCopy(params,players));

	step_copy->participants = new vector<Participation*>;

	for(unsigned int i = 0; i!= participants->size(); i++)
		step_copy->participants
		->push_back(participants->at(i)
				->deepCopy(params,players));

	if(condition!=NULL)
		step_copy->condition = condition->deepCopy(params,players);
	else
		step_copy->condition=NULL;

	if(leadPlayer!=NULL)
		step_copy->leadPlayer=(PlayerReference*)leadPlayer->deepCopy(params,players);
	else
		step_copy->leadPlayer=NULL;

	return step_copy;
}

Step* Step::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{

	Step* step_copy= new Step(id, waitTime, abortTime);


	step_copy->transitions = new vector<Transition*>;

	for(unsigned int i = 0; i!= transitions->size(); i++)
		step_copy->transitions
		->push_back(transitions->at(i)->inversion(params,players));

	step_copy->participants = new vector<Participation*>;

	for(unsigned int i = 0; i!= participants->size(); i++)
		step_copy->participants
		->push_back(participants->at(i)
				->inversion(params,players));

	if(condition!=NULL)
		step_copy->condition = condition->inversion(params,players);
	else
		step_copy->condition=NULL;

	if(leadPlayer!=NULL)
		step_copy->leadPlayer=(PlayerReference*)leadPlayer->deepCopy(params,
				players);
	else
		step_copy->leadPlayer=NULL;

	return step_copy;
}

void Step::substituteNamedRegions(const Context& world){

	if(condition!=NULL)condition->substituteNamedRegions(world);

	for(unsigned int i = 0; i!= participants->size(); i++)
		participants->at(i)->substituteNamedRegions(world);

	for(unsigned int i = 0; i!= transitions->size(); i++)
		transitions->at(i)->substituteNamedRegions(world);
}



//---------------------------------------------------------------------------
// TRANSITION METHODS
//---------------------------------------------------------------------------
Transition* Transition::parse(const string in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	unsigned short isAbort=0, isFinish=0, isNextStep=0;

	unsigned short nextStepID=0;//Only for nextStep

	rule<> tran_p
	= *space_p >> str_p("(abort");

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),tran_p,
			nothing_p);
	string rest;
	if(result.hit){
		isAbort=1;
		rest=in.substr(result.length);
	}

	tran_p
	= *space_p >> str_p("(finish");

	result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),tran_p,
			nothing_p);

	if(result.hit){
		isFinish=1;
		rest=in.substr(result.length);
	}

	tran_p
	= *space_p >> str_p("(nextStep");

	result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),tran_p,
			nothing_p);

	if(result.hit){
		isNextStep=1;
		rest=in.substr(result.length);

		//Parse next step number
		tran_p
		= *space_p >> ":id" >> *space_p >> uint_p[assign_a(nextStepID)];

		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),tran_p,
				nothing_p);
		if(result.hit)
			rest=rest.substr(result.length);
		else{
			out=in;
			return NULL;
		}
	}

	if(isNextStep+isFinish+isAbort!=1){// only one can have been parsed
		out=in;
		return NULL;
	}

	Cond* cond=NULL;
	//Check :cond, not mandatory
	rule<> cond_p = *space_p >> ":condition";
	result =BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),cond_p,
			nothing_p);
	if(result.hit){
		rest=rest.substr(result.length);

		cond=Cond::parse(rest,rest,parameters,players);
		if(!cond){
			cerr<< "Missing cond in Transition parse\n";
			out=in;
			return NULL;
		}
	}

	// To store the directives
	vector<Directive*>* dirs = new vector<Directive*>;
	//Check for directives' labels
	result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
			*space_p>>":directives"
			>>*space_p>>"(list",
			nothing_p);

	if(result.hit){
		//Must parse directives. Amount is not pre-defined
		Directive* dir=
				Directive::parse(rest.substr(result.length),rest,parameters,players);
		while(dir){
			dirs->push_back(dir);

			//Parse a new Directive
			dir= Directive::parse(rest, rest,parameters,players);
		}

		//Check ')'
		result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
				*space_p >> ')',
				nothing_p);

		if(!result.hit){
			cerr<<"\nMissing directive list termination or invalid directives. Got:"
					<<rest<<endl;
			out=in;
			return NULL;
		}
		else
			rest=rest.substr(result.length);
	}

	//Check final ')'
	result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),
			*space_p >> ')',
			nothing_p);

	if(result.hit){
		out=rest.substr(result.length);
		if(isAbort)
			return new Abort(cond, dirs);
		if(isFinish)
			return new Finish(cond, dirs);
		if(isNextStep)
			return new NextStep(nextStepID,cond, dirs);
	}
	out=in;
	return NULL;
}

vector<const Action*>* Transition::getDoActionsForPlayer(const PlayerID& p){
	vector<const Action*>* allActions= new vector<const Action*>;
	for(unsigned int i=0; i!=directives->size();i++)
		if(directives->at(i)->isDo()){
			for(unsigned int j=0; j!=directives->at(i)->getPlayers().size();j++)
				if(directives->at(i)->getPlayers().at(j)->value()->equals(&p))
					allActions->insert(allActions->end(),
							directives->at(i)->getActions()->begin(),
							directives->at(i)->getActions()->end());
		}

	return allActions;
}

void Transition::setActionsAsNotDone() const{
	for(unsigned int i=0; i!= directives->size(); i++)
		directives->at(i)->setActionsAsNotDone();
}

void Transition::substituteNamedRegions(const Context& world){

	if(cond!=NULL)
		cond->substituteNamedRegions(world);

	if(directives!=NULL)
		for(unsigned int i = 0; i!= directives->size();i++)
			directives->at(i)->substituteNamedRegions(world);

}


void Transition::writeAux(ostream &os) const{
	if(cond!=NULL){
		os<<" :condition ";
		cond->print(os);
	}

	os<<" :directives (list";
	if(directives!=NULL)
		for(unsigned int i = 0; i!= directives->size();i++)
			directives->at(i)->print(os);
	os<<")";
};

void Transition::writeAux2(ostream &os, unsigned int &indent) const{
	if(cond!=NULL){
		os << "\n";
		for(unsigned int i= 0; i < indent; i++)
			os << "\t";
		os<<" :condition ";
		cond->print(os);
	}
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << " :directives" << "\n";
	indent++;
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os << "(list" << "\n";
	indent++;

	if(directives!=NULL)
		for(unsigned int i = 0; i!= directives->size();i++)
			directives->at(i)->print2(os, indent);
	indent--;
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os<<")";
	indent--;
};

Transition* Abort::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	Abort* res = new Abort();
	if(getCond()!=NULL)
		res->setCond(getCond()->deepCopy(params,players));
	if(getDirectives()!=NULL){
		vector<Directive*>* dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->deepCopy(params,players));

		res->setDirectives(dir_new);
	}
	return res;
};

Transition* Abort::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	Abort* res = new Abort();
	if(getCond()!=NULL)
		res->setCond(getCond()->inversion(params,players));
	if(getDirectives()!=NULL){
		vector<Directive*>* dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->inversion(params,players));

		res->setDirectives(dir_new);
	}
	return res;
};

Transition* Finish::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	Finish* res = new Finish();
	if(getCond()!=NULL)
		res->setCond(getCond()->deepCopy(params,players));
	if(getDirectives()!=NULL){
		vector<Directive*>* dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->deepCopy(params,players));

		res->setDirectives(dir_new);
	}
	return res;
};

Transition* Finish::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	Finish* res = new Finish();
	if(getCond()!=NULL)
		res->setCond(getCond()->inversion(params,players));
	if(getDirectives()!=NULL){
		vector<Directive*>* dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->inversion(params,players));

		res->setDirectives(dir_new);
	}
	return res;
};


NextStep::~NextStep(){
}

void NextStep::print(ostream &os) const{
	os<<"(nextStep :id "<<nextStepNumber;
	writeAux(os) ;
	os<<")";
}

void NextStep::print2(ostream &os, unsigned int &indent) const{
	os<<"(nextStep :id "<<nextStepNumber;
	indent++;
	writeAux2(os,indent) ;
	indent--;
	os << "\n";
	for(unsigned int i= 0; i < indent; i++)
		os << "\t";
	os<<")";
}


Transition* NextStep::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<Directive*>* dir_new=NULL;
	if(getDirectives()!=NULL){
		dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->deepCopy(params,players));
	}
	return new NextStep(nextStepNumber,
			(getCond() == NULL?NULL:
					getCond()->deepCopy(params,players)),
					dir_new);
}

Transition* NextStep::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	vector<Directive*>* dir_new=NULL;
	if(getDirectives()!=NULL){
		dir_new=new vector<Directive*>;

		for(unsigned int i = 0; i!= directives->size();i++)
			dir_new->push_back(directives->at(i)->inversion(params,players));
	}
	return new NextStep(nextStepNumber,
			(getCond() == NULL?NULL:
					getCond()->inversion(params,players)),
					dir_new);
}



Participation* Participation::parse(const string in, string& out,
		const vector<SetplayParameter*>& parameters,
		const vector<PlayerReference*>& players){
	rule<> part_p
	= *space_p >> str_p("(at");

	parse_info<> result = BOOST_SPIRIT_CLASSIC_NS::parse(in.c_str(),part_p,
			nothing_p);

	string rest;
	if(result.hit){
		// must parse player and location
		PlayerReference* pl
		=(PlayerReference*)PlayerReference::parse(in.substr(result.length),
				rest,parameters,players);

		Region* loc=Region::parse(rest,rest,parameters,players);

		if(pl && loc){
			//check final ')'
			result = BOOST_SPIRIT_CLASSIC_NS::parse(rest.c_str(),*space_p >> ')',
					nothing_p);
			if(result.hit){
				out=rest.substr(result.length);
				return new Participation(pl,loc);
			}
		}
	}
	else{
		//Check for player without location
		PlayerReference* pl
		=(PlayerReference*)PlayerReference::parse(in,rest,parameters,players);

		if(pl){
			out=rest;
			return new Participation(pl);
		}
	}
	out=in;
	return NULL;
}

void Participation::print(ostream &os) const{
	if(location_)
		os << "(at ";
	participant_->print(os);
	if(location_){
		os<<" ";
		location_->print(os);
		os<<")";
	}

}

Participation* Participation::deepCopy(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players) const{
	return new Participation((PlayerReference*)participant_->deepCopy(params,players),
			(location_?location_->deepCopy(params,players)
					:NULL));
}

Participation* Participation::inversion(const vector<SetplayParameter*>& params,
		const vector<PlayerReference*>& players)
const{
	return new Participation((PlayerReference*)participant_->deepCopy(params,players),
			(location_?location_->inversion(params,players)
					:NULL));
}



void Participation::substituteNamedRegions(const Context& world){
	if(location_)
		location_->substituteNamedRegions(world);
}



