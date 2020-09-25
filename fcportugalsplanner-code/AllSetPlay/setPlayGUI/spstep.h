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

#ifndef SPSTEP_H
#define SPSTEP_H

#include <QMenu>
#include <QDebug>
#include <QPointF>
#include <QPainter>
#include <QMessageBox>
#include <QStyleOption>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>


class SpGraph;
class SpSetPlay;
class SpTransition;

class SpStep : public QGraphicsItem
{

public:
    enum types { middle, end, abort };
    SpStep(SpSetPlay *s, unsigned int idNumber, unsigned int waitTime = 0, unsigned int abortTime = 0);
    ~SpStep();

    void init();

    // Attributes
    inline unsigned int getIdNumber() const { return idNumber; }
    inline void setIdNumber(unsigned int i)  { idNumber = i; }
    inline unsigned int getType() const { return type; }

    // Time stuff
    int calculateAbortTime();
    inline unsigned int getWaitTime() const { return waitTime; }
    inline unsigned int getAbortTime() const { return abortTime; }
    inline void setWaitTime(unsigned int w) { waitTime = w; }
    inline void setAbortTime(unsigned int a) { abortTime = a; }
    inline void setAutomaticAbortTime(bool b) { automaticAbortTime = b; }
    inline bool isAutomaticAbortTime() { return automaticAbortTime; }

    // Transitions
    SpTransition * addSourceTransition(SpStep *destStep);
    bool canRemoveTransition(SpStep *dest);
    void removeTransition(SpTransition *t, bool isSource);
    void setActiveTransition(SpTransition *t);
    void transitionWasSelected(SpTransition *t);
    inline SpTransition * getActivedTransition() const { return activeTransition; }
    inline QList<SpTransition *> getSourceTransitionsList() const { return transitionSourceList; }
    inline QList<SpTransition *> getDestTransitionsList() const { return transitionDestList; }

    // Drawing stuff
    void active();
    void desactive();
    bool advance();
    void calculateForces();

private:
    // Attributes
    unsigned int idNumber;
    int type;

    // Time Stuff
    unsigned int waitTime;
    unsigned int abortTime;
    bool automaticAbortTime;

    // Others
    SpGraph* graph;
    SpTransition* activeTransition;
    void tryRemove();

    // Transitions
    QList< SpTransition *> transitionSourceList;
    QList< SpTransition *> transitionDestList;

    // Events
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    // Drawing stuff
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    bool actived;
    QPointF newPos;
    SpSetPlay* spSetPlay;
};

#endif // SPSTEP_H
