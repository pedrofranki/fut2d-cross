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

#ifndef SPOUTPROGRAM_H
#define SPOUTPROGRAM_H

#include <QMenu>
#include "spsetplay.h"

class ISpOutProgram : public QObject
{
    Q_OBJECT
public:
    virtual ~ISpOutProgram() {};

    virtual bool start() = 0;
    virtual QString getError() const { return ""; }

signals:
    virtual void finish(ISpOutProgram *, bool );
};



class SpOutProgram : public QObject
{
    Q_OBJECT

public:
    SpOutProgram(SpSetPlay * s) ;
    QMenu *getMenu();
    bool start(QString type);
    ISpOutProgram *getISpOutProgram(QString type);

private:
    QMenu *barMenu;
    SpSetPlay * spSetPlay;

    static const QStringList menuTexts;
    static QKeySequence shortcuts[];
private slots:
    void finish(ISpOutProgram *, bool );
};

#endif // SPOUTPROGRAM_H
