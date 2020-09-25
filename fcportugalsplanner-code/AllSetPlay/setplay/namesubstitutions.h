// -*-c++-*-

#ifndef NAMESUBSTITUTIONS_H
#define NAMESUBSTITUTIONS_H

#include<map>
#include<vector>
#include <string>
#include <iostream>


using namespace std;

namespace fcportugal{
  namespace setplay{
    class Region;
    
    class NameSubstitutions{
    public:
      NameSubstitutions(){};
      NameSubstitutions(map<const string,const Region* > reg):regions(reg){};
      ~NameSubstitutions(){};

      NameSubstitutions* deepCopy() const;

      unsigned int numSubstitutions() const {return regions.size();};

      void addSubstitution(const string name, const Region * reg);

      const Region* getSubstitutionRegion(const string& name) const;

      void changeSubstitutionRegion(const string& name,const Region* reg)
      {regions[name]=reg;};

      void print(std::ostream& out) const;
    private:
      map<const string,const Region* > regions;
    };

  }
}
#endif
