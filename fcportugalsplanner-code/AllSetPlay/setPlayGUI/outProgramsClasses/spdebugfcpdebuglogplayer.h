// -*-c++-*-

/* Changelog:
 - Joao Cravo, Luis Paulo Reis: Creation of tool, Version 1.3 (MsC FEUP -University of Porto-2011)
 (Detailed description of the Tool: http://hdl.handle.net/10216/62120, in portuguese)
 - Joao Fabro (joaofabro@gmail.com): Version 1.5 - Corrections and update (2014)
*/

 //Copyright:

 //Copyright (C) Joao Cravo and Luis Paulo Reis

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef SPDEBUGFCPDEBUGLOGPLAYER_H
#define SPDEBUGFCPDEBUGLOGPLAYER_H

#include "spoutprogram.h"

class SpDebugFCPDebugLogPlayer : public ISpOutProgram
{
    Q_OBJECT

public:
    SpDebugFCPDebugLogPlayer(SpSetPlay * s);
    ~SpDebugFCPDebugLogPlayer() {};
    inline QString getError() const {return error ;}
    bool start();

signals:
    void finish(ISpOutProgram * , bool);

private:
    SpSetPlay *spSetPlay;
    QString error;

    static QString lastDebugDirectory;

private slots:
    void processfinish(int i );

};

#endif // SPDEBUGFCPDEBUGLOGPLAYER_H




