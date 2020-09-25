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

#include "spstep.h"
#include "spgraph.h"
#include "spsetplay.h"
#include "sptransition.h"
#include "spplayer.h"
#include "spaction.h"

SpStep::SpStep(SpSetPlay *s, unsigned int idNumber , unsigned int waitTime, unsigned int abortTime ):
    idNumber(idNumber),
    waitTime(waitTime),
    abortTime(abortTime),
    automaticAbortTime(true)
{
    if(s->getNextStepId() <= (int)idNumber)
        s->incrementNextStepId(idNumber);

    spSetPlay = s;
    graph = s->getGraph();
    type = end;

    activeTransition = NULL;
    actived = false;
    newPos = QPointF(0,0);

    transitionSourceList = QList<SpTransition *> ();
    transitionDestList = QList<SpTransition *> ();


    setZValue(1);
    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemIsSelectable, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );
}

SpStep::~SpStep()
{
    activeTransition = NULL;

    foreach (SpTransition * t , transitionDestList)
        if (t)
            delete t;

   foreach (SpTransition * t , transitionSourceList)
        if (t)
            delete t;
    transitionDestList.clear();
    transitionSourceList.clear();
    if(graph)
        graph->removeNode(this);
    if(spSetPlay)
        spSetPlay->removeStep(this);
}

void SpStep::init()
{
    graph->addNode(this);
    if(idNumber == 0)
        setPos(QPointF(0,0));
}

void SpStep::tryRemove()
{
    QMessageBox message;

    if(transitionSourceList.length() != 0)
    {
        QMessageBox::information(NULL,
            "Action prohibited - Cann't remove step",
            QString("It's impossible remove the step %1 \
                    while there are transitions from this step to anothers.").arg(idNumber)
        );
    }
    else if(idNumber == 0 )
    {
        QMessageBox::information(NULL,
            "Action prohibited - Cann't remove step",
            QString("It's impossible remove the step 0. Have to be the step 0")
        );
    }
    else
    {
        QMessageBox::information(NULL,
            "Sucess removing step",
            QString("Step %1 removed with sucess! ").arg(idNumber)
        );
        delete this;
    }
}

SpTransition * SpStep::addSourceTransition(SpStep * destStep)
{
    if(!destStep)
        return NULL;

    SpTransition * newTrans = new SpTransition(this, destStep, graph, spSetPlay->getNextTransitionId());
    newTrans->init();
    destStep->transitionDestList << newTrans ;
    transitionSourceList << newTrans;
    type = middle;
    if(!activeTransition)
        activeTransition = newTrans;
    return newTrans;
}

void SpStep::removeTransition(SpTransition *transition, bool isSource)
{
    if(!transition)
        return;
    if( isSource)
    {
        transitionSourceList.removeOne(transition);
        if(activeTransition == transition)
            activeTransition = NULL;
        if(transitionSourceList.length() == 0)
            type = end;
        spSetPlay->removeTransition(transition);
    }
    else
        transitionDestList.removeOne(transition);
}

bool SpStep::canRemoveTransition(SpStep *dest)
{

    int n = 0;
    foreach(SpTransition *transition , transitionSourceList)
        if(transition->getDest() == dest)
            n++;
    if(n > 1)
        return true;

    if(dest->getDestTransitionsList().size() > 1)
        return true;

    if(dest->getIdNumber() == 0)
        return true;


    return false;
}

void SpStep::active()
{
    if(activeTransition)
        activeTransition->active();
    actived =true;
    update();
}

void SpStep::desactive()
{
    if(activeTransition)
        activeTransition->desactive();
    actived = false;
    update();
}


void SpStep::transitionWasSelected(SpTransition *t)
{
    spSetPlay->changeCurrentTransition(t);
}

void SpStep::setActiveTransition(SpTransition *t)
{
    if(activeTransition)
        activeTransition->desactive();
    activeTransition = t;
    activeTransition->active();
}

void SpStep::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton ||  actived)
        return;

    spSetPlay->changeCurrentStep(this, activeTransition);
    update();

    return QGraphicsItem::mousePressEvent(event);
}


void SpStep::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    setEnabled(false);
    spSetPlay->changeCurrentStep(this, activeTransition);

    event->accept();

    QMenu menu;
    QMenu * addTransactionMenu = menu.addMenu("Add transition");

    QList<SpStep *> allNodes = spSetPlay->getGraph()->nodeList();
    allNodes.removeOne(this);

    foreach(SpTransition *t, transitionSourceList)
        allNodes.removeOne(t->getDest());

    QMap< int, QAction * > actionList;
    foreach(SpStep * n, allNodes)
    {
        QAction * a = addTransactionMenu->addAction("Step " + QString::number(n->getIdNumber()));
        actionList.insert(n->getIdNumber(), a);
    }

    QAction * createStep = addTransactionMenu->addAction("Create a new Step");
    QAction * removeAction = menu.addAction("Remove step");
    QAction * resultAction = menu.exec(event->screenPos()); //Shows the menu in the screen position.

    setEnabled(true);
    if(resultAction == removeAction )
        tryRemove();
    else if(resultAction == createStep)
        spSetPlay->createStepByGraph(this);
    else
    {
        foreach(int k , actionList.keys())
            if(resultAction == actionList[k])
            {
                spSetPlay->connectNodes(this, k);
            }
    }
}

bool SpStep::advance()
{
    if( newPos == pos() )
        return false;

    setPos(newPos);
    return true;
}

void SpStep::calculateForces()
{
    if ( ! scene() )
    {
        newPos = pos() ;
        return;
    }

    qreal xvel = 0;
    qreal yvel = 0;
    foreach (SpStep *node, graph->nodeList())
    {
        //TODO Corrigir problema de mandar nodes para fora...
        if(node == this)
            continue;

        QLineF line(mapFromItem(node, 0, 0), QPointF(0, 0));

        qreal dx = line.dx();
        qreal dy = line.dy();
        double l = 1.0 * (dx * dx + dy * dy);

        if(l == 0)
            node->setPos((scene()->sceneRect().center() + node->pos()) / 2);

        if (l > 0)
        {
            xvel += (dx * 200.0) / l;
            yvel += (dy * 200.0) / l;
        }

        if(abs(xvel) < 2)
           xvel = 0;
        if(abs(yvel) < 2)
           yvel = 0;
    }

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 15), sceneRect.right() - 15));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 15), sceneRect.bottom() - 15));
}

int SpStep::calculateAbortTime()
{
    QList <SpPlayer *> plist = spSetPlay->getParticipantPlayers();
    int slowerActionTime = 0;
    abortTime = 0;
    foreach(SpTransition *t, transitionSourceList)
    {
        if(t == spSetPlay->getCurrentTransition())
        {
            foreach(SpPlayer *p, plist)
            {
                SpAction *act = p->getAction();
                if (act)
                {
                    int actionTime = act->getTime();
                    if(actionTime >= slowerActionTime)
                    {
                        abortTime = waitTime + actionTime;
                        slowerActionTime = actionTime;
                    }
                }
            }
            continue;
        }

        int tIdNumber = t->getIdNumber();
        foreach(SpPlayer *p, plist)
        {
            SpAction *act = p->getActionByTransition(tIdNumber);
            if (act)
            {
                int actionTime = act->getTime();
                if(actionTime > slowerActionTime)
                {
                    abortTime = waitTime + actionTime;
                    slowerActionTime = actionTime;
                }
            }
        }
    }
    return abortTime;
}

QRectF SpStep::boundingRect() const
{
    return QRectF(-15, -15, 30, 30);
}

QPainterPath SpStep::shape() const
{
    QPainterPath path;
    path.addEllipse(-15, -15, 30, 30);
    return path;
}

void SpStep::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{

    painter->setPen(Qt::NoPen);

    QRadialGradient gradient(-3, -3, 10);
    if (actived)
    {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::green).light(120));
        gradient.setColorAt(0, QColor(Qt::darkGreen).light(120));
    }
    else
    {
        gradient.setColorAt(0, QColor(184,184,184));
        gradient.setColorAt(1, QColor(84,84,84));
    }


    if(type == end)
        painter->setPen(QPen(Qt::black,3));

    painter->setBrush(gradient);

    painter->drawEllipse(-15, -15, 30, 30);

    if(idNumber == 0)
        painter->setBrush(Qt::black);
    else
        painter->setBrush(gradient);
    painter->drawEllipse(-10, -10, 20, 20);

    QFont font;
    font.setBold(true);

    painter->setFont(font);
    painter->setPen(QPen(Qt::white));
    painter->drawText(QRect(-15,-15,30,30), Qt::AlignCenter, QString("%1").arg(idNumber));
}

QVariant SpStep::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionHasChanged:
        {
            // value is the new position.
            QPointF newPos = value.toPointF();
            qreal halfWidth = boundingRect().width() / 2 ;
            QRectF rect =  QRectF(scene()->sceneRect().topLeft() + QPointF(halfWidth, halfWidth ),
                                 scene()->sceneRect().bottomRight() - QPointF(halfWidth, halfWidth ));

            if (!rect.contains(newPos))
            {
                // Keep the item inside the scene rect.
                newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
                setPos(newPos);
            }

            foreach (SpTransition *transition, transitionSourceList)
                transition->adjust();
            foreach (SpTransition *transition, transitionDestList)
                transition->adjust();

            graph->itemMoved();
            break;
        }
        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}
