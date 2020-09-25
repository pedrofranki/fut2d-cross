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

#include "sptransition.h"

#include "spstep.h"
#include "spgraph.h"

SpTransition::SpTransition(SpStep * s, SpStep * d, SpGraph * g, int id):
    idNumber(id),
    source(s),
    dest(d),
    graph(g)
{
    arrowSize = 10;
    actived = false;
    brotherTransitionsNumber = 0;
    setFlag( QGraphicsItem::ItemIsSelectable, true );
}

SpTransition::~SpTransition()
{
    if(graph)
        graph->removeTransition(this);
    if(dest)
        dest->removeTransition(this, false);
    if(source)
        source->removeTransition(this, true);
}

void SpTransition::init()
{
    if(!dest)
        return;

    if(graph)
    {
        graph->addTransition(this);

        brotherTransitionsNumber = 0;
        foreach(SpTransition *t, dest->getSourceTransitionsList())
            if(source == t->getDest())
            {
                brotherTransitionsNumber=2;
                t->defineBrotherTransitionNumber(1);
                break;
            }
        adjust();
    }
}

void SpTransition::adjust()
{
    if (!source || !dest)
        return;

    QPointF a = mapFromItem(source, 0, 0);
    QLineF line(a, mapFromItem(dest, 0, 0));
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * 15) / length, (line.dy() * 15) / length);

    prepareGeometryChange();
    sourcePoint = line.p1() + edgeOffset;
    destPoint = line.p2() - edgeOffset;
}

void SpTransition::active()
{
    actived = true;
    setZValue(1);
    update();
}

void SpTransition::desactive()
{
    actived = false;
    setZValue(0);
    update();
}

void SpTransition::tryRemove()
{
    if(!source)
        return;

    qDebug() << "TRY REMOVE TRANSITION";


    if(source->canRemoveTransition(dest))
    {
        QMessageBox::information(NULL, "Success removing transition",
                                 "Transition was removed with success.");

        foreach(SpTransition *t, dest->getSourceTransitionsList())
            if(source == t->getDest())
            {
                t->defineBrotherTransitionNumber(0);
                break;
            }

        delete this;
    }
    else
        QMessageBox::information(NULL, "Action prohibited - Cann't remove transition",
                                "It's impossible remove the transition active because is \
                                 the only one between its destination and source step.");

}

void SpTransition::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton || actived)
        return;

    source->transitionWasSelected(this);
    QGraphicsItem::mousePressEvent(event);
}

void SpTransition::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    setEnabled(false);
    if(!actived && source)
        source->transitionWasSelected(this);

    event->accept();

    QMenu menu;
    QAction * removeAction = menu.addAction("Remove transition");
    QAction * resultAction =  menu.exec(event->screenPos());

    setEnabled(true);
    if(resultAction == removeAction )
        tryRemove();
}

QRectF SpTransition::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 3;
    qreal extra = arrowSize - penWidth;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath SpTransition::shape() const
{
    QLineF line(sourcePoint, destPoint);

    /*qreal p1 = sin(line.angle() * Pi / 180) * arrowSize /2;
    qreal p2 = cos(line.angle() * Pi / 180) * arrowSize /2;

    QRectF b = QRectF(QPointF(0,0), QSizeF(line.length(),arrowSize)).normalized();

    QPainterPath path;
    QMatrix mat;
    mat.translate(sourcePoint.x() - p1, sourcePoint.y() - p2)
        .rotate(-line.angle());
    path.addRect(b);
    QPainterPath newpath =  mat.map(path);*/
    double angle = acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    QPainterPath path(sourcePoint);
    if(brotherTransitionsNumber == 0)
        path.lineTo(destPoint);
    else
    {
        QPointF upPoint = QPointF(sin(angle - Pi ) * 15,
                                  cos(angle - Pi) * 15);
        QPointF downPoint = QPointF(sin(angle - Pi + Pi ) * 15,
                                    cos(angle - Pi + Pi) * 15);
        QPointF middle = (sourcePoint + destPoint) / 2;

        QPointF cPoint;
        if(brotherTransitionsNumber == 1)
            cPoint= middle + upPoint;
        else
            cPoint= middle - downPoint;

        path.quadTo(cPoint , destPoint);
    }
    QPainterPathStroker pps;
    pps.setWidth(15);
    return pps.createStroke(path);
}

void SpTransition::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);

    painter->setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    double angle = acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                                  cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                                  cos(angle - Pi + Pi / 3) * arrowSize);

    QPainterPath path(sourcePoint);
    if(brotherTransitionsNumber == 0)
        path.lineTo(destPoint);
    else
    {
        QPointF upPoint = QPointF(sin(angle - Pi ) * 15,
                                  cos(angle - Pi) * 15);
        QPointF downPoint = QPointF(sin(angle - Pi + Pi ) * 15,
                                    cos(angle - Pi + Pi) * 15);
        QPointF middle = (sourcePoint + destPoint) / 2;

        QPointF cPoint;
        if(brotherTransitionsNumber == 1)
            cPoint= middle + upPoint;
        else
            cPoint= middle - downPoint;

        path.quadTo(cPoint , destPoint);
    }


    if(actived)
    {
        painter->setBrush(Qt::green);
        painter->setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
    else
    {
        painter->setBrush(Qt::black);
        painter->setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
  painter->drawPath(path);

    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
