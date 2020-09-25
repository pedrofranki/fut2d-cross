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

#ifndef SPFCPORTUGALPORTING_H
#define SPFCPORTUGALPORTING_H

#include <QString>
#include <QDebug>
#include <QPointF>
#include <setplay/setplay.h>

#include "spporting.h"

class SpPoint;
class SpStep;
class SpField;
class SpPlayer;
class SpAction;
class SpTransition;
class SpSetPlay;


class SpFCPortugalPorting : public ISpPorting
{

public:
    SpFCPortugalPorting(SpSetPlay *ssp);

    bool importSetPlay(QString filename);
    bool exportSetPlay(QString filename);
    bool exportSetPlay();
    inline QString getError() const {return error ;}

private:
    SpSetPlay * spSetPlay;
    fcportugal::setplay::Setplay * fcpsetplay;
    QString error;

    static QString lastImportDirectory;



    // exporting

    bool generatePlayers();
    fcportugal::setplay::Cond * makeAbortCondition();
    bool generateStepParticipants(fcportugal::setplay::Step *step);
    bool generateStepConditions(SpStep * spStep, fcportugal::setplay::Step *step);
    bool generateStepLeadPlayer(fcportugal::setplay::Step *step);
    bool generateTransitions(SpStep * spStep, fcportugal::setplay::Step *step);
    SpTransition * generateTransition(fcportugal::setplay::Step * s, fcportugal::setplay::Transition *t);
    bool generateAbortTransition(fcportugal::setplay::Step * step);
    bool generateEndTransition(fcportugal::setplay::Step * step);
    bool generateConditionsToTransition(SpTransition * spTransition, fcportugal::setplay::NextStep * transitionNextStep,
                                         fcportugal::setplay::Step *step);
    bool generateDirectivesToTransition(SpTransition *spTransition,
                                         fcportugal::setplay::NextStep *transitionNextStep,
                                         fcportugal::setplay::Step *step);
    fcportugal::setplay::Directive * generateDirective(fcportugal::setplay::PlayerReference *playerReference,
                                                       vector<fcportugal::setplay::Action*> *actions);
    fcportugal::setplay::Action * generateAction(SpAction *spAction);
    fcportugal::setplay::PlayerReference * spPlayerToPlayerReference(SpPlayer *sp);
    rcss::clang::PlayMode situationToPlayMode(int situation, int type);
    fcportugal::setplay::RegPoint * qpointToRegPoint(QPointF pos);
    fcportugal::setplay::Region * spPointToRegion(SpPoint *sp);

    // importing
    bool importingSpSetPlayInformation();
    bool generateSpAbortConditions();
    bool generateSpSteps();
    bool generateSpStepConditions(fcportugal::setplay::Step *step, SpStep *spStep);
    bool generateSpTransitions();
    bool generateSpTransitionConditions(fcportugal::setplay::Transition *transaction, SpTransition *spTransation);
    bool generateSpActions(fcportugal::setplay::Transition *t , SpTransition * spTransition);
    bool generateSpAction(SpPlayer *p, SpTransition *t , fcportugal::setplay::Action *act);
    bool generateSpPassForward(SpPlayer *p, SpTransition *t , fcportugal::setplay::Action *act, QList<SpPlayer *>);
    SpPlayer * playerReferenceToSpPlayer(const fcportugal::setplay::PlayerReference * pr);//JAF2014 inserted const
    void playmodeToSituation(rcss::clang::PlayMode pm, int &situation, int &type);
    SpPoint * regionToSpPoint(const fcportugal::setplay::Region * r);
    inline QPointF toQPointF(const fcportugal::setplay::PointSimple * p) {return QPointF(p->getX()->value(), p->getY()->value()); }
};

//#define DEBUGEXPORT 0

#endif // SPFCPORTUGALPORTING_H
