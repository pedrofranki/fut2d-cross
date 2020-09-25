// -*-c++-*-

/***************************************************************************
                                  clangutil.h  
                       Utility classes for clang messages
                             -------------------
    begin                : 25-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server 
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef _CLANGUTIL_H_
#define _CLANGUTIL_H_

#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace rcss
{
  namespace clang
  {

    enum BallMoveToken
    { 
      BMT_None,
      Pass,
      Dribble,
      Clear,
      Score,
      BMT_All 
    };

    class BallMove
    {
    public:
      //It is important that None is first and All is last
      
      static const char* TOKEN_STRINGS[];
      static const int TOKEN_MAX;

      BallMove()
        : M_entries ( 0 )
      {}

      BallMove( const BallMoveToken& init_entry )
        : M_entries ( 0 )
      { addToken( init_entry ); }

      ~BallMove()
      {}

      void
      clear()
      { M_entries = 0; }

      void
      addToken( const BallMoveToken& t );

      void
      removeToken( const BallMoveToken& t );

      bool
      isMember( const BallMoveToken& t ) const;

      std::ostream&
      print( std::ostream& out ) const;


      std::auto_ptr< BallMove >
      deepCopy() const
      { return std::auto_ptr< BallMove >( new BallMove( *this ) ); }
    private:
      unsigned M_entries;
    };

    enum PlayMode{ //LMOTA
      //Some new playmodes were introduced due to usage
      // in the midsize league, namely all the After modes...
      None,
      BeforeKickOff,
      TimeOver,
      PlayOn,
      KickOff_Our,
      KickOff_Our_After,
      KickOff_Opp,
      KickOff_Opp_After,
      KickIn_Our,
      KickIn_Our_After,
      KickIn_Opp,
      KickIn_Opp_After,
      FreeKick_Our,
      FreeKick_Our_After,
      FreeKick_Opp,
      FreeKick_Opp_After,
      Ind_FreeKick_Our,
      Ind_FreeKick_Our_After,
      Ind_FreeKick_Opp,
      Ind_FreeKick_Opp_After,
      CornerKick_Our,
      CornerKick_Our_After,
      CornerKick_Opp,
      CornerKick_Opp_After,
      GoalKick_Our,
      GoalKick_Our_After,
      GoalKick_Opp,
      GoalKick_Opp_After,
      GoalieCatch_Our,
      GoalieCatch_Opp,
      AfterGoal_Our,
      AfterGoal_Opp,
      Invalid
    };


    static const unsigned short MODE_STRINGS_LENGTH=33;

    static const char* MODE_STRINGS[] = {
      "",
      "bko",
      "time_over",
      "play_on",
      "ko_our",
      "ko_our_after",
      "ko_opp",
      "ko_opp_after",
      "ki_our",
      "ki_our_after",
      "ki_opp",
      "ki_opp_after",
      "fk_our",
      "fk_our_after",
      "fk_opp",
      "fk_opp_after",
      "ifk_our",
      "ifk_our_after",
      "ifk_opp",
      "ifk_opp_after",
      "ck_our",
      "ck_our_after",
      "ck_opp",
      "ck_opp_after",
      "gk_our",
      "gk_our_after",
      "gk_opp",
      "gk_opp_after",
      "gc_our",
      "gc_opp",
      "ag_our",
      "ag_opp",
      "invalid" };


    inline
    std::ostream&						
    operator<<( std::ostream & os, const PlayMode& s )
    { 
      return os<<MODE_STRINGS[s];
    } 

    class UNum
    {
    public:
      enum unum_t
      { uAll, u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, uMax };

    private:
      unum_t M_unum;
      
      std::string M_var;

    public:
      UNum()
        : M_unum( uMax ),
          M_var( "" )
      {}

      UNum( const unum_t& unum )
        : M_unum( unum ),
          M_var( "" )
      {}
      
      UNum( const std::string& var )
        : M_unum( uMax ),
          M_var( var )
      {}
      
      UNum( const std::string& var, const unum_t& unum )
        : M_unum( unum ),
          M_var( var )
      {}
      
      ~UNum()
      {}

      unum_t
      getUNum() const
      { return M_unum; }
    
      bool
      isValid() const
      { return M_unum != uMax; }

      bool
      isWildCard() const
      { return M_unum == uAll; }

      std::string
      getVar() const
      { return M_var; }

      bool
      isBindable() const
      { return M_var.length() > 0; }

      bool
      isBound() const
      { return isValid(); }

      bool
      bind( const unum_t& unum )
      { 
        if( isBindable() && unum != uMax && unum != uAll )
          {
            M_unum = unum;
            return true;
          }
        else
          return false;
      }


      bool
      unBind()
      { 
        if( isBindable() )
          {
            M_unum = uMax;
            return true;
          }
        else
          return false;
      }

      unum_t
      setUNum( const unum_t& unum )
      { return M_unum = unum; }

      std::string
      setVar( const std::string& var )
      { return M_var = var; }

      UNum
      operator++()
      {
        if( M_unum != uAll && M_unum != uMax )
          M_unum = ( unum_t )( ( ( unsigned int )M_unum ) + 1 );
        return *this;
      }

      UNum
      operator++( int )
      {
        UNum rval = *this;
        ++( *this );
        return rval;
      }

      UNum
      operator--()
      {
        if( M_unum == u1 )
          M_unum = uMax;
        else if( M_unum != uAll && M_unum != uMax )
          M_unum = ( unum_t )( ( ( unsigned int )M_unum ) - 1 );
        return *this;
      }

      UNum
      operator--( int )
      {
        UNum rval = *this;
        --( *this );
        return rval;
      }

        bool
        operator==( const UNum& unum ) const
        {
            return ( isValid() && unum.isValid()
                     && ( getUNum() == unum.getUNum() 
                          || isWildCard() 
                          || unum.isWildCard() ) ); 

        }
    };

      inline
      std::ostream& 
      operator<<( std::ostream & os, const BallMove& s )
      { return s.print(os); } 

      inline
      std::ostream& 
      operator<<( std::ostream & os, const BallMoveToken& s )
      { return os << rcss::clang::BallMove::TOKEN_STRINGS[ s ]; } 

// inline
// bool
// operator==( const rcss::clang::UNum& a, const rcss::clang::UNum& b )
// {
//   return ( a.isValid() && b.isValid()
//            && ( a.getUNum() == b.getUNum() 
//                 || a.isWildCard() 
//                 || b.isWildCard() ) ); 
// }

      inline
      std::ostream& 
      operator<<( std::ostream & os, const UNum& u )
      { 
	  if( u.isBindable() )
	      return os << u.getVar();
	  else
	  {
	      if( u.isValid() )
		  return os << (unsigned int)u.getUNum();
	      else
		  return os << "(null)";
	  }
      } 

      class UNumSet
    {
    private:
      typedef std::vector< UNum > container;

      container M_entries;

    public:
      UNumSet()
      {}

      ~UNumSet()
      {}

      bool
      contains( const UNum& unum ) const
      {
        if( unum.isValid() )
          {
            for( container::const_iterator i = M_entries.begin();
                 i != M_entries.end(); ++i )
              {
                if( unum == *i )
                  return true;
              }
          }
        return false;
      }

      void
      add( const UNum& unum )
      { M_entries.push_back( unum ); }

      typedef container::iterator iterator;
      typedef container::const_iterator const_iterator;

      iterator
      erase( const iterator& iter )
      { return M_entries.erase( iter ); }

      iterator
      begin()
      { return M_entries.begin(); }

      const_iterator
      begin() const
      { return M_entries.begin(); }

      iterator
      end()
      { return M_entries.end(); }

      const_iterator
      end() const
      { return M_entries.end(); }

      void
      clear()
      { M_entries.clear(); }
    };

      inline
      std::ostream& 
      operator<<( std::ostream & os, const UNumSet& u )
      {
	  os << "{";
	  for( rcss::clang::UNumSet::const_iterator i = u.begin();
	       i != u.end(); ++i )
	  {
	      if( i != u.begin() )
		  os << " ";
	      os << *i;
	  }
	  return os << "}";
      } 
  }
}

#endif
