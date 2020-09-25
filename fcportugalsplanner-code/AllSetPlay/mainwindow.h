// -*-c++-*-

/* Changelog:
 - Joao Cravo, Luis Paulo Reis: Creation of tool, Version 1.3 (MsC FEUP -University of Porto-2011)
 (Detailed description of the Tool: http://hdl.handle.net/10216/62120, in portuguese)
 - Joao Fabro: Version 1.5 - Corrections and update (2014)

 *Copyright:

 Copyright (C) Joao Cravo

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void makeMenu();

    QTabWidget * changeTabName(QString name);

private:
    Ui::MainWindow *ui;
    QAction *contextBarMenu;
    QAction *exportAction;

public slots:

    void openSetPlay();
    void saveSetPlay();
    void newSetPlay();



private slots:
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_currentChanged(QWidget* );
    void on_importFCPSetPlayButton_clicked();
    void on_newSetPlayButton_clicked();
    void about();
    void contentHelp();
};

#endif // MAINWINDOW_H
