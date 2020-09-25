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

#include "spdebugfcpdebuglogplayerconfigdialog.h"
#include "ui_spdebugfcpdebuglogplayerconfigdialog.h"

#include <QFileDialog>
#include <QDebug>
SpDebugFCPDebugLogPlayerConfigDialog::SpDebugFCPDebugLogPlayerConfigDialog(QString glp, QString pld,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpDebugFCPDebugLogPlayerConfigDialog)
{
    ui->setupUi(this);

     gameLogPath = glp;
    playersLogDirectoryPath = pld;

    withPlayersLog = true;

    on_ResetPushButton_clicked();


}

SpDebugFCPDebugLogPlayerConfigDialog::~SpDebugFCPDebugLogPlayerConfigDialog()
{
    delete ui;
}

void SpDebugFCPDebugLogPlayerConfigDialog::on_gameLogFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                QString("Game log file..."),
                ui->gameLogFileLineEdit->text(), tr("game log files (*.rcg *.rcg.gz)"));

    if(filePath.isEmpty())
       return;

    ui->gameLogFileLineEdit->setText(filePath);
}

void SpDebugFCPDebugLogPlayerConfigDialog::on_playersLogDirectoryhButton_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                QString("Players log directory..."),
                ui->playersLogDirectoryLineEdit->text());

    if(filePath.isEmpty())
       return;
    ui->playersLogDirectoryLineEdit->setText(filePath);
}



void SpDebugFCPDebugLogPlayerConfigDialog::on_cancelButton_clicked()
{
  reject();
}

void SpDebugFCPDebugLogPlayerConfigDialog::on_debugButton_clicked()
{
    gameLogPath = ui->gameLogFileLineEdit->text();
    playersLogDirectoryPath = ui->playersLogDirectoryLineEdit->text();
    withPlayersLog = ui->groupBox->isChecked();

    accept();
}

void SpDebugFCPDebugLogPlayerConfigDialog::on_ResetPushButton_clicked()
{

    ui->gameLogFileLineEdit->setText(gameLogPath);
    ui->playersLogDirectoryLineEdit->setText(playersLogDirectoryPath);

    ui->groupBox->setChecked(withPlayersLog);
}
