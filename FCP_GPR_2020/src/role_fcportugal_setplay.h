// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef ROLE_FCPORTUGAL_SETPLAY_H
#define ROLE_FCPORTUGAL_SETPLAY_H

#include "soccer_role.h"
#include "fcportugal_setplay_player.h"

#include <setplay/clangaction.h>

class RoleFCPortugalSetplay
    : public SoccerRole, fcportugal::setplay::Action::Executor {
public:
    static const std::string NAME;

    RoleFCPortugalSetplay():player(NULL){ }

    ~RoleFCPortugalSetplay(){ }

    virtual bool execute( rcsc::PlayerAgent * agent );


    static const std::string & name(){
          return NAME;
    }

    static SoccerRole::Ptr create(){
          SoccerRole::Ptr ptr( new RoleFCPortugalSetplay() );
          return ptr;
      }

    virtual void execute( const fcportugal::setplay::ActPos& a );
    virtual void execute( const fcportugal::setplay::ActForward& a );
    virtual void execute( const fcportugal::setplay::ActPass& a );
    virtual void execute( const fcportugal::setplay::ActMark& a );
    virtual void execute( const fcportugal::setplay::ActMarkLinePlayer& a );
    virtual void execute( const fcportugal::setplay::ActMarkLineReg& a );
    virtual void execute( const fcportugal::setplay::ActDribble& a );
    virtual void execute( const fcportugal::setplay::ActOffsideLine& a );
    virtual void execute( const fcportugal::setplay::ActClear& a );
    virtual void execute( const fcportugal::setplay::ActShoot& a );
    virtual void execute( const fcportugal::setplay::ActHold& a );
    virtual void execute( const fcportugal::setplay::ActIntercept& a );
    virtual void execute( const fcportugal::setplay::ActTackle& a );
    virtual void execute( const fcportugal::setplay::ActStop& a );
    virtual void execute( const fcportugal::setplay::ActAttentionToReg& a );
    virtual void execute( const fcportugal::setplay::ActAttentionToObj& a );
    virtual void execute( const fcportugal::setplay::ActMarkGoal& a );
    virtual void execute( const fcportugal::setplay::ActReceiveBall& a );
    virtual void execute( const fcportugal::setplay::ActMoveToOffsideLine& a);

private:
	// reference to player must be kept, in order to be used in the execute methods
    FCPortugalSetplayPlayer *player;

};

#endif
