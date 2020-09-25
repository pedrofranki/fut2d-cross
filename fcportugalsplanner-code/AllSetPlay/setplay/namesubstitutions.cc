#include <cassert>

#include <setplay/namesubstitutions.h>
#include <setplay/region.h>

using namespace fcportugal::setplay;
using namespace std;


void NameSubstitutions::addSubstitution(const string  name, 
					const Region* reg){
  regions[name]=reg;
}

const Region* 
NameSubstitutions::getSubstitutionRegion(const string& name) const{
  map<const string,const Region* >::const_iterator it=
    regions.find(name);
  return (it==regions.end()?NULL:it->second);
}


void NameSubstitutions::print(std::ostream& out) const{
  out<<"# regions: "<<regions.size()<<"\n";
  for(map<const string,const Region* >::const_iterator it=
	regions.begin(); it!=regions.end(); it++){
    out<<it->first<<" ";
    if(it->second!=NULL)
      it->second->print(out);
    out<<"\n";
  }
}

NameSubstitutions* NameSubstitutions::deepCopy() const{
  map<const string,const Region* > res;

  for(map<const string,const Region* >::const_iterator it=
	regions.begin(); it!=regions.end(); it++){
    res[it->first]=(const Region*)it->second->deepCopy(*(new vector<SetplayParameter*>()),
    		*(new vector<PlayerReference*>()));
  }
  return new NameSubstitutions(res);
}
