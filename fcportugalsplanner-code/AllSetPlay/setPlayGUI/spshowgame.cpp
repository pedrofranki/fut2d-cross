#include "spshowgame.h"


// -*-c++-*-

/*!
  \file main_window.cpp
  \brief main application window class Source File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>



#include "rcsslog/qt4/field_canvas.h"

#include "rcsslog/qt4/log_player.h"
#include "rcsslog/qt4/log_player_tool_bar.h"
#include "rcsslog/qt4/log_slider_tool_bar.h"

#include "rcsslog/qt4/options.h"

#include <string>
#include <iostream>
#include <cstring>

#include "spsetplay.h"
#include "spfield.h"

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "rcsslogplayer"
#endif

#define USE_MONITOR_CLIENT

/*-------------------------------------------------------------------*/
/*!

 */
SpShowGame::SpShowGame(SpSetPlay *s)
    : M_game_log_path()
    , M_log_player( new LogPlayer( M_main_data ) )
    , M_player_type_dialog( static_cast< PlayerTypeDialog * >( 0 ) )

{

    spSetPlay = s;
    createToolBars();
    createFieldCanvas();

    spSetPlay->connect( M_log_player, SIGNAL( updated() ),
             this, SIGNAL( viewUpdated() ) );
    M_main_data.clear();

}

/*-------------------------------------------------------------------*/
/*!

 */
SpShowGame::~SpShowGame()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::createToolBars()
{
    M_log_player_tool_bar = new LogPlayerToolBar( M_log_player,
                                                  M_main_data, spSetPlay);

    spSetPlay->getLogToolBar()->layout()->addWidget(M_log_player_tool_bar);

    M_log_slider_tool_bar = new LogSliderToolBar( M_log_player,
                                                  M_main_data);

    spSetPlay->connect( this,  SIGNAL( viewUpdated() ),
            M_log_slider_tool_bar, SLOT( updateSlider() ) );
    spSetPlay->getLogToolBar()->layout()->addWidget(M_log_slider_tool_bar);


    QString imageDirPath = "setPlayGUI/images/";
    {
        // n
        QToolButton *pushN = new QToolButton();
        pushN->setIcon(QIcon(imageDirPath + "numbers.jpeg" ));
        pushN->setShortcut(Qt::Key_N);
        pushN->setToolTip( tr("Show/Hide player number" ));
        spSetPlay->getLogToolBar()->layout()->addWidget(pushN );
        M_field_canvas->connect( pushN, SIGNAL( clicked()),
                 this, SLOT( toggleShowPlayerNumber() ) );
    }
    {
        // v
        QToolButton *pushV = new QToolButton();
        pushV->setIcon(QIcon(imageDirPath + "vision.jpeg" ));
        pushV->setShortcut(Qt::Key_V);
        pushV->setToolTip(tr( "Show/Hide player's view area" ));
        spSetPlay->getLogToolBar()->layout()->addWidget(pushV );
        M_field_canvas->connect( pushV, SIGNAL( clicked() ),
                 this, SLOT( toggleShowViewArea() ) );
    }
    {
        // o
        QToolButton *pushO = new QToolButton();
        pushO->setIcon(QIcon(imageDirPath + "line.jpeg" ));
        pushO->setShortcut(Qt::Key_O);
        pushO->setToolTip(tr( "Show/Hide offside lines" ));
        spSetPlay->getLogToolBar()->layout()->addWidget(pushO );
        M_field_canvas->connect( pushO, SIGNAL( clicked() ),
                 this, SLOT( toggleShowOffsideLine() ) );
    }
    {
        QToolButton *showHide = new QToolButton();
        showHide->setIcon(QIcon(imageDirPath + "showHide.jpeg" ));
        showHide->setShortcut(Qt::Key_S);
        showHide->setToolTip(tr( "Show/Hide game log" ));
        spSetPlay->getLogToolBar()->layout()->addWidget(showHide );
        M_field_canvas->connect( showHide,  SIGNAL( clicked() ),
                this, SLOT( hideShow()  ));
    }
    {
        QToolButton *exit = new QToolButton();
        exit->setIcon(QIcon(imageDirPath + "exit.jpeg" ));
        exit->setShortcut(Qt::Key_E);
        exit->setToolTip(tr( "Exit game log" ));
        spSetPlay->getLogToolBar()->layout()->addWidget(exit );

        M_field_canvas->connect( exit,  SIGNAL( clicked() ),
                this, SLOT( finish()  ));

    }
}


/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::createFieldCanvas()
{
    M_field_canvas = new FieldCanvas( M_main_data, spSetPlay->getFrameField() );
    M_field_canvas->setFieldRect(spSetPlay->getField()->getFieldRect());
    spSetPlay->connect( this, SIGNAL( viewUpdated() ),
             M_field_canvas, SLOT( update() ) );
    M_field_canvas->setVisible(false);
    M_field_canvas->setEnabled(false);
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::wheelEvent( QWheelEvent * event )
{
    if ( event->delta() < 0 )
    {
        M_log_player->stepForward();
    }
    else
    {
        M_log_player->stepBack();
    }

    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
SpShowGame::openRCG()
{
    M_game_log_path = "./files/setPlaylogFiles";

    QString file_path = QFileDialog::getOpenFileName( spSetPlay,
                                                      tr( "Choose a game log file to open" ),
                                                      M_game_log_path,
                                                      tr( "Game Log files (*.rcg  *.rcg.gz)" ) );

    if ( file_path.isEmpty() )
    {
        //std::cerr << "SpShowGame::opneRCG() empty file path" << std::endl;
        return false;
    }
    M_main_data.setExist(true);
    return openRCG( file_path );
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
SpShowGame::openRCG( const QString & file_path )
{
    if ( ! QFile::exists( file_path ) )
    {
        std::cerr << "File [" << file_path.toStdString()
                  << "] does not exist."
                  << std::endl;
        return false;
    }

    M_log_player->stop();

    M_main_data.closeOutputFile();

    if ( ! M_main_data.openRCG( file_path, spSetPlay ) )
    {
        if ( Options::instance().autoQuitMode() )
        {
            std::cerr << "***ERROR*** Failed to read [" << file_path.toStdString()
                      << "]" << std::endl;
            QTimer::singleShot( 100,
                                qApp, SLOT( quit() ) );
        }
        else
        {
            QString err_msg = tr( "Failed to read [" );
            err_msg += file_path;
            err_msg += tr( "]" );
            QMessageBox::critical( spSetPlay,
                                   tr( "Error" ),
                                   err_msg,
                                   QMessageBox::Ok, QMessageBox::NoButton );
        }
        return false;
    }

    if ( M_main_data.dispHolder().dispInfoCont().empty() )
    {
        if ( Options::instance().autoQuitMode() )
        {
            std::cerr << "***ERROR*** Empty log file ["
                      << file_path.toStdString() << "]" << std::endl;
            QTimer::singleShot( 100,
                                qApp, SLOT( quit() ) );
        }
        else
        {
            QString err_msg = tr( "Empty log file [" );
            err_msg += file_path;
            err_msg += tr( "]" );
            QMessageBox::critical( spSetPlay,
                                   tr( "Error" ),
                                   err_msg,
                                   QMessageBox::Ok, QMessageBox::NoButton );

        }
        return false;
    }

    // update last opened file path
    QFileInfo file_info( file_path );
    M_game_log_path = file_info.absoluteFilePath();
    Options::instance().setGameLogFile( M_game_log_path.toStdString() );

    // set window title
    QString name = file_info.fileName();
    if ( name.length() > 128 )
    {
        name.replace( 125, name.length() - 125, tr( "..." ) );
    }

    M_field_canvas->setVisible(true);
    M_field_canvas->setEnabled(true);
    emit viewUpdated();

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void SpShowGame::update()
{
    M_field_canvas->repaint();
}

/*-------------------------------------------------------------------*/
/*!

 */
void SpShowGame::hideShow()
{
    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
        return;

    M_log_player->stop();
    if(M_field_canvas->isVisible())
        M_field_canvas->setVisible(false);
    else
        M_field_canvas->setVisible(true);
    M_field_canvas->update();
}

/*-------------------------------------------------------------------*/
/*!

 */
void SpShowGame::finish()
{
    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
        return;

    M_log_player->stop();
    M_main_data.clear();
    M_main_data.setExist(false);
    M_field_canvas->update();
    M_field_canvas->setVisible(false);
    M_field_canvas->setEnabled(false);
    spSetPlay->closeSPLA();

}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::resize()
{
    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
        return;

   M_field_canvas->setFieldRect(spSetPlay->getField()->getFieldRect());
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::toggleShowPlayerNumber()
{
    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
        return;

    Options::instance().toggleShowPlayerNumber();
    M_field_canvas->update();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::toggleShowViewArea()
{

    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
    {
        return;
    }
    Options::instance().toggleShowViewArea();
    M_field_canvas->update();

}

/*-------------------------------------------------------------------*/
/*!

 */
void
SpShowGame::toggleShowOffsideLine()
{
    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
        return;

    Options::instance().toggleShowOffsideLine();
    M_field_canvas->update();
}
