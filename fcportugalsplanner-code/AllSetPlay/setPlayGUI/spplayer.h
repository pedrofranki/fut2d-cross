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

#ifndef SPPLAYER_H
#define SPPLAYER_H

#include <QDebug>
#include <QGraphicsItem>
#include <QPointF>
#include <QPainter>
#include <QStyleOption>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDialog>


#include "sppoint.h"

class SpStep;
class SpField;
class SpAction;
class SpSetPlay;
class SpTransition;

class SpPlayer : public QGraphicsItem
{
public:
    SpPlayer(int n,SpSetPlay * s, QString na = "");

    bool setName(QString n);

    void setPosition(SpPoint * p);
    void setRealPosition(QPointF pointf);
    void setScreenPosition(QPointF pointf);
    void moveToSpecialArea(bool forcedummy);
    void updatePosOnField(double scale);
    SpPoint * getNextPosition() const;

    bool haveDeterministicPosition(int number);

    void addAction(SpAction * action);
    void removeActualAction();
    void actualizePosition();
    QString printAction() const;

    bool calcNewPosition(bool movePlayer);
    void changePosition(bool movePlayer, bool final);

    void createParticipionTransitionSet(int stepIdNumber);
    void save(SpStep *step, SpTransition *transition);
    void actualize(SpStep *step, SpTransition *transition, bool b);

    void correctStep(SpStep *spStep);
    void updateStep(SpStep *spStep);
    void gaveTheBallFromStep(SpStep *spStep);

    void removeTransition(unsigned int number);
    void removeFromStep(int stepNumber);
    void removeStep(int stepNumber);
    void cameFromGhost();

    void gaveTheBall();
    void takeTheBall();

    bool hasStepCondition(int step, int condition) const;

    inline void setHighLight(bool highlight) { actived= highlight; update(); }

    inline QString getName() const { return name; }
    inline int getNumber() const { return number; }
    inline void setIdentificationByNumber(bool ibn) { identificatedByNumber = ibn; }
    inline bool isIdentificatedByNumber() const { return identificatedByNumber; }

    inline void setPositionOnStep(int IdNumber, SpPoint * p ) {positionsByStep[IdNumber] =  p; }
    inline SpPoint * getPosition() const { return actualPosition; }
    inline SpPoint * getStepPosition(unsigned int idNumber) const { return positionsByStep[idNumber]; };

    inline SpAction * getAction() const { return actualAction; }
    inline SpAction * getActionByTransition(unsigned int idNumber) const { return actionsByTransitions[idNumber]; };
    inline void addActionToTransition(SpAction *a, unsigned int t) { actionsByTransitions.insert(t,a); }

    inline bool isParticipant() const {  return participant; }
    inline bool isParticipantOnStep(unsigned int number) { return positionsByStep.contains(number); }

    inline bool isTheBallOwner() const { return currentBallHolder; }
    inline void setTaker(bool t) { taker = t; }
    inline bool isTaker() { return taker ; }

    inline QList<int> getStepConditions(int stepNumber) { return conditionsByStep[stepNumber]; }
    inline void clearStepConditions(int stepNumber) { conditionsByStep[stepNumber].clear(); }
    inline void addStepCondition(int stepNumber, int condition) { if(!conditionsByStep[stepNumber].contains(condition))
                                                                        conditionsByStep[stepNumber].append(condition); }


    inline void changeTransitionCondition(int transitionNumber, int condition) { conditionsByTransitions[transitionNumber] = condition; }
    inline void removeTransitionCondition(int transitionNumber) { conditionsByTransitions.remove(transitionNumber); }
    inline int getTransitionCondition(int transitionNumber) { return conditionsByTransitions.contains(transitionNumber) ?
                                                                     conditionsByTransitions[transitionNumber] : -1; }

    inline SpField * getField() { return field; }



    inline bool isGhost() const { return ghost; }
    void removeFromAllSetPlay();
    bool canRemoveFromAllSetPlay();

    bool isGhostOnStep(int id);
    bool isGhostOnStep(SpStep *spStep);

    bool isGhostOnTrasitionOfStep(int stepNumber, int transitionNumber ) const ;
    void removeParticipantOnTransitionOfStep(int stepNumber, int transitionNumber);
    void setParticipantOnStepByTransition(int stepNumber, int transitionNumber );
    void updateInformationFrame();
    void openConfigWindow();

private:
    const int number;
    QString name;

    SpPoint * actualPosition;
    SpPoint * lastPosition;

    SpAction *actualAction;

    SpField *field;

    SpSetPlay *setPlay;

    bool actived;
    bool ctrlPressed;

    bool currentBallHolder;
    bool participant;
    bool taker;
    bool ghost;

    bool identificatedByNumber;

    bool moving;

    QSet<int> ballOwnerOnStep;
    QMap<int, QSet<int> > participantOnTransitions;
    QMap<int, SpPoint *> positionsByStep;
    QMap<int, QList<int> > conditionsByStep;
    QMap<int, SpAction*> actionsByTransitions;
    QMap<int, int > conditionsByTransitions;


    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent( QKeyEvent * event );

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // SPPLAYER_H
