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

#include "spinitdialog.h"
#include "ui_spinitdialog.h"
#include "spdefinitions.h"
#include "spsetplay.h"

#include <QDebug>
#include <QFormLayout>


SpInitDialog::SpInitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpInitDialog)
{
    ui->setupUi(this);

    ui->situationComboBox->insertItem(-1, "Select a type...");
    ui->situationComboBox->addItem("Play on");
    ui->situationComboBox->addItem("Kick off");
    ui->situationComboBox->addItem("Throw in");
    ui->situationComboBox->addItem("Direct free kick");
    ui->situationComboBox->addItem("Indirect free kick");
    ui->situationComboBox->addItem("Corner kick");
    ui->situationComboBox->addItem("Goal kick");
    ui->situationComboBox->addItem("Keeper catch");
}

SpInitDialog::~SpInitDialog()
{
    delete ui;
}

void SpInitDialog::on_cancelButton_clicked()
{
     reject();
}

void SpInitDialog::on_createButton_clicked()
{
    type = ui->offensiveTypeRB->isChecked() ? off : def;
    situation = ui->situationComboBox->currentIndex() - 1;
    specialAreas.clear();

    if(!ui->offensiveTypeRB->isChecked() && !ui->defensiveTypeRB->isChecked())
    {
        QMessageBox::warning(this, "No type selected",
                             "Choose if the setplay is on offensive or defensive situation.");
    }
    else if(ui->situationComboBox->currentIndex() == 0)
    {
        QMessageBox::warning(this, "No situation selected",
                             "Choose the setplay's trigger situation.");
    }
    else if(ui->positionGroupBox->isEnabled())
    {
        if(ui->pointBox->isChecked())
            specialAreas.append(point);
        else
        {
            if(ui->fieldBox->isChecked())
                specialAreas.append(field);
            else
            {
                if(ui->ourMiddleField->isChecked())
                    specialAreas.append(our_middle_field);
                else
                {
                    if(ui->ourBack->isChecked())
                        specialAreas.append(our_back);
                    if(ui->ourMiddle->isChecked())
                        specialAreas.append(our_middle);
                    if(ui->ourFront->isChecked())
                        specialAreas.append(our_front);
                }

                if(ui->theirMiddleField->isChecked())
                    specialAreas.append(their_middle_field);
                else
                {
                    if(ui->theirFront->isChecked())
                        specialAreas.append(their_front);
                    if(ui->theirMiddle->isChecked())
                        specialAreas.append(their_middle);
                    if(ui->theirBack->isChecked())
                        specialAreas.append(their_back);
                }

                if(ui->leftCB->isChecked()){
                    //specialAreas.append(left_region);
                    specialTransverseAreas.append(left_region);//JAF2014
                }
                else
                {
                    if(ui->farLeft->isChecked()){
                        //specialAreas.append(far_left);
                        specialTransverseAreas.append(far_left);//JAF2014
                    }
                    if(ui->midLeft->isChecked()){
                        //specialAreas.append(mid_left);
                        specialTransverseAreas.append(mid_left);//JAF2014
                    }
                    if(ui->cenLeft->isChecked()){
                        //specialAreas.append(centre_left);
                        specialTransverseAreas.append(centre_left);//JAF2014
                    }
                }

                if(ui->rightCB->isChecked()){
                    //specialAreas.append(right_region);
                    specialTransverseAreas.append(right_region);
                }
                else
                {
                    if(ui->cenRight->isChecked()){
                        //specialAreas.append(centre_right);
                        specialTransverseAreas.append(centre_right);//JAF2014
                    }
                    if(ui->midRight->isChecked()){
                        //specialAreas.append(mid_right);
                        specialTransverseAreas.append(mid_right);//JAF2014
                    }
                    if(ui->farRight->isChecked()){
                        //specialAreas.append(far_right);
                        specialTransverseAreas.append(far_right);//JAF2014
                    }
                }
            }
        }

        if(situation == throw_in &&
            (   !specialTransverseAreas.contains(left_region) &&
                !specialTransverseAreas.contains(right_region) ) )
        {
            if(ui->leftCB->isChecked()){
                specialTransverseAreas.append(left_region);
            }
            else if(ui->rightCB->isChecked())
                specialTransverseAreas.append(right_region);
        }

        if((specialAreas.size() == 0)&&(situation != throw_in) && (situation != corner_kick))
        {
            QMessageBox::warning(this, "No positions selected",
                                 "Choose the position where the setplay starts.");
        }
        else if(situation == throw_in || situation == corner_kick)
        {
           if(specialTransverseAreas.contains(right_region) || specialTransverseAreas.contains(left_region))
           {
               if(situation == throw_in && specialAreas.size() < 1)
               {
                   QMessageBox::warning(this, "No positions selected for Throw_in",
                                        "Choose the position where the setplay starts.");
               }
               else
                   accept();
           }
           else
           {
               QMessageBox::warning(this, "No side selected",
                                    "Choose left or right position where the setplay starts.");
           }
        }
        else
        {
            accept();
        }
    }
    else
    {
        accept();
    }
}

void SpInitDialog::on_situationComboBox_currentIndexChanged(QString s)
{
    situation = ui->situationComboBox->currentIndex() - 1;

    ui->horizontalSides->setDisabled(false);
    ui->verticalSides->setDisabled(false);
    ui->sixHorizontalAreas->setDisabled(false);
    ui->sixVerticalAreas->setDisabled(false);
    ui->pointBox->setDisabled(false);
    ui->fieldBox->setDisabled(false);

    if( s == "Select a type..."  || s == "Kick off" ||
        s == "Goal kick" || s == "Keeper catch" )
    {
        ui->positionGroupBox->setEnabled(false);
    }
    else if (s == "Throw in")
    {
        ui->positionGroupBox->setEnabled(true);
        ui->sixVerticalAreas->setDisabled(true);

        ui->leftCB->setChecked(false);
        ui->rightCB->setChecked(false);
        ui->pointBox->setChecked(false);
        ui->fieldBox->setChecked(false);
    }
    else if (s == "Play on" || s == "Direct free kick" || s == "Indirect free kick")
    {
        ui->positionGroupBox->setEnabled(true);
    }
    else if (s == "Corner kick")
    {
        ui->positionGroupBox->setEnabled(true);

        ui->sixVerticalAreas->setDisabled(true);
        ui->horizontalSides->setDisabled(true);
        ui->sixHorizontalAreas->setDisabled(true);
        ui->pointBox->setDisabled(true);
        ui->fieldBox->setDisabled(true);

        ui->leftCB->setChecked(false);
        ui->rightCB->setChecked(false);
        ui->pointBox->setChecked(false);
        ui->fieldBox->setChecked(false);
    }
}


void SpInitDialog::on_fieldBox_stateChanged(int checked)
{
    if(situation != throw_in )
        ui->verticalSides->setDisabled(checked);
    else
        ui->verticalSides->setDisabled(false);
    ui->horizontalSides->setDisabled(checked);

    if(checked)
    {
        ui->farLeft->setDisabled(true);
        ui->midLeft->setDisabled(true);
        ui->cenLeft->setDisabled(true);
        ui->farRight->setDisabled(true);
        ui->midRight->setDisabled(true);
        ui->cenRight->setDisabled(true);
        ui->ourBack->setDisabled(true);
        ui->ourMiddle->setDisabled(true);
        ui->ourFront->setDisabled(true);
        ui->theirBack->setDisabled(true);
        ui->theirMiddle->setDisabled(true);
        ui->theirFront->setDisabled(true);
    }
    else
    {      
        ui->farLeft->setDisabled(ui->leftCB->isChecked());
        ui->midLeft->setDisabled(ui->leftCB->isChecked());
        ui->cenLeft->setDisabled(ui->leftCB->isChecked());
        ui->farRight->setDisabled(ui->rightCB->isChecked());
        ui->midRight->setDisabled(ui->rightCB->isChecked());
        ui->cenRight->setDisabled(ui->rightCB->isChecked());

        ui->ourBack->setDisabled(ui->ourMiddleField->isChecked());
        ui->ourMiddle->setDisabled(ui->ourMiddleField->isChecked());
        ui->ourFront->setDisabled(ui->ourMiddleField->isChecked());
        ui->theirBack->setDisabled(ui->theirMiddleField->isChecked());
        ui->theirMiddle->setDisabled(ui->theirMiddleField->isChecked());
        ui->theirFront->setDisabled(ui->theirMiddleField->isChecked());
    }
}

void SpInitDialog::on_pointBox_stateChanged(int checked)
{
    ui->fieldBox->setDisabled(checked);

    if(checked)
    {
        if(situation != throw_in )
            ui->verticalSides->setDisabled(true);
        else
            ui->verticalSides->setDisabled(false);
        ui->horizontalSides->setDisabled(true);

        ui->farLeft->setDisabled(true);
        ui->midLeft->setDisabled(true);
        ui->cenLeft->setDisabled(true);
        ui->farRight->setDisabled(true);
        ui->midRight->setDisabled(true);
        ui->cenRight->setDisabled(true);
        ui->ourBack->setDisabled(true);
        ui->ourMiddle->setDisabled(true);
        ui->ourFront->setDisabled(true);
        ui->theirBack->setDisabled(true);
        ui->theirMiddle->setDisabled(true);
        ui->theirFront->setDisabled(true);
    }
    else
    {
        bool fieldChecked = ui->fieldBox->isChecked();
        ui->horizontalSides->setDisabled(fieldChecked);
        ui->verticalSides->setDisabled(fieldChecked);

        ui->farLeft->setDisabled(fieldChecked || ui->leftCB->isChecked());
        ui->midLeft->setDisabled(fieldChecked || ui->leftCB->isChecked());
        ui->cenLeft->setDisabled(fieldChecked || ui->leftCB->isChecked());
        ui->farRight->setDisabled(fieldChecked || ui->rightCB->isChecked());
        ui->midRight->setDisabled(fieldChecked || ui->rightCB->isChecked());
        ui->cenRight->setDisabled(fieldChecked || ui->rightCB->isChecked());

        ui->ourBack->setDisabled(fieldChecked || ui->ourMiddleField->isChecked());
        ui->ourMiddle->setDisabled(fieldChecked || ui->ourMiddleField->isChecked());
        ui->ourFront->setDisabled(fieldChecked || ui->ourMiddleField->isChecked());
        ui->theirBack->setDisabled(fieldChecked || ui->theirMiddleField->isChecked());
        ui->theirMiddle->setDisabled(fieldChecked || ui->theirMiddleField->isChecked());
        ui->theirFront->setDisabled(fieldChecked || ui->theirMiddleField->isChecked());
    }
}

void SpInitDialog::on_ourMiddleField_stateChanged(int checked)
{
    ui->ourBack->setDisabled(checked);
    ui->ourMiddle->setDisabled(checked);
    ui->ourFront->setDisabled(checked);
}

void SpInitDialog::on_theirMiddleField_stateChanged(int checked)
{
    ui->theirBack->setDisabled(checked);
    ui->theirMiddle->setDisabled(checked);
    ui->theirFront->setDisabled(checked);
}

void SpInitDialog::on_rightCB_clicked(bool checked)
{
    if(situation == throw_in || situation == corner_kick)
        ui->leftCB->setChecked(false);
    else
    {
        ui->farRight->setDisabled(checked);
        ui->midRight->setDisabled(checked);
        ui->cenRight->setDisabled(checked);
    }
}

void SpInitDialog::on_leftCB_clicked(bool checked)
{
    if(situation == throw_in || situation == corner_kick)
        ui->rightCB->setChecked(false);
    else
    {
        ui->farLeft->setDisabled(checked);
        ui->midLeft->setDisabled(checked);
        ui->cenLeft->setDisabled(checked);
    }
}
