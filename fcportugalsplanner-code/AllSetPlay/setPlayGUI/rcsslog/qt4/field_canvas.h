// -*-c++-*-

/*!
	\file field_canvas.h
	\brief field canvas class Header File.
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.	If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_FIELD_CANVAS_H
#define RCSSLOGPLAYER_FIELD_CANVAS_H

#ifdef USE_GLWIDGET
#include <QGLWidget>
#else
#include <QWidget>
#endif
#include <QPen>
#include <QFont>

#include "mouse_state.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <cmath>
#include <QFrame>

class QContextMenuEvent;
class QMenu;
class QMouseEvent;
class QPaintEvent;

class MainData;
class PainterInterface;


//! main soccer field canvas class
class FieldCanvas: public QFrame
{

    Q_OBJECT

private:

    MainData & M_main_data;

    QMenu * M_normal_menu;


    std::vector< boost::shared_ptr< PainterInterface > > M_painters;

    //! 0: left, 1: middle, 2: right
    MouseState M_mouse_state[3];

    QPen M_measure_line_pen;
    QPen M_measure_mark_pen;
    QPen M_measure_font_pen;
    QPen M_measure_font_pen2;
    QFont M_measure_font;




    QRect fieldrect;

public:

    explicit
    FieldCanvas( MainData & main_data, QFrame *parent );

    ~FieldCanvas();

    void createPainters();
    void draw( QPainter & painter );
    void setFieldRect(QRectF f);
private:

    void drawMouseMeasure( QPainter & painter );
    void updateFocus();
    void selectPlayer( const QPoint & point );

protected:

    void mouseDoubleClickEvent( QMouseEvent * event );
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void mouseMoveEvent( QMouseEvent * event );
    void paintEvent( QPaintEvent * );


signals:

    void playerMoved( const QPoint & point );
    void playerSelected( int number );
    void focusChanged( const QPoint & point );
    void mouseMoved( const QPoint & point );
};

#endif
