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

#include "sptestrcsoccersimconfigdialog.h"
#include "ui_sptestrcsoccersimconfigdialog.h"
#include <QFileDialog>
#include <QDebug>


QString SpTestRCSoccerSimConfigDialog::logFilesDirectory = "./files/setPlaylogFiles";
QString SpTestRCSoccerSimConfigDialog::simulatorExecutable = "/usr/local/bin/rcsoccersim";

QString SpTestRCSoccerSimConfigDialog::teamDirectory = "~/fcportugal";
QString SpTestRCSoccerSimConfigDialog::teamScript = "~/fcportugal/start.sh";
QString SpTestRCSoccerSimConfigDialog::teamSetPlayFile = "~/fcportugal/setplay.conf";

QString SpTestRCSoccerSimConfigDialog::opponentDirectory = "~/agent2D-3.1.1";
QString SpTestRCSoccerSimConfigDialog::opponentScript = "~/agent2D-3.1.1/start.sh";
bool SpTestRCSoccerSimConfigDialog::opponentGroupActive = true;


SpTestRCSoccerSimConfigDialog::SpTestRCSoccerSimConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpTestRCSoccerSimConfigDialog)
{
    ui->setupUi(this);

    on_resetButton_clicked();
}

SpTestRCSoccerSimConfigDialog::~SpTestRCSoccerSimConfigDialog()
{
    delete ui;
}

void SpTestRCSoccerSimConfigDialog::on_logFilesDirectoryButton_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                QString("Directory for log files..."),
                ui->logFilesDirectoryPathLine->text());

    if(filePath.isEmpty())
       return;

    ui->logFilesDirectoryPathLine->setText(filePath);
}

void SpTestRCSoccerSimConfigDialog::on_simulatorExecutableButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                QString("Simulator executable..."),
                ui->simulatorExecutablePathLine->text());

    if(filePath.isEmpty())
       return;

    ui->simulatorExecutablePathLine->setText(filePath);
}

void SpTestRCSoccerSimConfigDialog::on_teamDirectoryButton_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                QString("Team directory..."),
                ui->teamDirectoryPathLine->text());

    if(filePath.isEmpty())
       return;

    ui->teamDirectoryPathLine->setText(filePath);
}

void SpTestRCSoccerSimConfigDialog::on_teamScriptButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                QString("Team script..."),
                ui->teamScriptPathLine->text());

    if(filePath.isEmpty())
       return;

    ui->teamScriptPathLine->setText(filePath);
}



void SpTestRCSoccerSimConfigDialog::on_teamSetPlayFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                QString("Team setplay file to substitute..."),
                ui->teamSetPlayFilePathLine->text());

    if(filePath.isEmpty())
       return;

    ui->teamSetPlayFilePathLine->setText(filePath);
}

void SpTestRCSoccerSimConfigDialog::on_opponentDirectoryButton_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                QString("Opponent team directory..."),
                ui->opponentDirectoryPathLine->text());

    if(filePath.isEmpty())
       return;

    ui->opponentDirectoryPathLine->setText(filePath);
}

void SpTestRCSoccerSimConfigDialog::on_opponentScriptButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                QString("Opponent team script..."),
                ui->opponentScriptPathLine->text());

    if(filePath.isEmpty())
       return;

    ui->opponentScriptPathLine->setText(filePath);
}


void SpTestRCSoccerSimConfigDialog::on_cancelButton_clicked()
{
    reject();
}

void SpTestRCSoccerSimConfigDialog::on_resetButton_clicked()
{
    ui->logFilesDirectoryPathLine->setText(logFilesDirectory);
    ui->simulatorExecutablePathLine->setText(simulatorExecutable);

    ui->teamDirectoryPathLine->setText(teamDirectory);
    ui->teamScriptPathLine->setText(teamScript);
        ui->teamSetPlayFilePathLine->setText(teamSetPlayFile);

    ui->opponentDirectoryPathLine->setText(opponentDirectory);
    ui->opponentScriptPathLine->setText(opponentScript);

    ui->opponentGroup->setChecked(opponentGroupActive);
}

void SpTestRCSoccerSimConfigDialog::on_testButton_clicked()
{
    logFilesDirectory = ui->logFilesDirectoryPathLine->text();
    simulatorExecutable = ui->simulatorExecutablePathLine->text();

    teamDirectory = ui->teamDirectoryPathLine->text();
    teamScript = ui->teamScriptPathLine->text();
    teamSetPlayFile = ui->teamSetPlayFilePathLine->text();

    opponentDirectory = ui->opponentDirectoryPathLine->text();
    opponentScript = ui->opponentScriptPathLine->text();

    opponentGroupActive = ui->opponentGroup->isChecked();


    accept();
}

