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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "spsetplay.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    makeMenu();
    contextBarMenu = NULL;

    ui->initialFrame->show();
    ui->tabWidget->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::makeMenu()
{    


    // NEW ACTIONS == //
    QMenu *newMenu = new QMenu(tr("&New..."),this);
    newMenu->addAction(tr("&SetPlay"), this, SLOT(newSetPlay()), Qt::CTRL + Qt::Key_N );
    // ADD OTHER NEW MODULES ACTIONS HERE

    ui->menuFile->addMenu(newMenu);
    // == NEW ACTIONS //

    ui->menuFile->addSeparator();

    // IMPORT ACTIONS == //
    QMenu * importMenu = new QMenu(tr("&Import..."));
    importMenu->addAction(tr("&SetPlay"), this, SLOT(openSetPlay()),Qt::CTRL + Qt::Key_I );
    // ADD OTHER IMPORT MODULES ACTIONS HERE

    ui->menuFile->addMenu(importMenu);
    // == IMPORT ACTIONS  //

    // EXPORT ACTION == //
    exportAction = new QAction(tr("&Export..."), this);
    exportAction->setShortcut(Qt::Key_F3);
    exportAction->setEnabled(false);
    ui->menuFile->addAction(exportAction);
    // == EXPORT ACTION //

    ui->menuFile->addSeparator();

    QAction * exitAction = new QAction(tr("&Exit"),this);
    exitAction->setStatusTip("Exit Splanner");
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    ui->menuFile->addAction(exitAction);

    // ABOUT MENU
    QAction * aboutAction = new QAction(tr("&About"),this);
    aboutAction->setStatusTip("Splanner about");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    ui->menuHelp->addAction(aboutAction);

    QAction * contentHelpAction = new QAction(tr("&Content"),this);
    contentHelpAction->setStatusTip("Show help contents");
    connect(contentHelpAction, SIGNAL(triggered()), this, SLOT(contentHelp()));
    ui->menuHelp->addAction(contentHelpAction);
}


QTabWidget * MainWindow::changeTabName(QString name)
{
    int i = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(i, name);
}

void MainWindow::newSetPlay()
{

    SpSetPlay *newSetPlay = new SpSetPlay(this);
    if(!newSetPlay->init() )
        delete newSetPlay;
    else
    {
        ui->initialFrame->hide();
        ui->tabWidget->show();
        int i = ui->tabWidget->addTab(newSetPlay, "Set Play editor");
        ui->tabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    //TODO  call save
    ui->tabWidget->removeTab(index);
    if(ui->tabWidget->count() == 0)
    {
        ui->initialFrame->show();
        ui->tabWidget->hide();
    }
}


void MainWindow::on_tabWidget_currentChanged(QWidget* widget)
{
    if(contextBarMenu)
    {
        ui->menuBar->removeAction(contextBarMenu);
        exportAction->setText("Export...");
        exportAction->setEnabled(false);
    }
    if(dynamic_cast<SpSetPlay*>(widget))
    {
        SpSetPlay *a = dynamic_cast<SpSetPlay*>(widget);
        QMenu *c = a->getContextBarMenu();

        QAction *b = ui->menuHelp->menuAction();

        contextBarMenu = ui->menuBar->insertMenu( b, c);

        exportAction->setText("Export SetPlay");
        disconnect(exportAction, 0 , 0, 0);
        connect(exportAction, SIGNAL(triggered()), SLOT(saveSetPlay()));
        exportAction->setEnabled(true);
    }
}

void MainWindow::openSetPlay()
{

    SpSetPlay *newSetPlay = new SpSetPlay(this);
    if(! newSetPlay->open() )
        delete newSetPlay;
    else
    {
        ui->initialFrame->hide();
        ui->tabWidget->show();
        int i = ui->tabWidget->addTab(newSetPlay,"");

        ui->tabWidget->setCurrentIndex(i);
        ui->tabWidget->setTabText(i, "SP:" + newSetPlay->getName());
    }
}

void MainWindow::saveSetPlay()
{

    SpSetPlay *newSetPlay = dynamic_cast<SpSetPlay*>(ui->tabWidget->currentWidget());
    newSetPlay->save();
}


void MainWindow::on_importFCPSetPlayButton_clicked()
{
    openSetPlay();
}

void MainWindow::on_newSetPlayButton_clicked()
{
    newSetPlay();
}


void MainWindow::about()
{
    QString msg =
            tr( "SPlanner\n"
                " V 1.5\n\n"
                "Splanner - FCPortugal Setplay Planner\n"
                "Splanner is a Setplay Editor for FCPortugal Setplay Library\n\n"
                "Version 1.5 include:\n"
                "    - Setplay creation, edition, import and export \n"
                "    - Tools for fast test and debug of setplays, using the applications:   \n"
                "          - RoboCup Soccer Simulator\n"
                "          - RoboCup Soccer Log Player\n"
                "          - The FCPortugal's RoboCup Soccer Simulator Debugger\n\n"
                "  Copyright (C) Joao Cravo and Luis Paulo Reis (2011), Updated by Joao Fabro (2014)");

    QMessageBox::about( this, tr( "About FCPortugal Splanner" ), msg );
}

void MainWindow::contentHelp()
{
    QString msg =
            tr( "FCPortugal SPlanner V 1.5\n\n"
                "From the Splash Screen, Create new Setplays(New) or\n"
                "Import existing ones(Import) from config files(.conf). \n"
                "(For new Setplay, choose the Situation, and the start position)\n\n"
                "The Edit Setplay Main Screen will appear.\n"
                "  (The Setplay Graph -left of the screen- is in State 0-zero)\n\n"
                " To Edit the Setplay, do the following:\n\n"
                " 1 - Drag-and-Drop players from the top to the field;\n"
                "  (You can freely reposition the players, but only in State 0!)\n"
                "  (The current Setplay Framework accepts up to 5 players.)\n\n"
                " 2 - Click with the right mouse button over any player, \n"
                "     in order to associate actions to them;\n"
                "  (Detais of each player are available:click on ^ at bottom of screen)\n"
                "  - When you define an action to the player in possession of\n"
                "  the ball, a new State of the Setplay Graph is created.\n\n"
                " 3 - Left click on any Graph State - the field of play will be updated.\n"
                "   (You can also adjust the graph design with drag-drop operations) \n\n"
                " 4 - Now repeat the process (2-3), creating as many States you want. \n"
                "   (dont forget to adjust Wait time and Abort time for each Step.)\n\n"
                " 5 - Edit Setplay Name, Id, Comment, invertibility(left/right)\n"
                " and then export (File, Export Setplay) to a .conf file." );

    QMessageBox::about( this, tr( "Content Help" ), msg );
}
