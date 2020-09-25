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

#include "sptestrcsoccersim.h"

#include <QFile>
//#include <QFileInfo>

#include "sptestrcsoccersimconfigdialog.h"

#include "portingClasses/spfcportugalporting.h"

QMap<SpSetPlay* , QString> SpTestRCSoccerSim::lastDebugFilePath;


SpTestRCSoccerSim::SpTestRCSoccerSim(SpSetPlay *s)
{
    spSetPlay = s;
    error = "";

}

bool SpTestRCSoccerSim::start()
{

    if(!spSetPlay->correctSetPlay())
         return false;

    SpTestRCSoccerSimConfigDialog *configDialog = new SpTestRCSoccerSimConfigDialog((QWidget *)spSetPlay->getMainWindow());
    configDialog->setModal(Qt::ApplicationModal);
    if(!configDialog->exec())
        return false;

    QDir logFilesDir(configDialog->logFilesDirectory);
    QFileInfo simulatorCommand(configDialog->simulatorExecutable);

    QDir teamDir(configDialog->teamDirectory);
    QFileInfo teamScriptPath(configDialog->teamScript);
    QFileInfo setplayFile(configDialog->teamSetPlayFile);

    teamDir.rename(setplayFile.absoluteFilePath(), setplayFile.absoluteFilePath() + "_tosave");

    SpFCPortugalPorting exportingObject(spSetPlay);
    if (! exportingObject.exportSetPlay(setplayFile.absoluteFilePath()))
    {
        error += exportingObject.getError();
        return false;
    }

    QProcess *serverProcess = new QProcess();
    serverProcess->start(simulatorCommand.absoluteFilePath());
    serverProcess->waitForStarted();

    teamProcess = new QProcess();
    teamProcess->setWorkingDirectory(teamDir.absolutePath());
    QFile::remove(logFilesDir.absolutePath() + "/" + "output.txt");
    QString outputStdoutStderrFile = logFilesDir.absolutePath() + "/" + "output.txt";

    teamProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    teamProcess->start("tcsh -c \"" + teamScriptPath.absoluteFilePath() + " | tee " + outputStdoutStderrFile + " \"" );
    teamProcess->waitForStarted();

    if(configDialog->opponentGroupActive)
    {
        QDir opponentDir(configDialog->opponentDirectory);
        // QFileInfo opponentBash(configDialog->opponentBash);
         QFileInfo opponentScriptPath(configDialog->opponentScript);

        QProcess *oponnentProcess = new QProcess();
        oponnentProcess->setWorkingDirectory(opponentDir.absolutePath());
        oponnentProcess->start(opponentScriptPath.absoluteFilePath());
        oponnentProcess->waitForStarted();

    }


    connect(serverProcess, SIGNAL( finished(int)), this, SLOT(serverfinish(int)));

    return true;
}

void SpTestRCSoccerSim::serverfinish(int i)
{
    if(i != 0)
    {
        error = " RCSSoccerServer error: ... haven't more information about...";
        emit finish(this, false);
    }

    int result = QMessageBox::question( (QWidget *) spSetPlay->getMainWindow(), "Save log files, after executing a test...",
                          "You just finish the RCSoccerServer application and now you have the possibility of saving the log game file. Do you want?",
                          QMessageBox::No | QMessageBox::Yes , QMessageBox::Yes );

    if( result == QMessageBox::Yes )
    {
        TestFinish* thr = new TestFinish(this);
        thr->start();
    }
    else
    {
        // Find last rcg log file
        QDir actualDirectory = QDir::current();
        QStringList filters;
        filters << "*.rcg" << "*.rcl";
        actualDirectory.setNameFilters(filters);
        QStringList fileslist = actualDirectory.entryList();
        foreach(QString s, fileslist)
            actualDirectory.remove(s);
        emit finish(this, true);
    }

}

void SpTestRCSoccerSim::TestFinish::run()
{

    QDir logFilesDir("./files/setPlaylogFiles");

    QDir teamDir("../../../2D/trunk/fcportugal");
    QDir teamLogFilesDir(teamDir.absolutePath() + "/Logfiles");
    QString setplayFile = "setplay.conf";

    teamDir.remove(setplayFile);
    teamDir.rename(setplayFile + "_tosave", setplayFile);

    // Next code: copying LogFiles to a directory next SPlanner exe file.

    // Find logfiles folder
    if( ! QDir(teamLogFilesDir).exists() )
    {
        test->setError("Directory " + teamLogFilesDir.absolutePath() + " doesn't exist.");
        emit test->finish(test, false);
        return;
    }


    // Find last rcg log file
    QStringList fileslist = QDir::current().entryList(QDir::Files, QDir::Time);
    QRegExp regExp("*.rcg");
    regExp.setPatternSyntax(QRegExp::Wildcard);
    QString lastDebugFileName = "";
    foreach(QString s, fileslist)
        if(regExp.exactMatch(s))
        {
            lastDebugFileName = s;
            break;
        }

    if( lastDebugFileName == "")
    {
        test->setError("Last rcg log file didn't found");
        emit test->finish(test, false);
        return ;
    }
    // if isn't created, create setplayLogFiles on folder files
    if(!logFilesDir.exists())
        QDir().mkdir(logFilesDir.dirName());

    // create folder for test rcg and fcp logs

    QString lastDebugFileNameWithoutExtension = lastDebugFileName.left(lastDebugFileName.size() -4);
    QString lastLogFilesFolderPath = logFilesDir.absolutePath() + "/" + test->spSetPlay->getName()+ "_" + lastDebugFileNameWithoutExtension ;
    if(! QDir::current().mkpath(lastLogFilesFolderPath + "/LogFiles"))
    {
        test->setError("Cannot create log file folder");
        emit test->finish(test, false);
        return ;
    }

    // copying rcg log file (and rcl to)
    QDir::current().rename(lastDebugFileName , lastLogFilesFolderPath +"/" + lastDebugFileName);
    QDir::current().rename(lastDebugFileNameWithoutExtension + ".rcl" , lastLogFilesFolderPath +"/" + lastDebugFileNameWithoutExtension + ".rcl");

    // Find last rcg log file
    QDir actualDirectory = QDir::current();
    QStringList filters;
    filters << "*.rcg" << "*.rcl";
    actualDirectory.setNameFilters(filters);
    actualDirectory.setSorting(QDir::Time);
    fileslist = actualDirectory.entryList();
    foreach(QString s, fileslist)
        actualDirectory.remove(s);

    //copying fcpLogFiles
    test->teamProcess->waitForFinished();
    fileslist = teamLogFilesDir.entryList(QDir::Files);
    foreach(QString s, fileslist)
    {
        QFile file(teamLogFilesDir.absolutePath() + "/" + s);
        int tries = 3;
        while(true)
        {
            usleep(100);
            if(file.copy(lastLogFilesFolderPath + "/LogFiles/" + s))
                break;
            tries--;
            if(!tries)
            {
                test->setError("Error copying player log files. \n\n \
                               Cannot copy file " + teamLogFilesDir.absolutePath() + "/" + s);
                emit test->finish(test, false);
                return ;
            }

        }

    }

    SpTestRCSoccerSim::lastDebugFilePath[test->spSetPlay] = lastLogFilesFolderPath +"/" + lastDebugFileName;

    emit test->finish(test, true);

}

