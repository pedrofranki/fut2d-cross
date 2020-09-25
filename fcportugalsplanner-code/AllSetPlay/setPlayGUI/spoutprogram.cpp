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


#include "spoutprogram.h"

#include <QMessageBox>
#include <QList>

// ADD HERE YOUR PROGRAM CLASS HEADER PATH
#include "outProgramsClasses/spdebugfcpdebuglogplayer.h"
#include "outProgramsClasses/sptestrcsoccersim.h"

// ADD HERE YOUR PROGRAM MENU TEXTS
const QStringList SpOutProgram::menuTexts = QStringList()
        << tr("&test on rcsoccersim 2D")
        << tr("&debug on fcpdebuglogplayer") ;

// ADD HERE YOUR PROGRAM MENU SHORTCUT ... or NULL
QKeySequence SpOutProgram::shortcuts[] = {Qt::Key_F5, Qt::Key_F6};

ISpOutProgram *SpOutProgram::getISpOutProgram(QString type)
{
    // ADD HERE YOUR PROGRAM CLASS CONSTRUCTOR AS POINTER

    if(type == tr("&debug on fcpdebuglogplayer"))
        return new SpDebugFCPDebugLogPlayer(spSetPlay);
    if(type == tr("&test on rcsoccersim 2D"))
        return new SpTestRCSoccerSim(spSetPlay);
}

SpOutProgram::SpOutProgram(SpSetPlay *s)
{
    spSetPlay = s;
    barMenu = new QMenu(tr("&Run..."));

    for(int i = 0; i < SpOutProgram::menuTexts.size(); i++)
    {
        QAction *a = new QAction(SpOutProgram::menuTexts[i], spSetPlay);
        a->setStatusTip("Run a " + SpOutProgram::menuTexts[i]);
        a->setShortcut(shortcuts[i]);
        barMenu->addAction(a);
    }
}

QMenu *SpOutProgram::getMenu()
{
    return barMenu;
}


bool SpOutProgram::start(QString type)
{
    ISpOutProgram * newProg = getISpOutProgram(type);

    if(!newProg )
        return true;

    connect(newProg, SIGNAL(finish(ISpOutProgram*, bool)), this, SLOT(finish(ISpOutProgram* ,bool)));
    if(! newProg->start())
    {
        if(newProg->getError() != "")
            QMessageBox::warning((QWidget*)spSetPlay, "Start " + type + " error", newProg->getError());
        return false;
    }

    barMenu->setEnabled(false);
    barMenu->update();
    return true;
}

void SpOutProgram::finish(ISpOutProgram* prog, bool well)
{
    if(!well)
        QMessageBox::warning((QWidget*)spSetPlay, "Program finish error", prog->getError());

    barMenu->setEnabled(true);
    barMenu->update();
}
