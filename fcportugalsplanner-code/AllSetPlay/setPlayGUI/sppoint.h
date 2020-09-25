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

#ifndef SPPOINT_H
#define SPPOINT_H

#include <QPointF>
#include <QString>

class SpField;
class SpPlayer;

class SpPoint
{
public:
    enum relatives { nonRelative, ballRelative, playerRelative, pointRelative };
    enum relativesPoints { oppGoalCenter, oppLeftCorner, oppRightCorner, fieldCenter };
    enum dummys { nonDummy, forcedDummy, calculatedDummy };

    // constructors
    SpPoint(SpField *f, bool fromScreen, QPointF point);
    SpPoint(SpField *f, bool fromScreen, QPointF offset, SpPlayer * player);
    SpPoint(SpField *f, bool fromScreen, QPointF offset, int relative);
    SpPoint(SpField *f, bool fromScreen, QPointF offset, QPointF rpoint);
    SpPoint(SpPoint * p);

    // Absolute point
    void setPositional();
    void changeRealPoint(QPointF rp);
    inline QPointF getRealPoint() const { return realPoint; }

    // Relative point
    inline int isRelative() const { return relative; }
    inline void setRelative(int r) { relative = r; }
    void changeOffsetPoint(QPointF op);
    QPointF getOffsetPoint() const { return offsetPoint; }
    inline void cleanRelatives() { relativePlayer = NULL; ballRelativePlayer = NULL; }

    // Relative to ball
    inline QPointF getRelativePoint() const { return relativePoint; }
    inline SpPlayer * getBallRelativePlayer() const { return ballRelativePlayer; }

    // Relative to player
    void setBallRelativePlayer();
    bool isRelativeTo(SpPlayer *p);
    bool setRelativePlayer(SpPlayer * rPlayer, SpPlayer *actorPlayer);
    inline SpPlayer * getRelativePlayer() const { return relativePlayer; }

    // Relative to point

    // Dummy points
    void calcDummy();
    void calcDummy(int stepNumber);
    inline int isDummy() { return dummy; }
    inline void setDummy() { dummy = forcedDummy; }

    // SpPoint operations
    bool isOnLimits();
    QPointF getFinalPoint(int stepNumber);
    QPointF getFinalPoint();
    SpPoint * generateFinalSpPoint(int stepNumber);
    SpPoint * generateFinalSpPoint();
    bool operator==(SpPoint sp2);
    QStringList getStrings();
    QString print();

    // Screen Point
    void changeScreenPoint(QPointF sp);
    void calcScreenPoint(QPointF point);
    inline QPointF getScreenPoint() const { return screenPoint; }
    inline void setScreenX(double x) { screenPoint.setX(x); }

    static QPointF roundPoint(SpField *field, QPointF);
    static QPointF truncate(QPointF point);

    // Drawing stuff
    inline void actualize() {   !relative ? changeRealPoint(realPoint) : changeOffsetPoint(offsetPoint);}
    inline void scale() { changeRealPoint(realPoint); }

private:
    SpField *field;

    int relative;
    int dummy;
    QPointF realPoint;
    QPointF offsetPoint;
    SpPlayer *relativePlayer;
    SpPlayer *ballRelativePlayer;
    QPointF relativePoint;

    QPointF screenPoint;
};

#endif // SPPOINT_H
