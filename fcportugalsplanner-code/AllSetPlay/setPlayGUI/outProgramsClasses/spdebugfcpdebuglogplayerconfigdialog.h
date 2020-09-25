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

#ifndef SPDEBUGFCPDEBUGLOGPLAYERCONFIGDIALOG_H
#define SPDEBUGFCPDEBUGLOGPLAYERCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class SpDebugFCPDebugLogPlayerConfigDialog;
}

class SpDebugFCPDebugLogPlayerConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpDebugFCPDebugLogPlayerConfigDialog( QString pld, QString glp, QWidget *parent = 0);
    ~SpDebugFCPDebugLogPlayerConfigDialog();

    QString playersLogDirectoryPath;
    QString gameLogPath;
    bool withPlayersLog;

private slots:
    void on_gameLogFileButton_clicked();
    void on_playersLogDirectoryhButton_clicked();
    void on_cancelButton_clicked();
    void on_debugButton_clicked();
    void on_ResetPushButton_clicked();

private:
    Ui::SpDebugFCPDebugLogPlayerConfigDialog *ui;
};

#endif // SPDEBUGFCPDEBUGLOGPLAYERCONFIGDIALOG_H
