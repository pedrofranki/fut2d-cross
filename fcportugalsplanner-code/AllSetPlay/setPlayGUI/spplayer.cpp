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
#include "spfield.h"
#include "spplayer.h"
#include "spaction.h"
#include "spsetplay.h"
#include "sptransition.h"
#include "spplayereditordialog.h"


#include "mainwindow.h"
#include <QStatusBar>
#include <QMouseEvent>


SpPlayer::SpPlayer(int n, SpSetPlay * s, QString na ) :
    number(n), name(na), field(s->getField()), setPlay(s),
    actived(false), ctrlPressed(false), currentBallHolder(false), participant(false), taker(false),
    ghost(false), identificatedByNumber(false), moving(false)
{
    actualAction = NULL;

    setZValue(1);

    positionsByStep =  QMap<int, SpPoint * >() ;
    actionsByTransitions =  QMap<int, SpAction*>();
    participantOnTransitions.insert(0, QSet<int>());

    actualPosition = new SpPoint(field, true, QPointF(0,0));
    lastPosition = new SpPoint(actualPosition);

    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );
    setFlag( QGraphicsItem::ItemIsFocusable, true );


}


bool SpPlayer::setName(QString n)
{
    SpPlayer *player = setPlay->getPlayer(n);
    if(player && player != this)
        return false;

    name = n;
    return true;
}


void SpPlayer::gaveTheBallFromStep(SpStep *spStep)
{
    // foreach spstep source transition
    foreach(SpTransition * spTransition, spStep->getSourceTransitionsList())
    {
        int spTransitionIdNumber = spTransition->getIdNumber();
        SpStep *nextSpStep = spTransition->getDest();
        int nextSpStepIdNumber = nextSpStep->getIdNumber();

        SpAction * action = actionsByTransitions[spTransitionIdNumber];

        // if haven't action
        if(!action)
        {
            setPlay->setBallHolderOnStep(nextSpStepIdNumber, this);
            gaveTheBallFromStep(nextSpStep);
        }
        else
        {
            switch (action->getType())
            {
                // pass ball
                case SpAction::pass:
                case SpAction::passForward:
                    setPlay->setBallHolderOnStep(nextSpStepIdNumber, action->getDestActor());
                    action->getDestActor()->gaveTheBallFromStep(nextSpStep);
                    break;

                // take ball from spsetplay after this step
                case SpAction::shoot:
                    setPlay->updateInexistentBallOwnerAfterStep(nextSpStep);
                    break;

                // keep ball
                case SpAction::dribble:
                case SpAction::hold:
                case SpAction::run:
                case SpAction::wait:
                case SpAction::goToOffsideLine:
                default:
                    setPlay->setBallHolderOnStep(nextSpStepIdNumber, this);
                    gaveTheBallFromStep(nextSpStep);

            }
        }
    }
}

void SpPlayer::updateStep(SpStep *spStep)
{
#ifdef DEBUGSAVE
    cerr << "      SpPlayer::updateStep(): Enter!" << endl;//JAF2014
#endif

    int spStepIdNumber = spStep->getIdNumber();

#ifdef DEBUGSAVE
    cerr << "      SpPlayer::updateStep(): spStepIdNumber = " << spStepIdNumber << endl;//JAF2014
#endif

    SpPlayer * ballOwner = setPlay->getBallHolderOnStep(spStepIdNumber);
    bool ballOwnerOnSpStep;
    if(ballOwner == this)
    {
        ballOwnerOnStep << spStepIdNumber;
        ballOwnerOnSpStep = true;
    }
    else
    {
        ballOwnerOnStep.remove(spStepIdNumber);
        ballOwnerOnSpStep = false;
    }

#ifdef DEBUGSAVE
    cerr << "      SpPlayer::updateStep(): Going to process spTransitions!" << endl;//JAF2014
#endif

    // foreach spstep source transition
    foreach(SpTransition * spTransition, spStep->getSourceTransitionsList())
    {
        int spTransitionIdNumber = spTransition->getIdNumber();
        SpStep *nextSpStep = spTransition->getDest();
        int nextSpStepIdNumber = nextSpStep->getIdNumber();

#ifdef DEBUGSAVE
        cerr << "      SpPlayer::updateStep(): Going to process SpTransition number " << spTransitionIdNumber << endl;//JAF2014
        cerr << "      from current SpStep to SpStep number " << nextSpStepIdNumber << endl;//JAF2014
#endif

        SpAction * action = actionsByTransitions[spTransitionIdNumber];

        // correct actions

        if(!action)
        {
            if(ballOwnerOnSpStep)
            {
                if(this->isTaker() && setPlay->ballStoppedOnStep(spStepIdNumber))
                    setPlay->setBallStoppedOnStep(nextSpStepIdNumber);
            }
        }
        else
        {
            if(ballOwnerOnSpStep)
            {
                switch (action->getType())
                {
                    // move to action destination
                    case SpAction::run:
                        if( !(setPlay->getSituation()== keeper_catch
                                && setPlay->ballStoppedOnStep(spStepIdNumber)
                                && isTaker()))
                            actionsByTransitions[spTransitionIdNumber]->setType(SpAction::dribble);
                        break;
                    case SpAction::wait:
                        actionsByTransitions[spTransitionIdNumber]->setType(SpAction::hold);
                        break;
                    case SpAction::goToOffsideLine:
                        actionsByTransitions[spTransitionIdNumber] = NULL;
                        break;
                    default:
                        break;
                }

                // check if ball keeps stopped
                if(this->isTaker() && setPlay->ballStoppedOnStep(spStepIdNumber))
                {
                    if(action->getType() != SpAction::pass && action->getType() != SpAction::passForward
                            && action->getType() != SpAction::shoot )
                    {
                          //actionsByTransitions[spTransitionIdNumber] = NULL;
                            setPlay->setBallStoppedOnStep(nextSpStepIdNumber);

                    }
                    else
                        setPlay->removeBallStoppedOnStep(nextSpStepIdNumber);
                }
            }
            else
            {
                switch (action->getType())
                {
                    // move to action destination
                    case SpAction::dribble:
                        action->setType(SpAction::run);
                        break;
                    case SpAction::hold:
                        action->setType(SpAction::wait);
                        break;
                    case SpAction::pass:
                    case SpAction::passForward:
                    case SpAction::shoot:
                    {
                        conditionsByTransitions.remove(spTransitionIdNumber);
                        actionsByTransitions[spTransitionIdNumber] = NULL;
                    }
                }
            }

            // update action time TODO aqui???
        }


        // corrrect positions

        // if haven't action
        if(!action)
        {
            // keep position on next step.
            if(positionsByStep[spStepIdNumber]->isDummy())
                positionsByStep[nextSpStepIdNumber] = positionsByStep[spStepIdNumber];
            else
                positionsByStep[nextSpStepIdNumber] = positionsByStep[spStepIdNumber]->generateFinalSpPoint(spStepIdNumber);
        }
        else
        {
            switch (action->getType())
            {
                // move to action destination
                case SpAction::run:
                case SpAction::dribble:
#ifdef DEBUGSAVE
                    cerr << "      SpPlayer::updateStep(): Action = run or dribble: moving to position (";//JAF2014
#endif
                    positionsByStep[nextSpStepIdNumber] = actionsByTransitions[spTransitionIdNumber]->getSpDest()->generateFinalSpPoint(spStepIdNumber);
                    break;
                case SpAction::goToOffsideLine: // have to be dummy!
#ifdef DEBUGSAVE
                    cerr << "      SpPlayer::updateStep(): Action = gotoOffSideLine: moving to position (";//JAF2014
#endif
                    positionsByStep[nextSpStepIdNumber] = actionsByTransitions[spTransitionIdNumber]->getSpDest()->generateFinalSpPoint(spStepIdNumber);
                    positionsByStep[nextSpStepIdNumber]->setDummy();
                    break;

                // keep position
                case SpAction::pass:
                case SpAction::passForward:
                case SpAction::shoot:
                case SpAction::hold:
                case SpAction::wait:
                default:
#ifdef DEBUGSAVE
                cerr << "      SpPlayer::updateStep(): Action = pass, shoot, passForward, etc...: moving to position (";//JAF2014
#endif
                if(positionsByStep[spStepIdNumber]->isDummy())
                    positionsByStep[nextSpStepIdNumber] = positionsByStep[spStepIdNumber];
                else
                    positionsByStep[nextSpStepIdNumber] = positionsByStep[spStepIdNumber]->generateFinalSpPoint(spStepIdNumber);

            }
#ifdef DEBUGSAVE
            cerr << positionsByStep[nextSpStepIdNumber]->getFinalPoint().x() << ", "<<positionsByStep[nextSpStepIdNumber]->getFinalPoint().y() << ")" << endl;//JAF2014
#endif
        }
#ifdef DEBUGSAVE
        cerr << "      SpPlayer::updateStep(): Recursivelly calling updateStep(nextSpStep)!" << endl;//JAF2014
#endif
        updateStep(nextSpStep);
    }
}




void SpPlayer::createParticipionTransitionSet(int stepIdNumber)
{
    participantOnTransitions.insert(stepIdNumber, QSet<int>());
}

void SpPlayer::save(SpStep *step, SpTransition *transition)
{
#ifdef DEBUGSAVE
    cerr << "    SpPlayer::save(): Enter!" << endl;//JAF2014
#endif

    if(! participant)
    {
        qDebug() << "Player save... impossivel chegar aqui se nao e participant.";
        exit(-1);
        return;
    }

    int stepIdNumber = step->getIdNumber();
#ifdef DEBUGSAVE
    cerr << "    SpPlayer::save(): stepIdNumber = " << stepIdNumber << endl;//JAF2014
#endif
    // save position
    positionsByStep[stepIdNumber] = actualPosition;


    bool canRemove = field->canRemove(this);

    //if ghost
    if(ghost)
    {
#ifdef DEBUGSAVE
        cerr << "    SpPlayer::save(): ghost = true..." << endl;//JAF2014
#endif
        ballOwnerOnStep.remove(stepIdNumber);
        if(transition == NULL) // final step...
        {
            //participantOnTransitions.remove(stepIdNumber);
        }
        else
        {
            int transitionIdNumber = transition->getIdNumber();
            // remove (to have sure) all actions...
            if(actionsByTransitions[transitionIdNumber] != NULL)
            {
                 qDebug() << "Player save 2... impossivel chegar aqui se nao e participant.";
                 exit(-1);
            }
            //remove transition from participant transitions!!
            participantOnTransitions[stepIdNumber].remove(transitionIdNumber);
        }
    }
    else
    {
#ifdef DEBUGSAVE
        cerr << "    SpPlayer::save(): ghost = false..." << endl;//JAF2014
#endif
        if(transition == NULL  ) // final step... and it's not holder.. so haven't actions
        {
//             participantOnTransitions[stepIdNumber].remove(transitionIdNumber);
        }
        else
        {            
            int transitionIdNumber = transition->getIdNumber();
#ifdef DEBUGSAVE
            cerr << "    SpPlayer::save(): transition != NULL, transitionIdNumber=" << transitionIdNumber << endl;//JAF2014
#endif
            if(!currentBallHolder && canRemove && !actualAction)
                 participantOnTransitions[stepIdNumber].remove(transitionIdNumber);
            else
            {
                // add transition to participant transitions
                participantOnTransitions[stepIdNumber] << transitionIdNumber;

#ifdef DEBUGSAVE
                cerr << "    SpPlayer::save(): saving actualAction to vector actionsByTransitions -> ";//JAF2014
                if(actualAction!=NULL)
                    cerr << "actualAction=" << actualAction->getName().toStdString() << endl;//JAF2014
#endif

                // save actualAction ... can be NULL!!
                actionsByTransitions[transitionIdNumber] = actualAction;
            }
        }

        if(currentBallHolder)
        {
#ifdef DEBUGSAVE
            cerr << "    SpPlayer::save(): setting ballHolder for this step." << endl;//JAF2014
#endif

            setPlay->setBallHolderOnStep(stepIdNumber, this);
            ballOwnerOnStep << stepIdNumber;
        }
        else
            ballOwnerOnStep.remove(stepIdNumber);
    }

    //correctOthersTransitions();

}




void SpPlayer::actualize(SpStep *step, SpTransition *transition, bool ballHolder)
{
    currentBallHolder = ballHolder;
    actualAction = NULL;
    participant = false;
    ghost = false;

    int stepId = step->getIdNumber();

    if(positionsByStep.contains(stepId))
    {
        actualPosition = positionsByStep[stepId];
        actualPosition->actualize();
        lastPosition = new SpPoint(actualPosition);
        participant = true;

        if( transition )
        {
            int transitionId = transition->getIdNumber();
            if(actionsByTransitions.contains(transitionId))
                actualAction = actionsByTransitions[transitionId];
            ghost = !currentBallHolder && isGhostOnTrasitionOfStep(stepId, transitionId);
        }
        else
        {
            // check if already have a ghost set for the step and create if not. TODO remove on delete transition
            if (!participantOnTransitions.contains(stepId))
                participantOnTransitions.insert(stepId, QSet<int>());
            else
                ghost = !currentBallHolder && isGhostOnStep(stepId);
        }

        if(!conditionsByStep.contains(stepId))
        {
            QList<int> l;
            conditionsByStep.insert(stepId, l);
        }
    }

    bool flagbool = (stepId == 0 || ghost);
    setFlag( QGraphicsItem::ItemIsMovable, flagbool );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, flagbool );
    setFlag( QGraphicsItem::ItemIsFocusable, flagbool );
}

void SpPlayer::addAction(SpAction * newAction)
{
    if(actualAction != NULL)
        scene()->removeItem(actualAction);

    actualAction = newAction;
    scene()->addItem(actualAction);

    if(setPlay->getCurrentTransition() == NULL)
    {
       SpTransition *t =setPlay->createTransition();
       setPlay->changeCurrentTransition(t);
    }

    if(newAction->getType() != SpAction::hold &&
            newAction->getType() != SpAction::wait &&
            newAction->getType() != SpAction::shoot &&
            newAction->getType() != SpAction::pass &&
            newAction->getType() != SpAction::passForward)
    {
        SpPlayerEditorDialog *dialog = new SpPlayerEditorDialog(setPlay);
        dialog->init(this, SpPlayerEditorDialog::transitionTab);
        dialog->exec();
    }


    field->itemMoved();


    if(newAction->getType() == SpAction::pass)
        conditionsByTransitions.insert(setPlay->getCurrentTransitionNumber(), canPassPlayer);
    else if(newAction->getType() == SpAction::passForward)
        conditionsByTransitions.insert(setPlay->getCurrentTransitionNumber(), canPassRegion);
    else if(newAction->getType() == SpAction::shoot)
        conditionsByTransitions.insert(setPlay->getCurrentTransitionNumber(), canShoot);

    setPlay->changeAbortTime();
}

void SpPlayer::removeActualAction()
{
    SpTransition * cur = setPlay->getCurrentTransition();
    if(cur != NULL &&  actionsByTransitions.contains(cur->getIdNumber()))
        actionsByTransitions.remove(cur->getIdNumber());
    if(actualAction)
    {
        delete actualAction;
        actualAction = NULL;
    }
    setPlay->changeAbortTime();

    conditionsByTransitions.clear();

}

void SpPlayer::removeTransition(unsigned int idNumber)
{
    if(!actionsByTransitions.isDetached())
        return;
    if(actionsByTransitions.contains(idNumber))
    {
        delete actionsByTransitions[idNumber];
        actionsByTransitions.remove(idNumber);
        conditionsByTransitions.remove(idNumber);
    }
}

void SpPlayer::removeFromStep(int stepNumber)
{
    if(taker && setPlay->isBallStopped())
        return;

    bool canRemove = field->canRemove(this);
    if( !canRemove) // Keep the item inside the field rect.
        return;

    ghost = true;
    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );
    setFlag( QGraphicsItem::ItemIsFocusable, true );

    removeActualAction();
    field->itemMoved();

}

void SpPlayer::removeStep(int stepNumber)
{
    positionsByStep.remove(stepNumber);
    participantOnTransitions.remove(stepNumber);
    if(conditionsByStep.contains(stepNumber))
    {
        conditionsByStep[stepNumber].clear();
        conditionsByStep.remove(stepNumber);
    }
}

void SpPlayer::cameFromGhost()
{
    ghost = false;
    bool flagbool = (setPlay->getCurrentStepNumber() == 0 || ghost);
    setFlag( QGraphicsItem::ItemIsMovable, flagbool );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, flagbool );
    setFlag( QGraphicsItem::ItemIsFocusable, flagbool );

    field->itemMoved();
}

SpPoint * SpPlayer::getNextPosition() const
{
    if(actualAction && (
                actualAction->getType() == SpAction::run ||
                actualAction->getType() == SpAction::dribble))
        return actualAction->getSpDest();
    return actualPosition;
}

void SpPlayer::setRealPosition(QPointF point)
{
    if(point != actualPosition->getRealPoint())
    {
        actualPosition->changeRealPoint(point);
        setPos(actualPosition->getScreenPoint());
        setPlay->changeAbortTime();

      //  field->actualizePlayerPositions(this, ctrlPressed, true);
        lastPosition = new SpPoint(actualPosition);
    }


}

void SpPlayer::setScreenPosition(QPointF point)
{
    actualPosition->changeScreenPoint(point);
    setPos(actualPosition->getScreenPoint());
    setPlay->changeAbortTime();

    lastPosition = new SpPoint(actualPosition);
}

void SpPlayer::updatePosOnField(double scale)
{
    prepareGeometryChange();
    setScale(scale);


    actualPosition->scale();

    lastPosition->scale();
    setPos(actualPosition->getScreenPoint());
    if(actualAction)
    {
        actualAction->updatePosOnField(scale);

    }

}

void SpPlayer::moveToSpecialArea(bool forcedummy)
{
    participant = true;
    currentBallHolder = true;
    taker = true;
    setScreenPosition(field->getSpecialAreaPosition());
    if(forcedummy)
        actualPosition->setDummy();
}

void SpPlayer::gaveTheBall()
{
    currentBallHolder =true;
    addStepCondition(setPlay->getCurrentStepNumber(), bowner);
    if(actualAction)
    {
        if(actualAction->getType() == SpAction::run)
            actualAction->setType(SpAction::dribble);
        else if(actualAction->getType() == SpAction::wait)
            actualAction->setType(SpAction::hold);
        else if(actualAction->getType() == SpAction::goToOffsideLine)
            removeActualAction();
    }
    update();
    field->itemMoved();

}

void SpPlayer::takeTheBall()
{
    currentBallHolder = false;
    int stepNumber = setPlay->getCurrentStepNumber();
    if(conditionsByStep.contains(stepNumber))
        conditionsByStep[stepNumber].removeOne(bowner);
    if(actualAction)
    {
        if(actualAction->getType() == SpAction::dribble)
            actualAction->setType(SpAction::run);
        else if(actualAction->getType() == SpAction::hold)
            actualAction->setType(SpAction::wait);
        else
            removeActualAction();
    }
    if(actualPosition->isDummy() == SpPoint::forcedDummy)
        actualPosition->setPositional();
    update();
    field->itemMoved();

}

void SpPlayer::actualizePosition()
{
    actualPosition->actualize();
    if(actualAction)
        actualAction->actualize();

}

QString SpPlayer::printAction() const
{
    return actualAction ? actualAction->print() : "---";
}

bool SpPlayer::hasStepCondition(int step, int condition) const
{
    if(conditionsByStep.contains(step))
       return conditionsByStep[step].contains(condition);
    return false;
}

void SpPlayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    updateInformationFrame();
    moving =true;
    actived = true;

    QPointF mouseScreenPoint = event->scenePos();

    qreal halfWidth = boundingRect().width() / 4;
    QRectF rect =  QRectF(scene()->sceneRect().topLeft() + QPointF(halfWidth, halfWidth ),
                        scene()->sceneRect().bottomRight() - QPointF(halfWidth, halfWidth ));


    if(currentBallHolder)
    {
        if(taker && setPlay->isBallStopped())
        {

            if(!field->isOnSpecialAreas(mouseScreenPoint))
            {
                setPlay->setStatusMessage(QString("Taker player can't leave the special area before put ball in moviment..."));
                setCursor(Qt::ForbiddenCursor);
                update();
                return;
            }
        }

        else if(!field->canMove(this))
        {
            setPlay->setStatusMessage(QString("Player cann't move because pass cann't be so long..."));
            setCursor(Qt::ForbiddenCursor);
            update();
            return;
        }

        else if(!field->isInsideField(mouseScreenPoint)) // Keep the item inside the field rect.
        {
            mouseScreenPoint.setX(qMin(field->getFieldRect().right(), qMax(mouseScreenPoint.x(), field->getFieldRect().left())));
            mouseScreenPoint.setY(qMin(field->getFieldRect().bottom(), qMax(mouseScreenPoint.y(), field->getFieldRect().top())));

            setPlay->setStatusMessage(QString("Current ball holder player cann't leave the field..."));

            setCursor(Qt::ForbiddenCursor);
            update();

            return;
        }
    }
    else if(!field->isInsideField(mouseScreenPoint))
    {
        bool canRemove = field->canRemove(this);
        if( !canRemove) // Keep the item inside the field rect.
        {
            setPlay->setStatusMessage(QString("Player cann't leave the field. First remove relationships with him"));
            setCursor(Qt::ForbiddenCursor);
            update();

            return;
        }
        if( !canRemoveFromAllSetPlay()) // Keep the item inside the field rect.
        {
            setPlay->setStatusMessage(QString("Player cann't leave the field. He participates in others steps/transitions"));
            setCursor(Qt::ForbiddenCursor);
            update();
            return;
        }

        if (!rect.contains(mouseScreenPoint)) // Keep the item inside the scene rect.
        {
            mouseScreenPoint.setX(qMin(rect.right(), qMax(mouseScreenPoint.x(), rect.left())));
            mouseScreenPoint.setY(qMin(rect.bottom(), qMax(mouseScreenPoint.y(), rect.top())));
        }
    }


    QPointF originalActualScreenPoint = actualPosition->getScreenPoint();

    actualPosition->changeScreenPoint(mouseScreenPoint);
    field->actualizePlayerPositions(this, ctrlPressed, false);
    if(actualAction)
    {
        actualAction->actualize();
        if(!field->isInsideField(actualAction->getSpDest()->getScreenPoint()))
        {
            actualPosition->changeScreenPoint(originalActualScreenPoint);
            actualAction->actualize();
            setPlay->setStatusMessage(QString("Player can't move because have a action that can't leave the field "));
            setCursor(Qt::ForbiddenCursor);
            update();
            return;
        }
    }

    bool canMove = field->canMove(this);
    if(!ghost && participant)
    {

        if(!canMove)
        {
            actualPosition->changeScreenPoint(originalActualScreenPoint);
            if(actualAction)
                actualAction->actualize();
            setPlay->setStatusMessage(QString("Player can't move because pass cann't be so long..."));
            setCursor(Qt::ForbiddenCursor);
            update();
            return;
        }
        else if( !field->actualizePlayerPositions(this, ctrlPressed, false) )
        {

            actualPosition->changeScreenPoint(originalActualScreenPoint);
            if(actualAction)
                actualAction->actualize();
            setPlay->setStatusMessage(QString("Player can't move to that position, because relationships with him"));
            setCursor(Qt::ForbiddenCursor);
            update();
            return;
        }
        else
        {
            setPos(actualPosition->getScreenPoint());

        }
    }
    else
    {

        setPos(actualPosition->getScreenPoint());
    }
    setCursor(Qt::ArrowCursor);
    setPlay->clearStatusMessage();
    update();
}

void SpPlayer::updateInformationFrame()
{
    if(participant)
    {

        if(ghost)
            setPlay->actualizePlayerInfo(QStringList() << QString::number(number) << name,
                                            QStringList(),QStringList());
        else
        {

            QString stepCond = "";
            if(conditionsByStep[setPlay->getCurrentStepNumber()].size())
                foreach(int i, conditionsByStep[setPlay->getCurrentStepNumber()])
                    stepCond += QString(setPlayPlayerConditionNames[i]) + ", " ;

            QStringList actionSL;
            if(actualAction)
            {
                actionSL << actualAction->getName() << actualAction->getSpDest()->getStrings();
                if(conditionsByTransitions.contains(setPlay->getCurrentTransitionNumber()))
                    actionSL << setPlayPlayerConditionNames[conditionsByTransitions[setPlay->getCurrentTransitionNumber()]];
                else
                    actionSL << "";
            }

            setPlay->actualizePlayerInfo(QStringList() << QString::number(number) << name,
                                        QStringList() << actualPosition->getStrings() << stepCond,
                                        actionSL);
        }
    }
    else
        setPlay->actualizePlayerInfo(QStringList() << QString::number(number) << name,QStringList(),QStringList());

}

void SpPlayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    updateInformationFrame();
    actived = true;
    setPlay->changeActivePlayer(this);

    if(setPlay->getCurrentStepNumber() == 0)
    {

        update();
    }

    return QGraphicsItem::mousePressEvent(event);
}

void SpPlayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int tolerance = moving ? 0.5 : 5;
    moving = false;
    actived = false;
    setCursor(Qt::ArrowCursor);
    setPlay->clearStatusMessage();
    update();

    QPointF mouseScreenPoint = event->scenePos();

    QPointF actualPositionRealPoint = actualPosition->getRealPoint();
    QPointF realPointFromMouseScreenPoint(field->getPitchX(mouseScreenPoint.x()),
                                          field->getPitchY(mouseScreenPoint.y()));

    QLineF linedif(actualPositionRealPoint, realPointFromMouseScreenPoint);

    if(linedif.length() < tolerance)
        return QGraphicsItem::mouseReleaseEvent(event);

    QPointF originalActualScreenPoint = actualPosition->getScreenPoint();
    actualPosition->changeScreenPoint(mouseScreenPoint);
    if(actualAction)
    {
        actualAction->actualize();
        if(!field->isInsideField(actualAction->getSpDest()->getScreenPoint()))
        {
            actualPosition->changeScreenPoint(originalActualScreenPoint);
            actualAction->actualize();
            update();
            return;
        }
    }

    if(taker && setPlay->isBallStopped()) // the player who starts the setplay cann't get out
    {
        if( !field->isOnSpecialAreas(mouseScreenPoint) || !field->canMove(this) )
            actualPosition->changeScreenPoint(originalActualScreenPoint);
        else if( !field->actualizePlayerPositions(this, ctrlPressed, true) )
            actualPosition->changeScreenPoint(originalActualScreenPoint);
        if(actualAction)
            actualAction->actualize();
        update();
        return QGraphicsItem::mouseReleaseEvent(event);
    }

    double halfWidth = boundingRect().width() / 2;
    QRectF fieldRectangle = field->getFieldRect();
    QRectF limit = QRectF(fieldRectangle.topLeft() - QPointF(halfWidth, halfWidth ),
                          fieldRectangle.bottomRight() + QPointF(halfWidth, halfWidth ));

    if (limit.contains(mouseScreenPoint) )
    {
        if(!fieldRectangle.contains(mouseScreenPoint))
        {
            mouseScreenPoint.setX(qMin(fieldRectangle.right(), qMax(mouseScreenPoint.x(), fieldRectangle.left())));
            mouseScreenPoint.setY(qMin(fieldRectangle.bottom(), qMax(mouseScreenPoint.y(), fieldRectangle.top())));
            actualPosition->changeScreenPoint(mouseScreenPoint);

        }
        if(participant)
        {
            if(!field->canMove(this) )
               actualPosition->changeScreenPoint(originalActualScreenPoint);
            else if( !field->actualizePlayerPositions(this, ctrlPressed, true) )
               actualPosition->changeScreenPoint(originalActualScreenPoint);
        }
        else
        {
            participant = true;
            participantOnTransitions.insert(setPlay->getCurrentStepNumber(), QSet<int>());
            field->removePlayerFromBench(this);
            setScreenPosition(mouseScreenPoint);
        }
        update();
    }
    else
    {
        if( !field->canRemove(this)) // Keep the item inside the field rect.
        {
            actualPosition->changeScreenPoint(originalActualScreenPoint);
            if(actualAction)
                actualAction->actualize();
            return QGraphicsItem::mouseReleaseEvent(event);
        }
        if( !canRemoveFromAllSetPlay()) // Keep the item inside the field rect.
        {
            actualPosition->changeScreenPoint(originalActualScreenPoint);
            if(actualAction)
                actualAction->actualize();
            return QGraphicsItem::mouseReleaseEvent(event);
        }
        else
        {
            removeFromAllSetPlay();
        }

    }
    if(actualAction)
        actualAction->actualize();

    return QGraphicsItem::mouseReleaseEvent(event);
}

void SpPlayer::openConfigWindow()
{
    if(participant)
    {
        if (ghost)
            cameFromGhost();
        actived = true;
        SpPlayerEditorDialog * dialog = new SpPlayerEditorDialog(setPlay);
        dialog->init(this);
        dialog->exec();
        update();
        if(actualAction)
            actualAction->update();
        field->actualizePlayerPositions(this, false, true);
        actived = false;
        setPlay->changeAbortTime();
    }

}

void SpPlayer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    updateInformationFrame();
    openConfigWindow();
}

void SpPlayer::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(!participant)
        return;
    setEnabled(false);
    actived = true;
    event->accept();

    QMenu menu;
    QAction * openEditingWindow = menu.addAction("Advanced settings");
    menu.addSeparator();
    QAction * removeCurAction = menu.addAction("Remove current action");
    QAction * passAction  = menu.addAction("Pass ");
    QAction * kickAction = menu.addAction("Shoot");
    QAction * dribbleAction = menu.addAction("Dribble");
    QAction * holdAction = menu.addAction("Hold");
    QAction * runAction = menu.addAction("Run");
    QAction * waitAction = menu.addAction("Wait");
    QAction * goToOffSideLineAction = menu.addAction("Go to offside line");
    QAction * becomeOwnerAction = menu.addAction("Become Owner");

    QMenu* passForwardMenu = new QMenu("Pass forward to player no...");
    QMap< int, QAction * > playerList;
    foreach(SpPlayer *p, field->getInPlayers())
    {
        if (p == this)
            continue;
        QAction * a = passForwardMenu->addAction(QString::number(p->getNumber()));
        playerList.insert(p->getNumber(), a);
    }




    int situation =  setPlay->getSituation();
    menu.addSeparator();
    QAction *changeActionPositionAction = menu.addAction("Change action position...");
    if(actualAction)
    {
        int type = actualAction->getType();
        if(!(type != SpAction::pass && type != SpAction::wait && type != SpAction::hold
                && type != SpAction::shoot && type != SpAction::goToOffsideLine))
            menu.removeAction(changeActionPositionAction);
    }
    else
        menu.removeAction(changeActionPositionAction);

    QAction *changePositionAction = menu.addAction("Change position...");
    if( !(! ghost && setPlay->getCurrentStepNumber() == 0))
        menu.removeAction(changePositionAction);

    QAction * removePlayerAction = menu.addAction("Remove Player");
    QAction * addPlayerAction = menu.addAction("Add Player");

    if( ! ghost)
    {
        menu.removeAction(addPlayerAction);
        if( ! actualAction )
            menu.removeAction(removeCurAction);
        else
            menu.insertSeparator(passAction);

        if( ! currentBallHolder )
        {

            bool canRemove = field->canRemove(this);
            if( !canRemove)
                menu.removeAction(removePlayerAction);
            menu.removeAction(passAction);
            menu.removeAction(kickAction);
            menu.removeAction(dribbleAction);
            menu.removeAction(holdAction);

            if( (setPlay->getSituation() != play_on || setPlay->getCurrentStepNumber() != 0)
                    || !field->isOnSpecialAreas(actualPosition->getScreenPoint()))
                menu.removeAction(becomeOwnerAction);
            else
                menu.insertSeparator(becomeOwnerAction);
            if( setPlay->getSituation()  != play_on && setPlay->isBallStopped())
            {
                if(setPlay->getSituation()  == corner_kick || setPlay->getSituation()  == throw_in)
                    menu.removeAction(goToOffSideLineAction);
            }
        }
        else
        {
            menu.insertMenu(passAction, passForwardMenu);
            menu.removeAction(waitAction);
            menu.removeAction(becomeOwnerAction);
            menu.removeAction(goToOffSideLineAction);
            menu.removeAction(removePlayerAction);

            // special case... ball still stopped ... the taker cann't do some things...

            if( situation != play_on && setPlay->isBallStopped())
            {
                if(situation == corner_kick || situation == dir_free_kick
                        || situation == goal_kick || situation == keeper_catch )
                    menu.removeAction(dribbleAction);
                else if(situation == ind_free_kick || situation == throw_in ||
                        situation == kick_off)
                {
                    menu.removeAction(dribbleAction);
                    menu.removeAction(kickAction);
                }
                if(situation != keeper_catch)
                    menu.removeAction(runAction);
            }
            else
                menu.removeAction(runAction);
        }

        foreach(SpPlayer *p, field->getInPlayers())
            if(p->getAction() && p->getAction()->getDestActor() == this)
            {
                menu.removeAction(removePlayerAction);
                break;
            }
    }
    else
    {
        menu.removeAction(passAction);
        menu.removeAction(kickAction);
        menu.removeAction(dribbleAction);
        menu.removeAction(holdAction);
        menu.removeAction(runAction);
        menu.removeAction(waitAction);
        menu.removeAction(becomeOwnerAction);
        menu.removeAction(goToOffSideLineAction);
        menu.removeAction(removePlayerAction);
        menu.removeAction(removeCurAction);
    }


    QAction * resultAction = menu.exec(event->screenPos() +QPoint(10,10) );

    if(resultAction == openEditingWindow)
    {
        actived = true;
        SpPlayerEditorDialog * dialog = new SpPlayerEditorDialog(setPlay);
        dialog->init(this);
        dialog->exec();
        field->actualizePlayerPositions(this, false, true);
        actived = false;
        update();
        setPlay->changeAbortTime();
    }
    else if(resultAction == passAction)
        field->addAction(this, SpAction::pass);
    else if(resultAction == kickAction)
        addAction(new SpAction(this, SpAction::shoot, field));
    else if(resultAction == dribbleAction)
        field->addAction( this, SpAction::dribble);
    else if(resultAction == holdAction)
        addAction(new SpAction(this, SpAction::hold, field));
    else if(resultAction == runAction)
        field->addAction(this, SpAction::run);
    else if(resultAction == waitAction )
        addAction(new SpAction(this, SpAction::wait, field));
    else if(resultAction == goToOffSideLineAction )
        field->addAction(this, SpAction::goToOffsideLine);
    else if(resultAction == becomeOwnerAction )
        setPlay->changeCurrentHolderBallPlayer(this);
    else if(resultAction == removeCurAction )
        removeActualAction();
    else if(resultAction == removePlayerAction )
        removeFromStep(setPlay->getCurrentStepNumber());
    else if(resultAction == addPlayerAction )
        cameFromGhost();

    else if(resultAction == changePositionAction)
    {
        SpPlayerEditorDialog *dialog = new SpPlayerEditorDialog(setPlay);
        dialog->init(this, SpPlayerEditorDialog::stepTab);
        dialog->exec();
        field->actualizePlayerPositions(this, false, true);
        actived = false;
        update();
        setPlay->changeAbortTime();
    }
    else if(resultAction == changeActionPositionAction)
    {
        SpPlayerEditorDialog *dialog = new SpPlayerEditorDialog(setPlay);
        dialog->init(this, SpPlayerEditorDialog::transitionTab);
        dialog->exec();
        field->actualizePlayerPositions(this, false, true);
        actived = false;
        update();
        setPlay->changeAbortTime();
    }
    else
    {
        foreach(int k , playerList.keys())
            if(resultAction == playerList[k])
                field->addActionPassForward(this, SpAction::passForward, setPlay->getPlayer(k));
    }


    field->itemMoved();
    setEnabled(true);
    actived = false;
}

void
SpPlayer::keyPressEvent ( QKeyEvent * event )
{


    if(event->key() == Qt::Key_Shift)
    {

        if(!ctrlPressed && actived)
        {            

            QPointF mouseScreenPoint = field->mapFromGlobal(QCursor::pos());
            SpPoint * oriActPosition = new SpPoint(actualPosition);
            actualPosition->changeScreenPoint(mouseScreenPoint);

            if(currentBallHolder && taker && setPlay->isBallStopped()
                    && !field->isOnSpecialAreas(mouseScreenPoint))
            {
                setPlay->setStatusMessage(QString("Taker player can't leave the special area before put ball in moviment..."));
                setCursor(Qt::ForbiddenCursor);
                setPos(oriActPosition->getScreenPoint());
            }

            else if(field->actualizePlayerPositions(this, true, false))
            {
                setPlay->clearStatusMessage();
                setCursor(Qt::ArrowCursor);
                setPos(actualPosition->getScreenPoint());
            }
            else
            {
                setPlay->setStatusMessage(QString("Player cann't move to that position, because relationships with him"));
                setCursor(Qt::ForbiddenCursor);
                setPos(oriActPosition->getScreenPoint());
            }
            actualPosition = oriActPosition;

            update();
        }        
      ctrlPressed = true;
    }
    return QGraphicsItem::keyPressEvent(event);
}

void SpPlayer::keyReleaseEvent( QKeyEvent * event )
{
    if(event->key() == Qt::Key_Shift)
    {
        if(ctrlPressed & actived)
        {
            QPointF mouseScreenPoint = field->mapFromGlobal(QCursor::pos());

            SpPoint * oriActPosition = new SpPoint(actualPosition);
            actualPosition->changeScreenPoint(mouseScreenPoint);

            if(currentBallHolder && taker && setPlay->isBallStopped()
                    && !field->isOnSpecialAreas(mouseScreenPoint))
            {
                setPlay->setStatusMessage(QString("Taker player can't leave the special area before put ball in moviment..."));
                setCursor(Qt::ForbiddenCursor);
                actualPosition = oriActPosition;
                setPos(oriActPosition->getScreenPoint());
            }
            else if(field->actualizePlayerPositions(this, false, false))
            {
                setPlay->clearStatusMessage();
                setCursor(Qt::ArrowCursor);
                setPos(mouseScreenPoint);
            }
            else
            {
                setPlay->setStatusMessage(QString("Player cann't move to that position, because relationships with him"));
                setCursor(Qt::ForbiddenCursor);
                actualPosition = oriActPosition;
                setPos(oriActPosition->getScreenPoint());
            }

            update();

        }
        ctrlPressed = false;

    }
    return QGraphicsItem::keyReleaseEvent(event);
}

QRectF SpPlayer::boundingRect() const
{
    return QRectF(-15,-15,45,45);
}

QPainterPath SpPlayer::shape() const
{
    QPainterPath path;
    path.addRect(-15, -15, 45, 45);
    return path;
}

void SpPlayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);

    if(currentBallHolder)
    {
        QRect target(10, -15, 20, 20);
        QPixmap image("./setPlayGUI/images/ball.png");
        painter->drawPixmap(target, image);
    }

    if(ghost || (!participant && setPlay->getCurrentStepNumber() != 0))
    {
        QRect target(-15, -15, 30, 30);
        QPixmap image("./setPlayGUI/images/tshirtGhost.png");
        painter->drawPixmap(target, image);
    }
    else if(!actived)
    {
        QRect target(-15, -15, 30, 30);
        QPixmap image("./setPlayGUI/images/tshirt.png");
        painter->drawPixmap(target, image);
    }
    else
    {
        QRect target(-15, -15, 30, 30);
        QPixmap image("./setPlayGUI/images/tshirtSelected.png");
        painter->drawPixmap(target, image);
    }


    QFont font;
    font.setBold(true);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black));
    painter->drawText(QRect(-15,-15,30,30), Qt::AlignCenter, QString("%1").arg(number));
    if(participant && !ghost && (actualPosition->isRelative() || actualPosition->isDummy()))
    {
        QFont font;
        font.setBold(true);
        painter->setFont(font);
        QString simb;
        if(actualPosition->isRelative() == SpPoint::ballRelative)
        {
             painter->setPen(QPen(Qt::darkBlue));
             simb = "Rb";
        }
        else if(actualPosition->isRelative() == SpPoint::playerRelative)
        {
            int n =  actualPosition->getRelativePlayer()->getNumber();
             painter->setPen(QPen(Qt::darkBlue));
             simb = "R" + QString::number(n);
        }
        else if(actualPosition->isDummy())
        {
            painter->setPen(QPen(Qt::darkMagenta));
            simb = "D";
        }
        painter->drawText(QRect(-20,-15,30,10), Qt::AlignCenter, simb);
    }
}

void SpPlayer::setPosition(SpPoint * p)
{
    delete lastPosition;
    lastPosition = actualPosition;
    actualPosition = p;
    actualPosition->actualize();
    setPos(actualPosition->getScreenPoint());
    field->itemMoved();
}


bool SpPlayer::calcNewPosition(bool movePlayer)
{
    if(ghost || !movePlayer)
        return true;

    SpPoint * nextPos = new SpPoint(actualPosition);
    nextPos->actualize();

    if(!field->isInsideField(nextPos->getScreenPoint()))
        return false;

    return true;
}

void SpPlayer::changePosition(bool movePlayer, bool final)
{
    if(ghost)
        return;

    if(final)
    {
        if(movePlayer)
        {
            delete lastPosition;
            lastPosition = new SpPoint(actualPosition);
            setScreenPosition(pos());
        }
        else
        {
            setPos(actualPosition->getScreenPoint());
            actualPosition->changeScreenPoint(pos());

        }
    }
    else
    {
        if(movePlayer)
        {
            SpPoint * oriActPosition = new SpPoint(actualPosition);
            actualPosition->actualize();
            //if(actualAction)
                //actualAction->actualize();


            setPos(actualPosition->getScreenPoint());
            delete actualPosition;
            actualPosition = oriActPosition;
        }
        else
        {
            SpPoint * oriActPosition = new SpPoint(actualPosition);
            setPos(actualPosition->getScreenPoint());
            actualPosition->changeScreenPoint(pos());

            delete actualPosition;
            actualPosition = oriActPosition;
        }
    }
}


bool SpPlayer::haveDeterministicPosition(int number)
{
    if(taker && setPlay->getBallHolderOnStep(number) == this)
        //ou true e é relativo à bola???
        return false;

    if(number == 0)
        return true;

    SpTransition * t = setPlay->getStep(number)->getDestTransitionsList()[0];
    int lastStep = t->getSource()->getIdNumber();

    SpAction *act = getActionByTransition(t->getIdNumber());
    if(act->getSpDest()->isRelative())
        return false;

    if(positionsByStep[lastStep]->isRelative())
        return false;

    return true;

}
bool SpPlayer::isGhostOnStep(int stepNumber)
{
    if(!participantOnTransitions.contains(stepNumber))
        return false;

    return ! participantOnTransitions[stepNumber].size();
}
bool SpPlayer::isGhostOnStep(SpStep *spStep)
{
    int stepNumber = spStep->getIdNumber();
    return isGhostOnStep(stepNumber);
}

bool SpPlayer::isGhostOnTrasitionOfStep(int stepNumber, int transitionNumber) const
{
    if(participantOnTransitions.contains(stepNumber))
        return ! participantOnTransitions[stepNumber].contains(transitionNumber);
    return false;
    //qDebug() << "ERRO is ghost on transition of step";
}



void SpPlayer::removeParticipantOnTransitionOfStep(int stepNumber, int transitionNumber)
{
    if ( participantOnTransitions.contains(stepNumber))
        participantOnTransitions[stepNumber].remove(transitionNumber);
    else
         qDebug() << "ERRO removeGhostOnTransitionOfStep";
}

void SpPlayer::setParticipantOnStepByTransition(int stepNumber, int transitionNumber)
{
    if(!participantOnTransitions.contains(stepNumber))
    {
        participantOnTransitions.insert(stepNumber, QSet<int>());
    }
    //else
      //   qDebug() << "ERRO setGhostOnStepByTransition";
    participantOnTransitions[stepNumber] << transitionNumber;
}


void SpPlayer::removeFromAllSetPlay()
{
    field->playerToBench(this);
    participant = false;
    ghost = false;
    lastPosition = NULL;
    actualAction =NULL;
    currentBallHolder = false;
    taker = false;
    ballOwnerOnStep.clear();
    participantOnTransitions.clear();
    positionsByStep.clear();
    conditionsByStep.clear();

    actionsByTransitions.clear();
    conditionsByTransitions.clear();
actived = false;
ctrlPressed = false;
moving  =false;

}

bool SpPlayer::canRemoveFromAllSetPlay()
{
    if(setPlay->getSteps().size() == 1)
        return true;

    foreach(SpStep *s, setPlay->getSteps())
    {
        if(participantOnTransitions.contains(s->getIdNumber()))
        {
            int participantOnNumberTransitions = participantOnTransitions[s->getIdNumber()].size();
            if (participantOnNumberTransitions)
            {
                if(s == setPlay->getCurrentStep() && participantOnNumberTransitions == 1
                    && participantOnTransitions[s->getIdNumber()].contains(setPlay->getCurrentTransitionNumber())
                    && field->canRemove(this))
                {
                    continue;
                }
                return false;
            }
        }
    }
    return true;
}
