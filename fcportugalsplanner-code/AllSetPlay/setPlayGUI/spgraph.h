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

#ifndef SPGRAPH_H
#define SPGRAPH_H

#include <QGraphicsView>

#include "spdefinitions.h"

class SpStep;
class SpSetPlay;
class SpTransition;

class SpGraph : public QGraphicsView
{
    Q_OBJECT

public:
    SpGraph(QFrame * parent );
    ~SpGraph();

    inline void init(SpSetPlay *s) { spSetPlay = s; }

    // Nodes
    void addNode(SpStep * step);
    void removeNode(SpStep * step);
    inline QList < SpStep* > nodeList() const { return nodes; }

    // Transitions
    void addTransition(SpTransition  * transition);
    void removeTransition(SpTransition  * transition);

    // Drawing stuff
    void itemMoved();

private:
    // Attributes
    SpSetPlay * spSetPlay;
    QList < SpStep* > nodes;
    QList < SpTransition* > transitions;

    // Drawing stuff
    int timerId;
    QPointF calcNewPos();
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *);
    void scaleView(qreal scaleFactor);
    void drawBackground(QPainter *painter, const QRectF &rect);
};

#endif // SPGRAPH_H
