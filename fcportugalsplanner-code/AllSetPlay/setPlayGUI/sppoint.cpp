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


#include "sppoint.h"

#include "spfield.h"
#include "spplayer.h"
#include "spdefinitions.h"

/* constructors */
// Absolute point constructor
SpPoint::SpPoint(SpField *f, bool fromScreen, QPointF point)
{
    this->field =f;
    this->relative = nonRelative;
    cleanRelatives();
    if(fromScreen)
        changeScreenPoint(point);
    else
        realPoint = point;
    this->offsetPoint = QPoint(0,0);
    dummy = nonDummy;
}


SpPoint::SpPoint(SpField *f, bool fromScreen, QPointF offset, SpPlayer * p)
{
    this->field =f;
    this->relative = playerRelative;
    cleanRelatives();
    this->relativePlayer = p;
    if(fromScreen)
        changeScreenPoint(offset);
    else
        this->offsetPoint = offset;
    calcDummy();
}


SpPoint::SpPoint(SpField *f, bool fromScreen, QPointF offset, int r)
{
    field =f;
    relative = r;
    cleanRelatives();
    if(fromScreen)
        changeScreenPoint(offset);
    else
        this->offsetPoint = offset;
    calcDummy();
}


SpPoint::SpPoint(SpField *f, bool fromScreen, QPointF offset, QPointF p)
{
    field =f;
    relative = pointRelative;
    cleanRelatives();
    relativePoint = p;
    if(fromScreen)
        changeScreenPoint(offset);
    else
        this->offsetPoint = offset;
    dummy = false;
}


SpPoint::SpPoint(SpPoint * p)
{
    field = p->field;
    screenPoint = p->screenPoint;
    realPoint = p->realPoint;

    relative = p->relative;
    dummy = p->dummy;

    if(!relative)
    {
        cleanRelatives();
        realPoint = p->realPoint;
        offsetPoint = QPointF(0,0);
    }
    else
    {
        relativePlayer = p->relativePlayer;
        relativePoint = p->relativePoint;
        ballRelativePlayer = p->ballRelativePlayer;
        offsetPoint = p->offsetPoint;
    }
}


void SpPoint::changeScreenPoint(QPointF point)
{
    QPointF sp = truncate(point);
    screenPoint = sp;

    realPoint = roundPoint(field, sp);

    if(!relative)
        return;

    QPointF rpoint(0,0);
    if(relative == playerRelative)
    {
        if(relativePlayer)
            rpoint = relativePlayer->getPosition()->getFinalPoint();
    }
    else if(relative == ballRelative)
    {
        ballRelativePlayer = field->getCurrentBallPlayerHolder();
        if(ballRelativePlayer)
            rpoint = ballRelativePlayer->getPosition()->getFinalPoint();
        else
            rpoint = roundPoint(field, field->getLeftGoalCenter());
    }
    else if(relative == pointRelative)
        rpoint = relativePoint;
    offsetPoint = truncate(roundPoint(field, sp) - rpoint);
}


void SpPoint::changeRealPoint(QPointF point)
{
    QPointF rp = truncate(point);
    realPoint = rp;
    calcScreenPoint(rp);
}


void SpPoint::changeOffsetPoint(QPointF point)
{
    if(!relative)
        return;

    QPointF op = truncate(point);
    offsetPoint = op;

    QPointF rPoint(0,0);
    if(relative == playerRelative)
    {
        if(relativePlayer)
            rPoint = op + relativePlayer->getPosition()->getFinalPoint();
    }
    else if(relative == ballRelative)
    {
        ballRelativePlayer = field->getCurrentBallPlayerHolder();
        if(ballRelativePlayer)
            rPoint = op + ballRelativePlayer->getPosition()->getFinalPoint();
        else
            rPoint = op + roundPoint(field, field->getLeftGoalCenter());
    }
    else if(relative == pointRelative)
        rPoint = op + relativePoint;
    calcScreenPoint(rPoint);

    realPoint = roundPoint(field, screenPoint);
}


void SpPoint::setPositional()
{
    cleanRelatives();
    relative = nonRelative;
    changeScreenPoint(screenPoint);
    offsetPoint = QPointF(0,0);
    dummy = nonDummy;
}


bool SpPoint::setRelativePlayer(SpPlayer * rPlayer , SpPlayer *actorPlayer )
{
    if(actorPlayer)
    {
        SpPoint *npoint = rPlayer->getPosition();
        while(true)
        {
            SpPlayer *nplayer;
            if(npoint->isRelative() == playerRelative)
                nplayer = npoint->getRelativePlayer();
            else if(npoint->isRelative() == ballRelative)
                nplayer = npoint->getBallRelativePlayer();
            else
                break;
            if(nplayer == actorPlayer)
                return false;
            npoint = nplayer->getPosition();
        }
    }
    // doesn't created loop on relatives
    cleanRelatives();
    relative = playerRelative;
    relativePlayer = rPlayer;
    changeScreenPoint(screenPoint);
    return true;
}


void SpPoint::setBallRelativePlayer()
{
    cleanRelatives();
    relative = ballRelative;
    changeScreenPoint(screenPoint);
}

QPointF SpPoint::getFinalPoint(int stepNumber)
{
    calcDummy(stepNumber);
    if(dummy == forcedDummy)
        return realPoint;
        //return QPoint(0,0);
    if(!relative)
        return realPoint;
    else
    {
        if(relative == playerRelative)
        {
            if(relativePlayer)
                return truncate(offsetPoint + relativePlayer->getStepPosition(stepNumber)->getFinalPoint(stepNumber));
        }
        else if(relative == ballRelative)
        {
            if(ballRelativePlayer)
                return truncate(offsetPoint + ballRelativePlayer->getStepPosition(stepNumber)->getFinalPoint());
            else
            {
                ballRelativePlayer = field->getPlayerBallHolderOnStep(stepNumber);
                if(ballRelativePlayer)
                    return truncate(offsetPoint + ballRelativePlayer->getStepPosition(stepNumber)->getFinalPoint());
                else
                    return truncate(offsetPoint + field->getLeftGoalCenter());
            }
        }
        else if(relative == pointRelative)
            return truncate(offsetPoint + relativePoint);
    }
}


QPointF SpPoint::getFinalPoint()
{
    calcDummy();
    if(dummy)
        return roundPoint(field, screenPoint);
    if(!relative)
        return realPoint;
    else
    {
        if(relative == playerRelative)
        {
            if(relativePlayer)
                return truncate(offsetPoint + relativePlayer->getPosition()->getFinalPoint());
        }
        else if(relative == ballRelative)
        {
            if(ballRelativePlayer)
                return truncate(offsetPoint + ballRelativePlayer->getPosition()->getFinalPoint());
        }
        else if(relative == pointRelative)
            return truncate(offsetPoint + relativePoint);
    }
}


bool SpPoint::isRelativeTo(SpPlayer *p)
{
    if(!relative)
        return false;
    if(relative == SpPoint::ballRelative)
    {
        if(p == ballRelativePlayer)
            return true;
    }
    else if(relative == SpPoint::playerRelative)
        if(p == relativePlayer)
            return true;
    return false;
}


QPointF SpPoint::roundPoint(SpField *field, QPointF point)
{
    QPointF p;
    double part = 0.0;
    double decimal = modf (field->getPitchX(point.x()) , &part);
    if (decimal > 0.75)
        part += 1;
    else if(decimal > 0.25)
        part += 0.5;
    else if(decimal < -0.75)
        part -= 1;
    else if(decimal < -0.25)
        part -= 0.5;
    p.setX(part);

    decimal = modf (field->getPitchY(point.y()) , &part);
    if (decimal > 0.75)
        part += 1;
    else if(decimal > 0.25)
        part += 0.5;
    else if(decimal < -0.75)
        part -= 1;
    else if(decimal < -0.25)
        part -= 0.5;
    p.setY(part);
    return QPointF(truncate(p));
}


void SpPoint::calcScreenPoint(QPointF point)
{
    if(field)
        screenPoint = QPointF(field->screenX(point.x()),
                              field->screenY(point.y()));
    else
        screenPoint = QPointF(0,0);
}


QPointF SpPoint::truncate(QPointF p)
{
    return QPointF(  (double) ((int) (p.x() *100)) / 100.00,
                (double) ((int) (p.y() *100)) / 100.00);
}

QStringList SpPoint::getStrings( )
{
    calcDummy();
    QStringList strs;
    if(dummy)
        strs << "Dummy on";
    else
        strs << "On";



    if(!relative)
    {
        strs << QString::number(realPoint.x());
        strs << QString::number(realPoint.y());
        strs << "absolute";
    }

    else
    {
        strs << QString::number(offsetPoint.x());
        strs << QString::number(offsetPoint.y());
        if(relative == ballRelative)
             strs << "relative to the ball";
        else if(relative == playerRelative)
              strs << QString("relative to the player number "  +QString::number(relativePlayer->getNumber()));
        else if(relative == pointRelative)
            strs << QString("relative to the point");
    }
    return strs;
}

QString SpPoint::print()
{
    calcDummy();
    QString str = "";

    if(dummy)
        str = " ??? <-- ";

    if(!relative)
        return str +"X: " + QString::number(realPoint.x()) + " , "
                + "Y: " + QString::number(realPoint.y());

    QString pos = "X: " + QString::number(offsetPoint.x()) + " , "
            + "Y: " + QString::number(offsetPoint.y());

    if(relative == ballRelative)
        str += "Rel ball: " + pos;
    else if(relative == playerRelative)
    {
        if(relativePlayer->isIdentificatedByNumber() || relativePlayer->getName() == "")
            str += "Rel player no" + QString::number(relativePlayer->getNumber()) + ": " + pos;
        else
            str += "Rel " + relativePlayer->getName() + ": " + pos;
    }
    else if(relative == pointRelative)
        str += "Rel point " + pos;


    return str;
}


bool SpPoint::isOnLimits()
{
    return field->isInsideField(screenPoint);
}


void SpPoint::calcDummy()
{
    if(dummy == forcedDummy)
        return;
    dummy = calculatedDummy;
    if(!relative || relative == pointRelative)
        dummy =nonDummy;

    else if(relative == ballRelative)
    {
        if(ballRelativePlayer && !ballRelativePlayer->getPosition()->isDummy())
            dummy = nonDummy;
    }
    else if(relative == playerRelative)
    {
        if(relativePlayer && !relativePlayer->getPosition()->isDummy())
            dummy = nonDummy;
    }
}

void SpPoint::calcDummy(int stepNumber)
{
    if(dummy == forcedDummy)
        return;
    dummy = calculatedDummy;
    if(!relative || relative == pointRelative)
        dummy =nonDummy;

    else if(relative == ballRelative)
    {
        if(ballRelativePlayer)
        {
            if(!ballRelativePlayer->getStepPosition(stepNumber)->isDummy())
                    dummy = nonDummy;
        }
        else
        {
            ballRelativePlayer = field->getPlayerBallHolderOnStep(stepNumber);
            if(ballRelativePlayer && !ballRelativePlayer->getStepPosition(stepNumber)->isDummy())
                dummy = nonDummy;
        }

    }
    else if(relative == playerRelative)
    {
        if(relativePlayer && !relativePlayer->getStepPosition(stepNumber)->isDummy())
            dummy = nonDummy;
    }
}

SpPoint * SpPoint::generateFinalSpPoint(int stepNumber)
{
    SpPoint *p = new SpPoint(field, false, getFinalPoint(stepNumber));
    if(dummy)
        p->dummy = forcedDummy;
    return p;
}

SpPoint * SpPoint::generateFinalSpPoint()
{
    SpPoint *p = new SpPoint(field, false, getFinalPoint());
    if(dummy)
        p->dummy = forcedDummy;
    return p;
}


bool SpPoint::operator==(SpPoint sp2)
{
    if(relative != sp2.isRelative())
        return false;

    if(!relative && realPoint != (sp2.realPoint))
        return false;

    if(relative && offsetPoint != (sp2.offsetPoint))
        return false;

    if(relative == playerRelative)
    {
        if(relativePlayer != sp2.relativePlayer)
            return false;
    }
    else if(relative == pointRelative)
    {
        if(relativePoint != sp2.relativePoint)
            return false;
    }
    else if(relative == ballRelative)
        if(!ballRelativePlayer || ballRelativePlayer != sp2.ballRelativePlayer)
            return false;

    return true;
}
