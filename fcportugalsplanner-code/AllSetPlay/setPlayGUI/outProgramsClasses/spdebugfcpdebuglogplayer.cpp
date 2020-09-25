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

#include "spdebugfcpdebuglogplayer.h"

#include <QTextStream>
#include <QProcess>
#include "sptestrcsoccersim.h"
#include "spdebugfcpdebuglogplayerconfigdialog.h"
#include "QDebug"
QString SpDebugFCPDebugLogPlayer::lastDebugDirectory = "./files/setPlaylogFiles";

SpDebugFCPDebugLogPlayer::SpDebugFCPDebugLogPlayer(SpSetPlay * s)
{
    spSetPlay = s;
} 

bool SpDebugFCPDebugLogPlayer::start()
{
    QString lastDebugFilePath = SpTestRCSoccerSim::getLastDebugFilePath(spSetPlay);

    SpDebugFCPDebugLogPlayerConfigDialog *configDialog;
    if(lastDebugFilePath != "")
    {
        QFileInfo file(lastDebugFilePath);
        configDialog = new SpDebugFCPDebugLogPlayerConfigDialog(lastDebugFilePath, file.absolutePath() + "/LogFiles",
                                                                (QWidget *)spSetPlay->getMainWindow());
    }
    else
        configDialog = new SpDebugFCPDebugLogPlayerConfigDialog("", "", (QWidget *)spSetPlay->getMainWindow());
    configDialog->setModal(Qt::ApplicationModal);
    if(!configDialog->exec())
        return false;

    QFile fcpDebugConfFile(QDir::homePath() +"/.fcpdebugger");
    QTextStream fileStream(&fcpDebugConfFile);
    QString originalFCPDebuggerConf = "";
    if(configDialog->withPlayersLog)
    {
        //
        if(!fcpDebugConfFile.open(QFile::ReadWrite))
        {qDebug() << fcpDebugConfFile.error();
            return false;}

        while( !fileStream.atEnd() )
             originalFCPDebuggerConf += fileStream.readLine() + "\n";

        QString newConf = originalFCPDebuggerConf;
        int pos = newConf.indexOf("team_log_path=") + QString("team_log_path=").size();
        int lastpos = newConf.indexOf("\n", pos);
        newConf.remove(pos, lastpos -pos);
        newConf.insert(pos, configDialog->playersLogDirectoryPath);
        fileStream << newConf;
        fcpDebugConfFile.close();
        //
    }    

    QString debugProgram = "../../../2D/trunk/debugv1/fcpdebugguer";
    QProcess *debugProcess = new QProcess();
    debugProcess->start(debugProgram, QStringList() << configDialog->gameLogPath);

    if(configDialog->withPlayersLog)
    {
        if(!fcpDebugConfFile.open(QFile::ReadWrite))
            return true;

        fileStream << originalFCPDebuggerConf;
    }
    fcpDebugConfFile.close();

    connect(debugProcess, SIGNAL( finished(int)), this, SLOT(processfinish(int)));
    return true;

}


void SpDebugFCPDebugLogPlayer::processfinish(int i)
{
    if(i != 0)
    {
        error += "...some application error...";
        emit finish(this, false);
    }

    emit finish(this, true);
}

