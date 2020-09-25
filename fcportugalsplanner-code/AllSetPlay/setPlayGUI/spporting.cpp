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

#include "spporting.h"

#include <QMessageBox>
#include <QFileDialog>

#include "spsetplay.h"


// ADD HERE YOUR PORTING CLASS HEADER PATH
#include "portingClasses/spfcportugalporting.h"

// ADD HERE YOUR PORTING CLASS MENU TEXTS
const QStringList SpPorting::portingNames = QStringList()
    << tr("All Files ")
    << tr("FCPortugal SetPlay") ;

const QStringList SpPorting::portingSuffix = QStringList()
    << tr("*")
    << tr("conf") ;

// ADD HERE YOUR IMPORT/EXPORT MENU SHORTCUT ... or NULL
//QKeySequence SpPorting::importingShortcuts[] = {Qt::Key_F3};
//QKeySequence SpPorting::exportingShortcuts[] = {Qt::Key_F4};

ISpPorting *SpPorting::getISpPorting(QString extension)
{
    // ADD HERE YOUR PORTING CLASS CONSTRUCTOR AS POINTER
    if(extension == "conf" )
        return new SpFCPortugalPorting(spSetPlay);
    return NULL;
}




QString  SpPorting::lastExportDirectory = "./setPlayGUI/tests/";





bool SpPorting::importSetPlay()
{
    QString selectedFilter;
    QStringList filters;
    for(int i = 0; i < portingNames.size(); i++)
        filters <<  portingNames[i] + " (*." + portingSuffix[i] + ")";
    QString fileName = QFileDialog::getOpenFileName((QWidget*)spSetPlay->getMainWindow(),
                                 QString("Export setplay - " + spSetPlay->getName()),
                                 QString(lastExportDirectory + "/" + spSetPlay->getName()),
                                    filters.join(";;"),
                                         &selectedFilter);
    if(fileName == "")
        return false;
    QString suffix = QFileInfo(fileName).suffix();
    ISpPorting * newPort = getISpPorting(suffix);

    if(newPort != NULL)
    {
        if(! newPort->importSetPlay(fileName))
        {
            if(newPort->getError() != "")
                QMessageBox::warning((QWidget*)spSetPlay, "Importing error", newPort->getError());
            return false;
        }
        else
            return true;
    }
    else
        QMessageBox::warning((QWidget*)spSetPlay, "Importing error", "The chosen file - " + fileName + "- can't be imported by SPlanner");
    return false;

}

bool SpPorting::exportSetPlay()
{
    QString selectedFilter;

    QStringList filters;
    for(int i = 1; i < portingNames.size(); i++)
        filters <<  portingNames[i] + " (*." + portingSuffix[i] + ")";

    QString fileName = QFileDialog::getSaveFileName((QWidget*)spSetPlay->getMainWindow(),
                                 QString("Export setplay - " + spSetPlay->getName()),
                                 QString(lastExportDirectory + "/" + spSetPlay->getName()),
                                 filters.join(";;"),
                                         &selectedFilter);

    if (fileName.isNull())
        return true;

    int i = filters.indexOf(selectedFilter);

    if( QFileInfo(fileName).suffix() == "")
        fileName += "." + portingSuffix[i + 1];

    lastExportDirectory = QFileInfo(fileName).absoluteDir().absolutePath();

    ISpPorting * newPort = getISpPorting(QFileInfo(fileName).suffix() );
    if(newPort != NULL)
        return newPort->exportSetPlay(fileName);

    return true;

}
