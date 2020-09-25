// -*-c++-*-

#ifndef SPLOGANALYSIS_H
#define SPLOGANALYSIS_H

/*!
  \file main_window.h
  \brief main application window class Header File.
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

#include <QProcess>
#include <QString>
#include <QWheelEvent>

#include "rcsslog/qt4/main_data.h"

class QAction;
class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QLabel;
class QPoint;
class QResizeEvent;
class FieldCanvas;
class LogPlayer;
class LogPlayerToolBar;
class LogSliderToolBar;
class PlayerTypeDialog;

class SpSetPlay;

class SpShowGame : QObject
{
    Q_OBJECT
public:
    FieldCanvas * M_field_canvas;
private:

    MainData M_main_data;
    QString M_game_log_path; //!< file path to the last opened game log

    LogPlayer * M_log_player;
    LogPlayerToolBar * M_log_player_tool_bar;
    LogSliderToolBar * M_log_slider_tool_bar;
    PlayerTypeDialog * M_player_type_dialog;

    QLabel * M_position_label;

    // view actions
    QAction * M_toggle_menu_bar_act;
    QAction * M_toggle_tool_bar_act;

    QAction * M_toggle_field_canvas_act;
    QAction * M_full_screen_act;
    QAction * M_show_player_type_dialog_act;
    QAction * M_show_detail_dialog_act;
    QAction * M_show_config_dialog_act;

    SpSetPlay * spSetPlay;

    bool exist;

public:

    SpShowGame(SpSetPlay *s);
    ~SpShowGame();

    void update();
    void resize();



private:
    bool openRCG( const QString & file_path );
    void createToolBars();
    void createFieldCanvas();

protected:
    void wheelEvent( QWheelEvent * event );

private slots:    
    void toggleShowPlayerNumber();
    void toggleShowViewArea();
    void toggleShowOffsideLine();

public slots:
    bool openRCG();
    void hideShow();
    void finish();

signals:
    void viewUpdated();
};

#endif // SPLOGANALYSIS_H
