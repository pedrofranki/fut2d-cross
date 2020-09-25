// -*-c++-*-

/* Changelog:
 - LMOTA: adapted from 2013 sampple_player (late 2013)
 - LMOTA: added FCPortugal Setplay support (late 2013)


 *Copyright:

 Copyright (C) Hidehisa AKIYAMA, Luis MOTA

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

#ifndef FCPORTUGAL_SETPLAY_PLAYER_H
#define FCPORTUGAL_SETPLAY_PLAYER_H

#define SETPLAY_CONF_FILE "setplay.conf"
#define CBR_CONF_FILE "cbr.conf"

#include "fcportugal_context.h"
#include "soccer_role.h"
// LMOTA this import was creating a include loop that caused an error!
// LMOTA #include "role_fcportugal_setplay.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/abstract_player_object.h>
#include <sample_player.h>
#include <vector>

#include <setplay/setplaymanager.h>

class FCPortugalSetplayPlayer
    : public SamplePlayer {


public:

    FCPortugalSetplayPlayer(bool useCBR);

    virtual
    ~FCPortugalSetplayPlayer();

    inline Context getContext(){ return context;}
    inline fcportugal::setplay::SetplayManager* getSetplayManager(){ return setplayManager;}


protected:
    // Context:
    Context context;

    // Setplay manager
    const bool useCBR;
    fcportugal::setplay::SetplayManager* setplayManager;

    SoccerRole::Ptr roleSetplay;


    //! main decision
    virtual void actionImpl();

    //communication decision.
    virtual void communicationImpl();

};

#endif
