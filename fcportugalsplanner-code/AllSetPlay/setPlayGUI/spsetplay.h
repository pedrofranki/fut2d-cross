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

#ifndef SPSETPLAY_H
#define SPSETPLAY_H

#include <QWidget>
#include <QMap>
#include <QFileDialog>
#include <QMessageBox>
#include "setplay/setplay.h"
#include <QMenu>
#include <QFrame>
#include <QSet>
#include <QPushButton>
#include <QListWidgetItem>


//#define DEBUGSAVE 1 //JAF2014


class SpStep;
class SpGraph;
class SpField;
class SpPlayer;
class SpTransition;
class SpShowGame;
class SpPlayerTabInformation;

class SpPorting;
class SpOutProgram;

class MainWindow;

namespace Ui {
    class SpSetPlay;
}

class SpSetPlay : public QWidget
{
    Q_OBJECT

public:



    explicit SpSetPlay(MainWindow *parent);
    ~SpSetPlay();
    bool init();
    bool initForImport();


    MainWindow *getMainWindow() {return mainWindow;}


    static QMenu * getImportMenu (QWidget *parent);

    SpStep* createStep(bool active);
    void addStep(SpStep *);
    void removeStep(SpStep *step);

    SpTransition * createTransition();
    void addTransition(SpStep *dest); // TODO ??

    void removeTransition(SpTransition *transition);
    void resetMembers();

    void changeCurrentStep(SpStep * step, SpTransition *transition);
    void changeCurrentTransition(SpTransition *transition);

    void actionAdded();

    int getCurrentStepNumber() const;
    int getCurrentTransitionNumber() const;


    void resizeSPLA();
    //TODO VERIFIY IF EVERYTHING IS USED


    SpField *getField();
    SpGraph *getGraph();
    QWidget * getLogToolBar();
    QFrame *getFrameField();


    void changeCurrentHolderBallPlayer(SpPlayer *);

    void setIdNumber(unsigned int idNumber) ;
    void setInvertible(bool inv);
    void setName(QString name);
    void setComment(QString c);

    QList< SpPlayer *> getParticipantPlayers();

    SpPlayer * getPlayer(QString name);

    void createStepByGraph(SpStep *s);
    void connectNodes(SpStep *s, int i);


    void setBallHolderOnStep(int number, SpPlayer *p);
    void changeAbortTime();

    QMenu * getContextBarMenu();


    bool correctSetPlay();

    inline SpPlayer * getBallHolderOnStep(int number) { return ballHolderPlayers[number]; }
    inline SpPlayer * getCurrentBallHolderPlayer() { return currentBallHolderPlayer; }

    inline SpStep * getStep(int id) const { return steps.value(id); }
    inline QMap<int, SpPlayer *> getPlayers() { return players; }
    inline SpPlayer * getPlayer(unsigned int idNumber) { return players[idNumber]; }
    SpStep *getCurrentStep() const { return currentStep; }
    inline SpTransition *getCurrentTransition() const {return currentTransition; }
    inline QString getName() const { return name; }
    inline unsigned int getIdNumber() const { return idNumber; }
    inline QList<SpStep*> getSteps() const { return steps.values(); }
    inline QString getComment() const { return comment; }

    inline int getSituation() const { return situation; }
    inline int getType() const { return type; }
    inline bool isBallStopped() const { return ballStoppedSteps.contains( getCurrentStepNumber() ); }

    inline QList <int> getAbortConditions() const { return abortConditions; }
    inline void addToAbortConditions(int c) { if(!abortConditions.contains(c) )abortConditions.append(c); }
    inline void incrementNextStepId(int number) { nextStepId = number +1;}
    inline int getNextStepId() { return nextStepId;}
    inline QList <int> getRegions() { return regions;}
    inline QList <int> getTransverseRegions() { return transverse_regions;}
    inline bool ballStoppedOnStep(int i) const { return ballStoppedSteps.contains(i); }
    inline void setBallStoppedOnStep(int i) { ballStoppedSteps << i; }
    inline void removeBallStoppedOnStep(int i) {  ballStoppedSteps.remove(i); }
    inline void addRegion(int reg) { regions.push_back(reg); }
    inline void addTransverseRegion(int reg) { transverse_regions.push_back(reg); }
    inline void setSituation(int sit) { situation = sit; }
    inline void setType(int typ) { type = typ; }
    inline bool isInvertible() const {return invertible; }
    inline unsigned int getNextTransitionId() { return nextTransitionId++; }

    void setStatusMessage(QString s);
    void clearStatusMessage();
    void resizeEvent(QResizeEvent *event);

    void updateInexistentBallOwnerAfterStep(SpStep *spStep);
    bool open();

    void changeActivePlayer(SpPlayer * p);

    void save();
    void actualizePlayerInfo(QStringList pIdentification, QStringList pPosition, QStringList pAction );

    void update_abortConditions();
    void save_abortConditions();
    void closeSPLA();

private slots:

    void on_name_lostFocus();
    void on_invertible_stateChanged(int );
    void on_abortTimeCB_stateChanged(int );
    void on_waitTime_valueChanged(int );
    void run(QAction *);
    void uploadLogFile();

    void on_showPlayerInfoButton_clicked();

    void on_editPlayerButton_clicked();

private:
    Ui::SpSetPlay *ui;
    MainWindow *mainWindow;

    unsigned int idNumber;
    QString name;
    QString comment;
    bool invertible;
    QMap <int, QListWidgetItem*> allAbortConditions;
    QList <int> abortConditions;


    unsigned int situation;
    unsigned int type;
    QList <int> regions;
    QList <int> transverse_regions;


    QMap<int, SpStep *> steps;
    QMap<int, SpPlayer *> players;

    SpStep * currentStep;
    SpTransition *currentTransition;
    SpPlayer *currentBallHolderPlayer;
    QMap<int, SpPlayer *> ballHolderPlayers;    
    QSet <int> ballStoppedSteps;

    int nextStepId;
    int nextTransitionId;

    QMenu * contextBarMenu;
    SpShowGame *spla;
    SpPorting * spPorting;
    SpOutProgram *spOutProgram;

    SpPlayer *activePlayer;

    void saveFromStep(SpStep *spStep, SpTransition *sptransition);
};

#endif // SPSETPLAY_H

