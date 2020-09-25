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

#include "spsetplay.h"
#include "ui_spsetplay.h"


#include "spstep.h"
#include "spgraph.h"
#include "spaction.h"
#include "spplayer.h"
#include "sptransition.h"
#include "spinitdialog.h"

#include "mainwindow.h"
#include "spporting.h"
#include <fstream>
#include "QProcess"
#include "QStatusBar"
#include "spshowgame.h"
#include "spporting.h"
#include "spoutprogram.h"

SpSetPlay::SpSetPlay(MainWindow *parent) :
    QWidget(parent),
    ui(new Ui::SpSetPlay)
{
    ui->setupUi(this);

    spPorting = new SpPorting(this);
    spOutProgram = new SpOutProgram(this);

    contextBarMenu = new QMenu(tr("&Set Play"));

    QMenu * outProgramsMenu = spOutProgram->getMenu();
    connect(outProgramsMenu, SIGNAL(triggered(QAction*)), SLOT(run(QAction*)));
    contextBarMenu->addMenu(outProgramsMenu);

    QAction * showAction = new QAction(tr("&Show game log 2D"),this);
    showAction->setStatusTip("See 2D log game through RCSSlogplayer like interface.");
    showAction->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(showAction, SIGNAL(triggered()), this, SLOT(uploadLogFile()));
    contextBarMenu->addAction(showAction);

    ui->logToolBar->hide();
        mainWindow = (MainWindow *)parent;

    QIntValidator *inputRange = new QIntValidator(this);
    inputRange->setBottom(0);
    ui->id->setValidator(inputRange);

    ui->playerInfoFrame->hide();
    activePlayer = NULL;





    ui->abortConditionsList->clear();
    QListWidgetItem *loseBall = new QListWidgetItem("Opponent team gets ball possession");
    QListWidgetItem *getsBall = new QListWidgetItem("Our team gets ball possession");
    QListWidgetItem *playOff = new QListWidgetItem("Game stop or don't start as defined");

    allAbortConditions.insert(theirBallCond, loseBall);
    allAbortConditions.insert(ourBallCond, getsBall);
    allAbortConditions.insert(playOffCond, playOff);
    ui->abortConditionsList->addItem(playOff);
    if(type == off)
        ui->abortConditionsList->addItem(loseBall);
    else
        ui->abortConditionsList->addItem(getsBall);

    foreach(QListWidgetItem * item,
            ui->abortConditionsList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))

        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

}


SpSetPlay::~SpSetPlay()
{
    delete ui->field;
    delete ui->graph;

    foreach(SpPlayer *p , players)
        delete p;

    currentStep = NULL;
    currentTransition = NULL;
    currentBallHolderPlayer = NULL;
      spla = NULL;
    delete ui;
}

bool SpSetPlay::init( )
{
    SpInitDialog * sp = new SpInitDialog(this->mainWindow);

    bool teste = false; // TODO CLEAN
    if(!teste)
    if(sp->exec())
    {
        situation = sp->getSituation();
        type = sp->getType();
        regions = sp->getSpecialAreas();
        transverse_regions = sp->getSpecialTransverseAreas();
        sp->close();
    }
    else
        return false;
    else
    {
    situation = play_on;
    type = off;
    QList <int>  a;
    a.append(field);
    regions = a;
    }

    resetMembers();

    ui->graph->init(this);
    ui->field->init(this, situation, regions, transverse_regions);

    for(int i = 1; i < 12; i++)
    {
        players[i]->setName("Player" + QString::number(i) );
    }

    setName("newSetPlay");
    setIdNumber(1);

    comment = "";
    setInvertible(true);


    nextStepId = 0;
    nextTransitionId = 0;

    createStep(true);
    ballStoppedSteps << 0;

    int playerNo = rand() % 11 +1;
    ui->field->removePlayerFromBench(players[playerNo]);

    bool indeterminatedPosition = true;
    if( regions.contains(point) ||
        situation == corner_kick || situation == kick_off)
        indeterminatedPosition = false;

    players[playerNo]->moveToSpecialArea(indeterminatedPosition);
    changeCurrentHolderBallPlayer(players[playerNo]);
    ballHolderPlayers[0] = currentBallHolderPlayer;

    if(type == off)
       abortConditions.append(theirBallCond);
    else
        abortConditions.append(ourBallCond);

    abortConditions.append(playOffCond);


    update_abortConditions();
     spla = new SpShowGame(this);
    return true;
}

QList< SpPlayer *> SpSetPlay::getParticipantPlayers()
{
    QList< SpPlayer *> partPlayers;

    foreach(SpPlayer *p, players)
    {
        if(p->isParticipant())
        {
            foreach(SpStep *s, steps)
                if(!p->isGhostOnStep(s))
                {
                    partPlayers.append(p);
                    break;
                }
        }
    }
    return partPlayers;
}

void SpSetPlay::resetMembers()
{
    currentStep = NULL;
    currentTransition = NULL;
    currentBallHolderPlayer = NULL;
    players = QMap<int, SpPlayer *>() ;
    steps = QMap<int, SpStep *> ();

    for(int i = 1; i < 12; i++)
        players.insert(i, new SpPlayer(i, this));
}


bool SpSetPlay::open( )
{
    resetMembers();

    nextStepId = 0;
    nextTransitionId = 0;
    ui->field->init(this);
    ui->graph->init(this);

    spPorting = new SpPorting(this);
    if(!spPorting->importSetPlay())
        return false;

    ui->field->setSituationAndRegion(situation, regions, transverse_regions);

    if(ballHolderPlayers[0]->getStepPosition(0)->isDummy())
    {
        ballHolderPlayers[0]->getPosition()->changeScreenPoint(ui->field->getSpecialAreaPosition());
        ballHolderPlayers[0]->getPosition()->setDummy();
        ballHolderPlayers[0]->setPositionOnStep(0, ballHolderPlayers[0]->getPosition());
    }

    changeCurrentStep(steps[0],steps[0]->getActivedTransition());

    foreach (SpPlayer *p, ui->field->getInPlayers())
    {
        p->getPosition()->actualize();
        p->setPositionOnStep(0, p->getPosition());
    }
    getBallHolderOnStep(0)->setTaker(true);



   /* foreach(SpPlayer *p, getField()->getInPlayers())
    {
        p->updateStep(steps[0]);
        ui->playerTabInformation->updateName(p);
    }*/


    foreach(SpPlayer *p, getField()->getOutPlayers())
    {
        QString pname = "Player" + QString::number(p->getNumber());
        QString pnameAlt = pname;
        int i = 2;
        while(!p->setName(pnameAlt))
        {
            pnameAlt = pname + "_" + QString::number(i);
            i++;
        }

    }

    getBallHolderOnStep(0)->setTaker(true);

    QList<int> idNumbers = steps.keys();
    sort(idNumbers.begin(), idNumbers.end());
    nextStepId = idNumbers.last() +1;


    update_abortConditions();
               spla = new SpShowGame(this);
    return true;

}




void SpSetPlay::save()
{
#ifdef DEBUGSAVE
    cerr << "SpSetplay::save(): Enter!" << endl;//JAF2014
#endif

   if(!correctSetPlay())
        return;

#ifdef DEBUGSAVE
   if(currentStep!=NULL)
    cerr << "SpSetplay::save(): Going to Call saveFromStep for currentStep " << currentStep->getIdNumber();//JAF2014
   if(currentTransition!=NULL){
    cerr << " and currentTransition " << currentTransition->getIdNumber();//JAF2014
    cerr << " to  Step "<< currentTransition->getDest()->getIdNumber()<< endl;//JAF2014
   }
#endif

   saveFromStep(currentStep, currentTransition);

   // refresh from 0.
   foreach(SpPlayer *player, getField()->getInPlayers())
       player->updateStep(getStep(0));

    ballHolderPlayers[currentStep->getIdNumber()] = currentBallHolderPlayer;

    currentStep->setWaitTime(ui->waitTime->value());
    currentStep->setAbortTime(ui->abortTime->value());

    comment = ui->comment->toPlainText();

    QLineEdit * a; //JAF2014
    a = ui->id;//JAF2014
    QString idQString;//JAF2014
    idQString = a->text();//JAF2014

    idNumber = (unsigned int)idQString.toInt();//JAF2014 Conversion to int, then unsigned int
    if (idNumber==0) idNumber=1;//JAF2014 Default Number if problem with conversion(someone typed a letter in Id: field...)

#ifdef DEBUGSAVE
    cerr << "SpSetplay::save(): Going to Call save_abortConditions!" << endl;//JAF2014
#endif
    save_abortConditions();

#ifdef DEBUGSAVE
    cerr << "SpSetplay::save(): Going to Call spPorting->exportSetPlay()!" << endl;//JAF2014
#endif

    spPorting->exportSetPlay();
}

void SpSetPlay::changeCurrentHolderBallPlayer(SpPlayer *p)
{
    if(currentBallHolderPlayer != NULL)
    {
        currentBallHolderPlayer->takeTheBall();
        if(currentBallHolderPlayer->isTaker())
        {
            currentBallHolderPlayer->setTaker(false);
            if(p != NULL)
                p->setTaker(true);
        }
    }
    currentBallHolderPlayer = p;
    if(currentBallHolderPlayer != NULL)
        currentBallHolderPlayer->gaveTheBall();
}

SpStep* SpSetPlay::createStep(bool active)
{
    //TODO CHECK the id...
    SpStep * newStep = new SpStep(this, nextStepId);
    newStep->init();
    steps.insert(newStep->getIdNumber(), newStep);

    if(active)
    {
        changeCurrentStep(newStep, NULL);
    }
    return newStep;
}

void SpSetPlay::addTransition(SpStep *dest)
{
    currentStep->addSourceTransition(dest);
}


SpTransition * SpSetPlay::createTransition()
{
    SpStep * newStep = createStep(false);
    SpTransition *t = currentStep->addSourceTransition(newStep);
    currentTransition = t;
    currentStep->setActiveTransition(t);
    currentStep->active();
    return t;
}

void SpSetPlay::removeStep(SpStep *step)
{
    unsigned int stepNumber = step->getIdNumber();
    if(steps.contains(stepNumber))
        steps.remove(stepNumber);

    foreach (SpPlayer *p, players)
        p->removeStep(step->getIdNumber());

    if(steps[0])
        changeCurrentStep(steps[0], steps[0]->getActivedTransition());

    int nextId = stepNumber;
    foreach(SpStep *s , steps)
    {
        if(s->getIdNumber() < (unsigned int)nextId)
            continue;
        s->setIdNumber(nextId++);
    }
    nextStepId = nextId;



}

void SpSetPlay::setIdNumber(unsigned int idNumber)
{
    this->idNumber = idNumber;
    ui->id->setText(QString::number(idNumber));
}

void SpSetPlay::setInvertible(bool inv)
{
    this->invertible = inv;
    ui->invertible->setChecked(inv);
}

void SpSetPlay::setName(QString name)
{
    this->name = name;
    ui->name->setText(name);
}

void SpSetPlay::setComment(QString comment)
{
    this->comment = comment;
    ui->comment->setPlainText(comment);
}

void SpSetPlay::removeTransition(SpTransition *transition)
{
    foreach(SpPlayer *p, players)
        p->removeTransition(transition->getIdNumber());
    if(currentStep)
        currentTransition = currentStep->getActivedTransition();
}

void SpSetPlay::addStep(SpStep * s) {   steps.insert(s->getIdNumber(), s);}

int SpSetPlay::getCurrentStepNumber() const
{
     return currentStep->getIdNumber();
}

int SpSetPlay::getCurrentTransitionNumber() const
{
    if(currentTransition)
        return currentTransition->getIdNumber();
    else
        return -1;
}

void SpSetPlay::updateInexistentBallOwnerAfterStep(SpStep *spStep)
{
    int spStepIdNumber = spStep->getIdNumber();
    ballHolderPlayers[spStepIdNumber] = NULL;
    foreach(SpTransition * spTransition, spStep->getSourceTransitionsList())
    {
        SpStep *nextSpStep = spTransition->getDest();
        updateInexistentBallOwnerAfterStep(nextSpStep);
    }
}

void SpSetPlay::saveFromStep(SpStep *spStep, SpTransition *sptransition)
{
#ifdef DEBUGSAVE
    cerr << "  SpSetplay::saveFromStep(): Enter!" << endl;//JAF2014
#endif

    if(!spStep)
        return;
    int spStepIdNumber = spStep->getIdNumber();

#ifdef DEBUGSAVE
    cerr << "  SpSetplay::saveFromStep(): spStepIdNumber = " << spStepIdNumber << endl;//JAF2014
#endif
    // remove ballHolder to be reconfigured
    ballHolderPlayers[spStepIdNumber] = NULL;

#ifdef DEBUGSAVE
    cerr << "  SpSetplay::saveFromStep(): Going to call player->save(spStep, spTransition)!" << endl;//JAF2014
#endif

    // save players positions, actions and participant state on requested spStep and sptransition
    foreach(SpPlayer *player, getField()->getInPlayers())
    {
#ifdef DEBUGSAVE
        cerr << "  SpSetplay::saveFromStep(): Calling player->save(spStep, spTransition) for player " << player->getNumber() << endl;//JAF2014
#endif
        player->save(spStep, sptransition);
    }
    SpPlayer *ballHolder = ballHolderPlayers[spStepIdNumber];
#ifdef DEBUGSAVE
    cerr << "  SpPlayer::savefromStep(): Setting BallHolder!" << endl;//JAF2014
#endif

    // update ball owner from this step
    if (ballHolder)
        ballHolder->gaveTheBallFromStep(spStep);
    else    // no one have ball...
        updateInexistentBallOwnerAfterStep(spStep);

#ifdef DEBUGSAVE
    cerr << "  SpPlayer::savefromStep(): Going to call player->updateStep(spStep)!" << endl;//JAF2014
#endif

    // update steps that came after this spStep
    foreach(SpPlayer *player, getField()->getInPlayers())
    {
#ifdef DEBUGSAVE
        cerr << "  SpPlayer::savefromStep(): calling player->updateStep(spStep) for player " <<player->getNumber()<< endl;//JAF2014
#endif
        player->updateStep(spStep);
    }
}


void SpSetPlay::changeCurrentStep(SpStep *step, SpTransition *transition)
{
    if(!step || ( step == currentStep && transition == currentTransition))
        return;

    //Check if is all right with actual step.
    foreach(SpPlayer *player, getField()->getInPlayers())
    {
        SpAction *act = player->getAction();
        if(act)
        {
            SpPoint* poi = act->getSpDest();
            if(poi->isRelative() && !poi->isOnLimits())
            {
                QMessageBox::warning(this, "Step error",
                    "There are actions outside de field... Correct them first");
                return;
            }
        }
    }

    if (currentStep)
    {
        //Save
        saveFromStep(currentStep, currentTransition);

        ui->field->cleanField();

        currentStep->setWaitTime(ui->waitTime->value());
        currentStep->setAbortTime(ui->abortTime->value());
    }

    bool toChangeStep = currentStep && currentStep->getIdNumber() != step->getIdNumber();
    if(toChangeStep)
        currentStep->desactive();

    if(toChangeStep || !currentStep)
    {
        currentStep = step;
        currentStep->active();

        ui->stepDetailsLabel->setText("Step " + QString::number(currentStep->getIdNumber()) + " Times");
        ui->waitTime->setValue(step->getWaitTime());
        ui->abortTime->setValue(step->getAbortTime());
    }

    currentBallHolderPlayer = ballHolderPlayers[currentStep->getIdNumber()];

    currentTransition = transition;
    if(currentTransition )
        currentStep->setActiveTransition(transition);

    foreach(SpPlayer *player, players)
    {
        player->actualize(currentStep, currentTransition, currentBallHolderPlayer == player);

    }

    ui->abortTimeCB->setChecked(! currentStep->isAutomaticAbortTime());
    ui->field->actualize(currentStep);
    if(activePlayer)
    activePlayer->updateInformationFrame();
}


void SpSetPlay::changeCurrentTransition(SpTransition *transition)
{
    if(currentTransition != transition)
    {
        changeCurrentStep(transition->getSource(), transition);
        currentStep->setActiveTransition(transition);
    }
}

SpPlayer * SpSetPlay::getPlayer(QString name)
{
    foreach(SpPlayer *p , players)
        if(p->getName() == name)
            return p;
    return NULL;
}


void SpSetPlay::createStepByGraph(SpStep *s)
{
    //TODO CHECK the id...

    SpStep * newStep = createStep(false);
    SpTransition *t = s->addSourceTransition(newStep);

    changeCurrentStep(s,t);
}

void SpSetPlay::connectNodes(SpStep *s, int i)
{
    SpStep * destStep = getStep(i);
    if(destStep)
    {
        SpTransition *t = s->addSourceTransition(destStep);
        changeCurrentStep(s,t);
    }

}

void SpSetPlay::setBallHolderOnStep(int number, SpPlayer *p)
{
    if(p)
    {
        ballHolderPlayers[number] = p;
        p->addStepCondition(number, bowner);
    }
    else
    {
        ballHolderPlayers[number] = NULL;
    }
}

void SpSetPlay::changeAbortTime()
{
    if(currentStep->isAutomaticAbortTime())
    {
        foreach (SpPlayer *p, ui->field->getInPlayers())
            if(p->getAction())
               p->getAction()->calculateTime();
        int t = currentStep->calculateAbortTime();
        ui->abortTime->setValue(t);
    }
}

QMenu * SpSetPlay::getContextBarMenu()
{
    return contextBarMenu;

}



SpGraph *SpSetPlay::getGraph()
{
    return ui->graph;
}

QWidget * SpSetPlay::getLogToolBar()
{
    return  ui->logToolBar;
}

QFrame *SpSetPlay::getFrameField()
{
    return ui->frameField;
}


SpField *SpSetPlay::getField()
{
    return ui->field;
}

void SpSetPlay::resizeSPLA()
{
    if(spla)
        spla->resize();
}

void SpSetPlay::update_abortConditions()
{


    if(abortConditions.contains(theirBallCond))
        allAbortConditions[theirBallCond]->setCheckState(Qt::Checked);
    if(abortConditions.contains(ourBallCond))
        allAbortConditions[ourBallCond]->setCheckState(Qt::Checked);
    if(abortConditions.contains(playOffCond))
        allAbortConditions[playOffCond]->setCheckState(Qt::Checked);



}
void SpSetPlay::save_abortConditions()
{
    abortConditions.clear();
    foreach(int i, allAbortConditions.keys())
        if(allAbortConditions[i]->checkState() == Qt::Checked)
            abortConditions.append(i);

}

void SpSetPlay::on_name_lostFocus()
{
    QString t = ui->name->text();
    t.replace(' ', '_');
    ui->name->setText(t);
    name = t;
    if(t != "")
        mainWindow->changeTabName("SP: " + t);
    else
        mainWindow->changeTabName("Set Play editor");
}

void SpSetPlay::on_invertible_stateChanged(int inv)
{
    invertible = inv;
}

void SpSetPlay::on_abortTimeCB_stateChanged(int checked)
{
    ui->abortTime->setEnabled(checked);
    currentStep->setAutomaticAbortTime(!checked);
    if(!checked)
        changeAbortTime();
}

void SpSetPlay::on_waitTime_valueChanged(int value)
{
    if(currentStep->isAutomaticAbortTime())
    {
        int dif = value - currentStep->getWaitTime();
        int abortTime = currentStep->getAbortTime() + dif;
        currentStep->setAbortTime(abortTime);
        ui->abortTime->setValue(abortTime);
    }
    currentStep->setWaitTime(value);
}

bool SpSetPlay::correctSetPlay()
{
    if(name == "")
    {
        QMessageBox::warning(this, "Setplay without a name...",
                             "Define first a name for setplay...");
        return false;
    }

    if(steps.size() == 1)
    {
        QMessageBox::warning(this, "Setplay useless....",
                             "Make some transitions and steps first!...");

        return false;
    }

    foreach (SpPlayer *p, getParticipantPlayers())
    {
        if (!p->isIdentificatedByNumber() && p->getName() == "")
        {
            QMessageBox::warning(this, "Participant players unnamed...",
                                 "Type a name for players or identify them by number...");
            return false;
        }
    }

    //Check if is all right with actual step.
    foreach(SpPlayer *player, getField()->getInPlayers())
    {
        SpAction *act = player->getAction();
        if(act)
        {
            SpPoint* poi = act->getSpDest();
            if(poi->isRelative())
            {
                if(!poi->isOnLimits())
                {
                    QMessageBox::warning(this, "Step error",
                                         "There are actions outside de field... Correct them first");
                    return false;
                }
            }
        }
    }
    return true;
}

void SpSetPlay::run(QAction *a)
{
    //if(!correctSetPlay())
      //   return;

    saveFromStep(currentStep, currentTransition);

    // refresh from 0.
    foreach(SpPlayer *player, getField()->getInPlayers())
        player->updateStep(getStep(0));

     ballHolderPlayers[currentStep->getIdNumber()] = currentBallHolderPlayer;

     currentStep->setWaitTime(ui->waitTime->value());
     currentStep->setAbortTime(ui->abortTime->value());

     comment = ui->comment->toPlainText();

     spOutProgram->start(a->text());
}



void SpSetPlay::setStatusMessage(QString s)
{
    mainWindow->statusBar()->showMessage(s);
}

void SpSetPlay::clearStatusMessage()
{
    mainWindow->statusBar()->clearMessage();
}

void SpSetPlay::uploadLogFile()
{

   if( spla->openRCG())
   {
        ui->logToolBar->show();
        spla->resize();
   }
}

void SpSetPlay::closeSPLA()
{
    getLogToolBar()->setVisible(false);
}

void SpSetPlay::resizeEvent(QResizeEvent *event)
{
    resizeSPLA();
    event->accept();
}

void SpSetPlay::actualizePlayerInfo(QStringList pIdentification, QStringList pPosition, QStringList pAction)
{
    ui->editPlayerButton->setEnabled(true);
    ui->playerInfoIdentificationNumber->setText(
                QString(
                    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                    "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
                    "p, li { white-space: pre-wrap; }"
                    "</style></head><body style=\" font-family:'Ubuntu'; font-size:14pt; font-weight:400; font-style:normal;\">"
                    "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">"
                    "<tr><td style=\"border: none;\">"
                    "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                    "<span style=\" font-size:16pt; color:#000000;\">%1</span></p></td></tr></table></body></html>"
                        ).arg(pIdentification[0])
                    );
    ui->playerInfoIdentificationName->setText(
                QString(
                    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                    "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
                    "p, li { white-space: pre-wrap; }"
                    "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">"
                    "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">"
                    "<tr><td style=\"border: none;\">"
                    "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%2</p></td></tr></table></body></html>"
                        ).arg(pIdentification[1])
                    );

    if(pPosition.size())
    {
            ui->playerInfoPositionLabel->setText(
                    QString(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
        "p, li { white-space: pre-wrap; }"
        "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">"
        "<table style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">"
        "<tr><td style=\"border: none;\">"
        "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%1     X : %2 ; Y: %3</p>"
        "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%4</p>"
        "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>"
        "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%5</p></td></tr></table></body></html>"
        ).arg(pPosition[0], pPosition[1], pPosition[2], pPosition[3], pPosition[4]));


        if(pAction.size())
        {
                ui->playerInfoActionLabel->setText(
                        QString(
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
            "p, li { white-space: pre-wrap; }"
            "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">"
            "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">"
            "<tr><td style=\"border: none;\">"
            "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Action %1 </p>"
            "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">to X: %2;  Y: %3 %4</p>"
            "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>"
            "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%5</p></td></tr></table></body></html>"
            ).arg(pAction[0], pAction[2], pAction[3], pAction[4], pAction[5]));
        }
        else
            ui->playerInfoActionLabel->setText(" ... No Action ... ");
    }
    else
    {
        ui->playerInfoPositionLabel->setText(" Don't participate on step");
        ui->playerInfoActionLabel->setText(" ... ");
    }


}

void SpSetPlay::changeActivePlayer(SpPlayer * p)
{
    activePlayer = p;
}

void SpSetPlay::on_showPlayerInfoButton_clicked()
{
    if(ui->playerInfoFrame->isHidden())
    {
        ui->playerInfoFrame->show();
        ui->showPlayerInfoButton->setText("V");
        ui->showPlayerInfoButton->setToolTip("Hide Player Information");
    }
    else
    {
        ui->playerInfoFrame->hide();
        ui->showPlayerInfoButton->setText(trUtf8( "Λ" ));
        ui->showPlayerInfoButton->setToolTip("Show Player Information");
    }
}

void SpSetPlay::on_editPlayerButton_clicked()
{
    activePlayer->openConfigWindow();
}
