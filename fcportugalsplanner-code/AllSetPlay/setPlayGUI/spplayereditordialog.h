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

#ifndef SPPLAYEREDITORDIALOG_H
#define SPPLAYEREDITORDIALOG_H

#include <QDialog>

#include "sppoint.h"

class SpStep;
class SpAction;
class SpPlayer;
class SpSetPlay;

namespace Ui {
    class SpPlayerEditorDialog;
}

class SpPlayerEditorDialog : public QDialog
{
    Q_OBJECT

public:
    enum { infoTab, stepTab, transitionTab };
    explicit SpPlayerEditorDialog(QWidget *parent = 0);

    void init(SpPlayer *player, int openTab = 0);

    ~SpPlayerEditorDialog();

    void changeRelativePositionButtons();
    void changeRelativeActionButtons();

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_resetButton_clicked();


    void on_comboSelectRelativePlayerPosition_activated(int index);

    void on_comboSelectActionRelativePlayer_activated(int index);

    void on_rbPositionAbsolute_clicked(bool checked);

    void on_playerEditorTabs_currentChanged(int index);

private:


    Ui::SpPlayerEditorDialog *ui;
    SpPlayer *player;
    SpSetPlay * spSetPlay;
    SpAction *spAction;
    double lenght;
    double width;


    SpPoint *posPoint;
    SpPoint *actPoint;

    int currentStepNumber;
    int currentTransitionNumber;
    int actionType;
};

#endif // SPPLAYEREDITORDIALOG_H
