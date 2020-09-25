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

#include "spaction.h"
#include "spfield.h"
#include "spplayer.h"

//TODO READ FROM FILE THESE VALUES!!
const double SpAction::maxBallVelocity = 3.0;
const double SpAction::maxBallDistance = 35.0;
const double SpAction::ballDelay = 0.94;
const double SpAction::maxPlayerVelocity = 1.05;
const double SpAction::playerDelay = 0.4;

SpAction::SpAction(SpPlayer *a, int t, SpField *f):
    type(t),
    actor(a),
    field(f),
    time(0),
    originalPaintSize(5)
{
    destActor = NULL;

    sourceSpPoint = new SpPoint(actor->getPosition());
    if(type == shoot)
        destSpPoint = new SpPoint(f, true, field->getLeftGoalCenter());
    else if(type == wait || type == hold)
        destSpPoint = sourceSpPoint;
    else
        destSpPoint = new SpPoint(actor->getPosition());


    setZValue(2);
    paintSize = originalPaintSize * field->getScale();
}

SpAction::~SpAction()
{
    destActor = NULL;
}

QString SpAction::print()
{
    QString names []= { "pass", "shoot", "dribble", "hold", "wait", "run", "pass forward", "go to offside line" };
    if(type == wait || type == shoot || type == hold)
        return names[type];
    else if (type == dribble || type == run || type == passForward)
        return names[type] + " --> " + destSpPoint->print();
    else if (type == pass && destActor)
        return names[type] + " --> " + QString::number(destActor->getNumber());
    else if (type == goToOffsideLine)
        return names[type] + " --> Y: " +  QString::number(destSpPoint->getRealPoint().y());
    else
        return "";
}

QString SpAction::getName()
{
    QString names []= { "pass", "shoot", "dribble", "hold", "wait", "run", "pass forward", "go to offside line" };
    return names[type];
}

void SpAction::setSource(QPointF p)
{
    prepareGeometryChange();
    sourceSpPoint->changeScreenPoint(p);
}

void SpAction::setDest(QPointF p)
{

    prepareGeometryChange();
    destSpPoint->changeScreenPoint(p);

}

QRectF SpAction::boundingRect() const
{
    QPointF sourcePoint = sourceSpPoint->getScreenPoint();

    if(type == wait || type == hold)
        return QRectF(sourcePoint, sourcePoint + QPointF(10,20));

    QPointF destPoint = destSpPoint->getScreenPoint();
    if(type != goToOffsideLine)
        return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
            .normalized()
            .adjusted(-4, -4, 4, 4);
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                  destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-paintSize*2, -paintSize*5, paintSize*2, paintSize*5);
}

void SpAction::actualize()
{
    prepareGeometryChange();
    //sourceSpPoint->changeScreenPoint(actor->getPosition()->getScreenPoint());
    sourceSpPoint->changeScreenPoint(actor->pos());
    sourceSpPoint->actualize();
    if(destActor )
    {
        if(!destActor->isParticipant())
        {
            actor->removeActualAction();
              update();
            return;
        }

        destSpPoint = destActor->getNextPosition();

        if(destActor->getAction() && destActor->getAction()->getType() == run)
            type = passForward;
        else
            type = pass;
    }
    else
    {
        if(destSpPoint)
            destSpPoint->actualize();
        if(type == goToOffsideLine)
            destSpPoint->setScreenX(field->getOffSideLineX());
    }

    update();

}

void SpAction::updateSize(double scale)
{
    setScale(scale);
    sourceSpPoint->scale();
    destSpPoint->scale();
}

void SpAction::updatePosOnField(double scale)
{
    actualize();

    paintSize = originalPaintSize * scale;

}


void SpAction::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(type == wait || type == hold)
    {
        drawWait(painter);
        return;
    }

    QPointF sourcePoint = sourceSpPoint->getScreenPoint();
    QPointF destPoint = destSpPoint->getScreenPoint();
    if(sourcePoint == destPoint)
        return;

    QLineF line(sourcePoint, destPoint);
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * paintSize*3) / length, (line.dy() * paintSize*3) / length);

    QPointF falseDestPoint = destPoint;

    if(type == pass && length > paintSize*10)
        falseDestPoint = destPoint - edgeOffset;

    QPointF falseSourcePoint = sourcePoint + edgeOffset ;

    line = QLineF(falseSourcePoint, falseDestPoint);


    double angle = acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    if(type == goToOffsideLine)
    {
        line.setLength(line.length() -5);
    }

    QPointF destArrowP1 = line.p2() + QPointF(sin(angle - Pi / 3) * paintSize,
                                                  cos(angle - Pi / 3) * paintSize);
    QPointF destArrowP2 = line.p2()  + QPointF(sin(angle - Pi + Pi / 3) * paintSize,
                                                  cos(angle - Pi + Pi / 3) * paintSize);
    switch (type)
    {
        case shoot:
        {
            drawShoot(painter, falseSourcePoint, destArrowP1, destArrowP2);
            break;
        }
        case passForward:
        case pass:
        {
            drawPass(painter, line, destArrowP1, destArrowP2);
            break;
        }
        case dribble:
        {
            drawDribble(painter, line, destArrowP1, destArrowP2);
            break;
        }
        case run:
        {
            drawRun(painter, line, destArrowP1, destArrowP2);
            break;
        }
        case goToOffsideLine:
        {
            drawGoToOffsideLine(painter, line, destArrowP1, destArrowP2, angle);
            break;
        }
    }

    if(destSpPoint->isRelative())
    {
        QFont font;
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(QPen(Qt::darkBlue));
        line = QLineF(sourcePoint, destPoint);
        QPointF p = line.pointAt(0.5) + QPointF( -paintSize, paintSize);
        painter->drawText(p, QString("R"));
    }
}

void SpAction::drawPass(QPainter *painter, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2)
{
    painter->setBrush(Qt::black);
    painter->setPen(QPen(Qt::black, paintSize*3/5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

void SpAction::drawShoot(QPainter *painter, QPointF falseSourcePoint, QPointF &destArrowP1, QPointF &destArrowP2)
{
    QPolygonF shootTriangle;
    shootTriangle << falseSourcePoint << destArrowP1 << destArrowP2;
    painter->setBrush(Qt::red);
    painter->setPen(QPen(Qt::white, paintSize*2/5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPolygon(shootTriangle);
}

void SpAction::drawDribble(QPainter *painter, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2)
{
    int npoints = ((int) line.length()*4) / (paintSize *10);
    double angle = acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    if (npoints < 4)
        npoints = 4;

    bool up = false;


    QPointF upPoint = QPointF(sin(angle - Pi ) * paintSize,
                              cos(angle - Pi) * paintSize);
    QPointF downPoint = QPointF(sin(angle) * paintSize,
                                cos(angle) * paintSize);

    QPainterPath dribbleLine(line.p1());
    dribbleLine.lineTo( line.pointAt( 1.0 / npoints ));
    dribbleLine.lineTo( line.pointAt( 2.0 / npoints ) + upPoint);
    int i;
    for (i = 3 ; i < npoints -1; i++)
    {
        QPointF p = line.pointAt( (double) i /npoints);
        p += up ? upPoint : downPoint;
        up = !up;
        dribbleLine.lineTo(p);
    }
    dribbleLine.lineTo(line.pointAt((double) i/npoints));
    dribbleLine.lineTo(line.p2());

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(Qt::black, paintSize*2/5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(dribbleLine);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

void SpAction::drawRun(QPainter *painter, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2)
{
    painter->setBrush(Qt::black);
    painter->setPen(QPen(Qt::black, paintSize*3/5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

void SpAction::drawWait(QPainter *painter)
{
    QPointF sourcePoint = sourceSpPoint->getScreenPoint();
    painter->setBrush(Qt::red);
    painter->setPen(QPen(Qt::red, paintSize*7/5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawText(QRectF(sourcePoint, sourcePoint + QPointF(paintSize*3,paintSize*3)), Qt::AlignCenter, "||");
}

void SpAction::drawGoToOffsideLine(QPainter *painter, QLineF &line, QPointF &destArrowP1, QPointF &destArrowP2, double angle)
{
    painter->setBrush(Qt::black);
    painter->setPen(QPen(Qt::black, paintSize*3/5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);

    painter->setPen(QPen(Qt::yellow, paintSize*3/5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QPointF p1 = line.p2() + QPointF(cos(angle)*paintSize, sin(angle) *paintSize + paintSize *5);
    QPointF p2 = line.p2() + QPointF(cos(angle)*paintSize, sin(angle) *paintSize - paintSize *5);
    painter->drawLine(p1,p2);
}

void SpAction::calculateTime()
{
    time = 0;
    if(type == hold || type == wait)
        return ;

    QPointF sourcePointOnfield = sourceSpPoint->getRealPoint();
    QPointF destPointOnField = destSpPoint->getRealPoint();

    QLineF line(sourcePointOnfield,destPointOnField);
    double length = line.length();

    if(type == run)
    {
        // TODO based on float SpeedUtils::PlayerCycles(float dist)
        time = length +1;
    }
    else if(type == dribble)
        time = (length +1)*1.5;
    else
    {
        if(length > maxBallDistance)
            time = 999;
        else
            while(length > 0)
            {
                length -= maxBallVelocity * pow(ballDelay,time);
                time++;
            }
    }
    time += 20; // TODO clean
 }


/*
    //code to turn aroung rectangles or arrows...
    qreal p1 = sin(line.angle() * Pi / 180) * arrowSize /2;
    qreal p2 = cos(line.angle() * Pi / 180) * arrowSize /2;

    QRectF b = QRectF(QPointF(0,0), QSizeF(line.length(),arrowSize)).normalized();
    painter->translate(sourcePoint - QPointF(p1, p2));
    painter->rotate(-line.angle());
    painter->drawRect(b);
*/
/*
QPainterPath SpAction::shape() const
{
    QPointF sourcePoint = sourceSpPoint->getScreenPoint();
    QPointF destPoint = destSpPoint->getScreenPoint();
    if(type == wait || type == hold)
    {
        QPainterPath path;

        path.addRect(QRectF(sourcePoint, sourcePoint + QPointF(paintSize*2,paintSize*4)));
        return path;
    }

    QLineF line(sourcePoint, destPoint);
    qreal p1 = sin(line.angle() * Pi / 180) * 6 /2;
    qreal p2 = cos(line.angle() * Pi / 180) * 6 /2;

    QRectF b = QRectF(QPointF(0,0), QSizeF(line.length(),paintSize)).normalized();

    QPainterPath path;
    QMatrix mat;
    mat.translate(sourcePoint.x() - p1, sourcePoint.y() - p2).rotate(-line.angle());
    path.addRect(b);

    QPainterPath newpath;// =  mat.map(path);
    return newpath;

}*/
