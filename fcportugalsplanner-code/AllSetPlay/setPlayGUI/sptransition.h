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

#ifndef SPTRANSITION_H
#define SPTRANSITION_H

#include <QDebug>
#include <QGraphicsItem>

#include "spdefinitions.h"

class SpStep;
class SpGraph;

class SpTransition : public QGraphicsItem
{
public:
    SpTransition(SpStep * s, SpStep * d, SpGraph *g, int id);
    ~SpTransition();

    void init();

    // Source and Destination
    inline SpStep * getSource() const { return source; }
    inline SpStep * getDest() const { return dest; }
    inline unsigned int getIdNumber() const { return idNumber; }

    // Drawing stuff
    void active();
    void desactive();
    void adjust();

private:
    //Attributes
    const unsigned int idNumber;
    SpStep* source;
    SpStep* dest;

    // removing stuff
    void tryRemove();
    int brotherTransitionsNumber;
    inline void defineBrotherTransitionNumber(int i) { brotherTransitionsNumber = i; }

    // Drawing stuff
    bool actived;
    double arrowSize;
    QPointF sourcePoint;
    QPointF destPoint;
    SpGraph *graph;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const;
};

#endif // SPTRANSITION_H
