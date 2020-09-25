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

#include "spplayereditordialog.h"
#include "ui_spplayereditordialog.h"

#include "spplayer.h"
#include "spsetplay.h"
#include "spaction.h"
#include "spfield.h"
#include "sptransition.h"
#include "spstep.h"


SpPlayerEditorDialog::SpPlayerEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpPlayerEditorDialog)
{
    ui->setupUi(this);
    ui->actionX->setSingleStep(0.5);
    ui->actionY->setSingleStep(0.5);
    ui->positionX->setSingleStep(0.5);
    ui->positionY->setSingleStep(0.5);
    currentStepNumber = -1;
    currentTransitionNumber = -1;
    actionType = -1;   
    setWindowModality(Qt::ApplicationModal);


    spSetPlay = dynamic_cast<SpSetPlay*>(parent);
}

SpPlayerEditorDialog::~SpPlayerEditorDialog()
{
    delete ui;
}

void SpPlayerEditorDialog::init(SpPlayer *p, int openTab)
{
    player = p;

    lenght = spSetPlay->getField()->getLenght();
    width = spSetPlay->getField()->getWidth();
    this->setWindowTitle("Player editor: " + QString::number(p->getNumber()));

    ui->comboSelectRelativePlayerPosition->clear();
    ui->comboSelectActionRelativePlayer->clear();
    ui->comboSelectRelativePlayerPosition->addItem("Select a player...", -1);
    ui->comboSelectActionRelativePlayer->addItem("Select a player...", -1);
    foreach(SpPlayer *p, spSetPlay->getField()->getInPlayers())
    {
        QString identification = "No ";

        identification += QString::number(p->getNumber()) + "  -- " + p->getName();

        if(p->isGhost())
            identification += "     (ghost)";

        ui->comboSelectRelativePlayerPosition->addItem(identification,p->getNumber());
        ui->comboSelectActionRelativePlayer->addItem(identification,p->getNumber());

    }

    // identification
    ui->textEditPlayerName->setText(player->getName());
    ui->labelPlayerNumber->setText(QString::number(player->getNumber()));

    if(player->isIdentificatedByNumber())
    {
        ui->rbByNumber->setChecked(true);
        ui->rbByName->setChecked(false);
    }
    else
    {
        ui->rbByNumber->setChecked(false);
        ui->rbByName->setChecked(true);
    }


    // step position

    currentStepNumber = spSetPlay->getCurrentStepNumber();

    ui->stepPositionTitle->setText("Step " + QString::number(currentStepNumber) + " player position");

    if(!player->isTheBallOwner() || !spSetPlay->isBallStopped())
        ui->cbDummyPosition->setEnabled(false);
    else
        ui->cbDummyPosition->setEnabled(true);

    posPoint = new SpPoint(p->getPosition());
    ui->cbDummyPosition->setChecked(posPoint->isDummy());

    int relativePosition = posPoint->isRelative();
    if(relativePosition)
    {
        if(relativePosition == SpPoint::playerRelative)
        {
            ui->rbPositionRelativeToPlayer->setChecked(true);
            int i = ui->comboSelectRelativePlayerPosition->findData(posPoint->getRelativePlayer()->getNumber());
            ui->comboSelectRelativePlayerPosition->setCurrentIndex(i);

        }
        else if(relativePosition == SpPoint::ballRelative)
            ui->rbPositionRelativeToBall->setChecked(true);
        else if(relativePosition == SpPoint::pointRelative)
        {
            //TODO
        }

        ui->positionX->setValue(posPoint->getOffsetPoint().x());
        ui->positionY->setValue(posPoint->getOffsetPoint().y());
    }
    else
    {
        ui->rbPositionAbsolute->setChecked(true);
        ui->positionX->setValue(posPoint->getRealPoint().x());
        ui->positionY->setValue(posPoint->getRealPoint().y());
    }


    if(player->hasStepCondition(currentStepNumber, ppos))
        ui->onPositionCond->setChecked(true);
    if(player->hasStepCondition(currentStepNumber, nearOffsideLine))
        ui->nearOffsideLineCond->setChecked(true);
    if(player->isTheBallOwner())
    {
        if(player->hasStepCondition(currentStepNumber, bowner))
                ui->ballOwnerCond->setChecked(true);
    }
    else
        ui->ballOwnerCond->setDisabled(true);

    // transition action

    currentTransitionNumber = spSetPlay->getCurrentTransitionNumber();

    if( currentTransitionNumber != -1)
    {
        int nextStepNumber = spSetPlay->getCurrentTransition()->getDest()->getIdNumber();
        ui->transitionTitle->setText("Transition's action from " + QString::number(currentStepNumber) + " to " +
                                     QString::number(nextStepNumber));

        spAction = player->getAction();
        if(spAction)
        {
            ui->actionLabel->setText("Action:" + spAction->getName()); //TODO improve output
            actionType = spAction->getType();

            if( actionType != SpAction::hold && actionType != SpAction::wait )
            {

                if(actionType == SpAction::pass || actionType == SpAction::shoot)
                {
                    ui->actionPositionFrame->setEnabled(false);

                }
                else
                {
                    actPoint = new SpPoint(spAction->getSpDest());

                    int actRelative = actPoint->isRelative();
                    if(!actRelative)
                    {
                        ui->rbActionAbsolute->setChecked(true);
                        ui->actionX->setValue(actPoint->getRealPoint().x());
                        ui->actionY->setValue(actPoint->getRealPoint().y());
                    }
                    else
                    {
                        if(relativePosition == SpPoint::playerRelative)
                        {
                            ui->rbActionRelativeToPlayer->setChecked(true);
                            int i = ui->comboSelectActionRelativePlayer->findData(actPoint->getRelativePlayer()->getNumber());
                            ui->comboSelectActionRelativePlayer->setCurrentIndex(i);
                        }
                        else if(relativePosition == SpPoint::ballRelative)
                            ui->rbActionRelativeToBall->setChecked(true);
                        else if(relativePosition == SpPoint::pointRelative)
                        {
                            //TODO
                        }
                        ui->actionX->setValue(actPoint->getOffsetPoint().x());
                        ui->actionY->setValue(actPoint->getOffsetPoint().y());
                    }
                }

                if( actionType == SpAction::pass || actionType == SpAction::passForward ||
                     actionType == SpAction::shoot )
                {
                    int hasCondition = player->getTransitionCondition(currentTransitionNumber);
                    if(hasCondition != -1)
                        ui->cbActionCondition->setChecked(hasCondition);
                    ui->cbActionCondition->setText("Can " + spAction->getName());
                }
                else
                {
                    ui->actionPositionFrame->setEnabled(false);
                    ui->cbActionCondition->setDisabled(true);
                    ui->cbActionCondition->setText("No conditions available... ");
                }
            }
            else
                ui->actionPositionFrame->setEnabled(false);
        }
        else
            ui->playerEditorTabs->setTabEnabled(2,false);
    }
    else
        ui->playerEditorTabs->setTabEnabled(2,false);

    ui->playerEditorTabs->setCurrentIndex(openTab);
}

void SpPlayerEditorDialog::on_cancelButton_clicked()
{
    close();
}

void SpPlayerEditorDialog::on_saveButton_clicked()
{
    // PLAYER IDENTIFICATION
    if(! player->setName(ui->textEditPlayerName->text()))
    {
        QMessageBox::warning(this, "Invalid name",
                "It can't have two players with same name!");
        ui->textEditPlayerName->setText(player->getName());
        return;
    }

    //STEP

    // PLAYER POSITION

    if(ui->rbPositionAbsolute->isChecked())
        posPoint->setPositional();
    else if(ui->rbPositionRelativeToBall->isChecked())
        posPoint->setRelative(SpPoint::ballRelative);
    else if(ui->rbPositionRelativeToPlayer->isChecked())
    {
        int i = ui->comboSelectRelativePlayerPosition->currentIndex();
        if(i == 0)
        {
            QMessageBox::warning(this, "Can't save", "Relative player in step position isn't selected!");
                return;
        }
        else
        {
            int playerNumber =  ui->comboSelectRelativePlayerPosition->itemData(i).toInt();
            posPoint->setRelativePlayer(spSetPlay->getPlayer(playerNumber),player);
        }
    }
    if(ui->cbDummyPosition->isChecked())
        posPoint->setDummy();

    if(posPoint->isRelative())
        posPoint->changeOffsetPoint(QPointF(ui->positionX->value(), ui->positionY->value()));
    else
        posPoint->changeRealPoint(QPointF(ui->positionX->value(), ui->positionY->value()));

    SpField * f = player->getField();
    QPointF proposedRealPoint = posPoint->getFinalPoint();
    QPointF proposedScreenPos(f->screenX(proposedRealPoint.x()), f->screenY(proposedRealPoint.y()));

    if(!f->isInsideField(proposedScreenPos))
    {
        QMessageBox::warning(this, "Can't move player to position selected",
                "Reason: his realocation will move him outside of the field.");
        return;
    }

    // TRANSITION
    if( currentTransitionNumber  != -1)
    {
        // ACTION
        if(actionType != -1)
        {
            if( actionType != SpAction::hold && actionType != SpAction::wait &&
                actionType != SpAction::pass && actionType != SpAction::shoot)
            {
                if(ui->rbActionAbsolute->isChecked())
                    actPoint->setPositional();
                else if(ui->rbActionRelativeToBall->isChecked())
                    actPoint->setRelative(SpPoint::ballRelative);
                else if(ui->rbActionRelativeToPlayer->isChecked())
                {
                    int i = ui->comboSelectActionRelativePlayer->currentIndex();
                    if(i == 0)
                    {
                        QMessageBox::warning(this, "Can't save", "Relative player in action position isn't selected!");
                            return;
                    }
                    else
                    {
                        int playerNumber =  ui->comboSelectActionRelativePlayer->itemData(i).toInt();
                        actPoint->setRelativePlayer(spSetPlay->getPlayer(playerNumber),player);
                    }
                }

                if(actPoint->isRelative())
                    actPoint->changeOffsetPoint(QPointF(ui->actionX->value(), ui->actionY->value()));
                else
                    actPoint->changeRealPoint(QPointF(ui->actionX->value(), ui->actionY->value()));


                proposedRealPoint = actPoint->getFinalPoint();
                QPointF proposedScreenPos(f->screenX(proposedRealPoint.x()), f->screenY(proposedRealPoint.y()));

                if(!f->isInsideField(proposedScreenPos))
                {
                    QMessageBox::warning(this, "Can't move action to position selected",
                            "Reason: her realocation will move her to outside the field.");
                    return;
                }

                spAction->setSpDest(actPoint);
                if( actionType == SpAction::passForward)
                    spAction->getDestActor()->getAction()->setSpDest(actPoint);
            }


            // TRANSITION CONDITION
            if(ui->cbActionCondition->isChecked())
            {
                if(actionType == SpAction::pass)
                    player->changeTransitionCondition(currentTransitionNumber, canPassPlayer);
                if(actionType == SpAction::passForward)
                    player->changeTransitionCondition(currentTransitionNumber, canPassRegion);
                if(actionType == SpAction::shoot )
                    player->changeTransitionCondition(currentTransitionNumber, canShoot);
            }
            else
                player->removeTransitionCondition(currentTransitionNumber);
        }
    }

    player->setIdentificationByNumber(ui->rbByNumber->isChecked());
    player->setPosition(posPoint);


    // PLAYER STEP CONDITIONS
    player->clearStepConditions(currentStepNumber);
    if(ui->onPositionCond->isChecked())
        player->addStepCondition(currentStepNumber, ppos);
    if(ui->nearOffsideLineCond->isChecked())
        player->addStepCondition(currentStepNumber, nearOffsideLine);
    if(ui->ballOwnerCond->isChecked())
        player->addStepCondition(currentStepNumber, bowner);


    player->updateInformationFrame();
    close();
}

void SpPlayerEditorDialog::on_resetButton_clicked()
{
    init(player, ui->playerEditorTabs->currentIndex());
}




void SpPlayerEditorDialog::on_comboSelectRelativePlayerPosition_activated(int index)
{
    if(ui->comboSelectRelativePlayerPosition->itemData(index) == -1)
        ui->rbPositionRelativeToPlayer->setChecked(false);
    else
        ui->rbPositionRelativeToPlayer->setChecked(true);
}

void SpPlayerEditorDialog::on_comboSelectActionRelativePlayer_activated(int index)
{
    if(ui->comboSelectActionRelativePlayer->itemData(index) == -1)
        ui->rbActionRelativeToPlayer->setChecked(false);
    else
        ui->rbActionRelativeToPlayer->setChecked(true);
}

void SpPlayerEditorDialog::on_rbPositionAbsolute_clicked(bool checked)
{
    ui->cbDummyPosition->setChecked(false);
}

void SpPlayerEditorDialog::on_playerEditorTabs_currentChanged(int index)
{
    //TODO

    // para a primeira tab.. verificar se tem nome se selecionado o nome

    // para as outras, se selecionado o player relative tem o jogador
                    //se posicao final esta dentro do limite
}
