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

#ifndef SPPORTING_H
#define SPPORTING_H

#include <QMenu>

class SpSetPlay;


class ISpPorting
{
public:

    virtual ~ISpPorting() {}
    virtual bool importSetPlay(QString filename) = 0;
    virtual bool exportSetPlay() = 0;
    virtual bool exportSetPlay(QString filename) = 0;
    virtual QString getError() const { return ""; }
};

class SpPorting : QObject
{
    Q_OBJECT
public:
    static const QStringList portingNames;
    static const QStringList portingSuffix;

    //static QKeySequence importingShortcuts[];
    //static QKeySequence exportingShortcuts[];

    SpPorting(SpSetPlay *s) { spSetPlay = s;}

    static QMenu *importingMenu(QWidget *parent);
    static QMenu *exportingMenu(QWidget *parent);
    bool importSetPlay();
    bool exportSetPlay();
    ISpPorting *getISpPorting(QString type);

private:
    SpSetPlay * spSetPlay;
    static QString lastExportDirectory;
};


#endif // SPPORTING_H
