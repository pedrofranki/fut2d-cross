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

#ifndef SPINIT_H
#define SPINIT_H

#include <QDialog>

namespace Ui {
    class SpInitDialog;
}

class SpInitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpInitDialog(QWidget *parent = 0);
    ~SpInitDialog();

    inline const int getType() { return type; }
    inline const int getSituation() { return situation; }
    inline const QList <int> getSpecialAreas() { return specialAreas; }
    inline const QList <int> getSpecialTransverseAreas() { return specialTransverseAreas; }

private slots:
    void on_situationComboBox_currentIndexChanged(QString );
    void on_fieldBox_stateChanged(int );
    void on_pointBox_stateChanged(int );
    void on_ourMiddleField_stateChanged(int );
    void on_theirMiddleField_stateChanged(int );
    void on_rightCB_clicked(bool checked);
    void on_leftCB_clicked(bool checked);
    void on_cancelButton_clicked();
    void on_createButton_clicked();

private:
    int type;
    int situation;
    QList <int> specialAreas;
    QList <int> specialTransverseAreas;
    Ui::SpInitDialog *ui; 
};

#endif // SPINIT_H
