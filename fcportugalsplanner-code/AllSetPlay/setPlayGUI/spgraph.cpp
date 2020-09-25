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

#include "spgraph.h"

#include "spstep.h"
#include "spsetplay.h"
#include "sptransition.h"

SpGraph::SpGraph(QFrame *)
{
    timerId = 0;

    nodes = QList<SpStep*>();
    transitions = QList<SpTransition*>();
    setMinimumSize(300, 200);
    setMaximumSize(200, 2000);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setScene(scene);

    //TODO o que e isto??
    //setCacheMode(CacheBackground);
    //setRenderHint(QPainter::Antialiasing);
    //setTransformationAnchor(AnchorUnderMouse);
    //set9esizeAnchor(AnchorViewCenter);
}

SpGraph::~SpGraph()
{
    foreach(SpTransition *t , transitions)
        scene()->removeItem(t);
    foreach(SpStep *s , nodes)
        scene()->removeItem(s);
}

void SpGraph::addNode(SpStep * node)
{
    scene()->addItem(node);
    node->setPos(calcNewPos());
    nodes << node;
}

QPointF SpGraph::calcNewPos()
{
    QPointF newPoint(0,0);
    while(true)
    {
        int high = scene()->sceneRect().right() ;
        int low = scene()->sceneRect().left() ;

        newPoint.setX(((qrand() % (high + 1 - low) + low) ));

        high = scene()->sceneRect().bottom();
        low = scene()->sceneRect().top();

        newPoint.setY(((qrand() % (high + 1 - low) + low)));

        SpStep *node = qgraphicsitem_cast<SpStep *>(scene()->itemAt(newPoint));
        if(!node || node->pos() != newPoint)
            break;
    }
    return newPoint;
}

void SpGraph::addTransition(SpTransition  * transition)
{
    scene()->addItem(transition);
    transitions << transition;
}

void SpGraph::removeNode(SpStep * node)
{
    scene()->removeItem(node);
    nodes.removeOne(node);
}

void SpGraph::removeTransition(SpTransition  * transition)
{
    scene()->removeItem(transition);
    transitions.removeOne(transition);
}

void SpGraph::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void SpGraph::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event); //TODO saber o que e isso!!

    foreach (SpStep *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (SpStep *node, nodes)
        if (node->advance())
            itemsMoved = true;

    if (!itemsMoved)
    {
        killTimer(timerId);
        timerId = 0;
    }
}

void SpGraph::wheelEvent(QWheelEvent *event)
{
  //  scaleView(pow((double)2, -event->delta() / 240.0));
}

void SpGraph::resizeEvent(QResizeEvent *)
{
    scene()->setSceneRect(frameRect());
    itemMoved();
}

void SpGraph::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    scale(scaleFactor, scaleFactor);
}

void SpGraph::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    painter->setViewport(this->frameRect());
    painter->fillRect( painter->window(), Qt::lightGray );
}
