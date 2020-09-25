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

#ifndef SPTESTRCSOCCERSIMCONFIGDIALOG_H
#define SPTESTRCSOCCERSIMCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class SpTestRCSoccerSimConfigDialog;
}

class SpTestRCSoccerSimConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpTestRCSoccerSimConfigDialog(QWidget *parent = 0);
    ~SpTestRCSoccerSimConfigDialog();

    static QString logFilesDirectory;
    static QString simulatorExecutable;

    static QString teamDirectory;
    static QString teamScript;
    static QString teamSetPlayFile;

    static QString opponentDirectory;
    static QString opponentScript;
    static bool opponentGroupActive;

private slots:
    void on_logFilesDirectoryButton_clicked();
    void on_simulatorExecutableButton_clicked();
    void on_teamDirectoryButton_clicked();
    void on_teamScriptButton_clicked();

    void on_teamSetPlayFileButton_clicked();
    void on_opponentDirectoryButton_clicked();
    void on_opponentScriptButton_clicked();

    void on_cancelButton_clicked();
    void on_resetButton_clicked();
    void on_testButton_clicked();

private:
    Ui::SpTestRCSoccerSimConfigDialog *ui;


};

#endif // SPTESTRCSOCCERSIMCONFIGDIALOG_H
