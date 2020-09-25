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

#ifndef SPACTION_H
#define SPACTION_H

#include <QDebug>
#include <QPointF>
#include <QPainter>
#include <QGraphicsItem>

#include "spdefinitions.h"

class SpPoint;
class SpField;
class SpPlayer;

class SpAction : public QGraphicsItem
{
public:
    enum types{ pass, shoot, dribble, hold, wait, run, passForward, goToOffsideLine};
    static const double maxBallVelocity;
    static const double maxBallDistance;
    static const double ballDelay;
    static const double maxPlayerVelocity;
    static const double playerDelay;

    SpAction(SpPlayer *a, int t);
    SpAction(SpPlayer *a, int t, SpField *f);
    ~SpAction();

    // Actors
    inline SpPlayer *getActor() const { return actor; }
    inline SpPlayer *getDestActor() const { return destActor; }
    inline void setDestPlayer(SpPlayer *player) { destActor = player; }

    //type
    inline int getType() const { return type; }
    inline void setType(int t) { type = t; }

    // Points
    void setSource(QPointF p);
    void setDest(QPointF p);
    inline void setSpDest(SpPoint *p) { destSpPoint = p; }
    inline SpPoint * getSpSource() const {return sourceSpPoint; }
    inline SpPoint * getSpDest() const {return destSpPoint; }

    // Time
    void calculateTime();
    inline int getTime() const { return time; }

    // Drawing stuff
    void actualize();
    QString print();
    QString getName();
    void updateSize(double scale);
    void updatePosOnField(double scale);

private:
    // attributes
    int type;
    SpPlayer* actor;
    SpPlayer* destActor;

    SpField* field;
    int time;

    SpPoint* sourceSpPoint;
    SpPoint* destSpPoint;

    // Drawing stuff
    double paintSize;
    double originalPaintSize;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    void drawPass(QPainter *, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2);
    void drawShoot(QPainter *, QPointF falseSourcePoint, QPointF &destArrowP1, QPointF &destArrowP2);
    void drawDribble(QPainter *, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2);
    void drawRun(QPainter *, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2);
    void drawWait(QPainter *painter);
    void drawGoToOffsideLine(QPainter *painter, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2, double angle);
};

#endif // SPACTION_H

