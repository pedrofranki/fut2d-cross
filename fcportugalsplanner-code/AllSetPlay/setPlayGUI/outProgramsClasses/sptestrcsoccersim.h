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

#ifndef SPTESTRCSOCCERSIM_H
#define SPTESTRCSOCCERSIM_H

#include "spoutprogram.h"

#include <QProcess>
#include <QSemaphore>
#include <QThread>
class SpSetPlay;

class SpTestRCSoccerSim : public ISpOutProgram
{
    Q_OBJECT

public:
    SpTestRCSoccerSim(SpSetPlay *s);
    ~SpTestRCSoccerSim() {};
    inline QString getError() const {return error ;}
    bool start();

   static QString getLastDebugFilePath(SpSetPlay *s) { return lastDebugFilePath[s]; }
   inline void setError(QString s) { error = s; }
       SpSetPlay *spSetPlay;
          QProcess *teamProcess;
          static QMap<SpSetPlay* , QString> lastDebugFilePath;
signals:
    void finish(ISpOutProgram *, bool);

private:

    QString error;



   class TestFinish : public QThread
   {
   public:

       TestFinish(SpTestRCSoccerSim *t) {this->test = t;};
       void run();

       SpTestRCSoccerSim * test;
   };



private slots:
    void serverfinish(int i );

};

#endif // SPTESTRCSOCCERSIM_H
