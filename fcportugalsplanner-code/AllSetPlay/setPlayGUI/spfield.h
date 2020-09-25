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

#ifndef SPFIELD_H
#define SPFIELD_H

#include <QGraphicsView>
#include <QLabel>
class SpStep;
class SpPlayer;
class SpAction;
class SpSetPlay;

class SpField : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpField(QFrame *parent = 0);
    ~SpField();

    void init(SpSetPlay * ssp, int situation, QList <int> regions, QList <int> transverse_regions);
    void init(SpSetPlay * ssp);
    void setSituationAndRegion(int sit, QList <int> reg, QList <int> transverse_reg);
    void setFieldDimensions(std::string league);

    void changePlayers(QList<SpPlayer *> players);
    void removePlayerFromBench(SpPlayer *player);
    void playerToBench(SpPlayer *player);
    void addAction(SpPlayer *player, int type);
    void addActionPassForward(SpPlayer *p, int type, SpPlayer *destP);

    bool actualizePlayerPositions(SpPlayer * player, bool moveplayers, bool final);
    void itemMoved();
    void cleanField();

    void actualize(SpStep * step);
    SpPlayer * getCurrentBallPlayerHolder();
    SpPlayer * getPlayerBallHolderOnStep(int stepNumber);

    void positionLabelUpdate(QPoint p);
    bool canRemove(SpPlayer *player);
    bool canMove(SpPlayer *player);
    QPointF getSpecialAreaPosition();
    bool isOnSpecialAreas(QPointF pos);
    bool isInsideField(QPointF pos);

    inline QPointF getLeftGoalCenter() const { return QPointF(screenX(  pitch_length * 0.5 ), screenY(0) ); }
    inline QRectF getFieldRect() const { return fieldRect; }
    inline QList<QRectF> getSpecialAreas() const { return specialAreas; }


    inline int screenX( const double &x ) const { return g_pitch_center.x() + scaleToField( x ); }
    inline int screenY( const double &y ) const { return g_pitch_center.y() + scaleToField( y ); }
    inline double getPitchX( const double x ) const { return ( x - g_pitch_center.x() ) / g_pitch_scale; }
    inline double getPitchY( const double y ) const { return ( y - g_pitch_center.y() ) / g_pitch_scale; }

    inline double getLenght() const {return pitch_length; }
    inline double getWidth() const {return pitch_width; }

    inline QList<SpPlayer *> getInPlayers() const { return inPlayers; }
    inline QList<SpPlayer *> getOutPlayers() const { return outPlayers; }
    inline double getOffSideLineX() const { return offSideLineX; }

    inline double getScale() {return g_pitch_scale/6;}

private:
    QList<SpPlayer *> outPlayers;
    QList<SpPlayer *> inPlayers;

    QFrame *frame;
    SpSetPlay *setPlay;

    SpAction * drawingAction;
    QRectF fieldRect;
    QRectF benchRect;

    QList <int> specialAreasDef;
    QList <int> specialTransverseAreasDef;
    QList <QRectF> specialAreas;
    int situation;
    int offSideLineX;


    QPainter *painter;

    double pitch_length;
    double pitch_width;
    double pitch_margin;
    double centerCircle_radius;
    double goalArea_length;
    double goalArea_width;
    double penaltyArea_length;
    double penaltyArea_width;
    double penaltyCircle_radius;
    double penalty_distance;
    double goal_depth;
    double goal_width;
    double goal_post_radius;
    double bench_width;
    double bench_length;

    QPoint g_pitch_center;
    double g_pitch_scale;
    bool g_zoomed;
    QPointF g_focus_point;
    QPixmap g_pitch_pixmap;

    QBrush g_background_brush;
    QPen g_lines_pen;


    QGraphicsSimpleTextItem* positionLabel;



    void calculateRects();
    void replacePlayers();
    void replaceBenchPlayers();
    void replaceParticipantPlayers();


    void drawBackground(QPainter *, const QRectF &rect);
    void drawBackgroundContainedArea(QPainter *painter);
    void drawFieldLines(QPainter *painter);
    void drawBench(QPainter *painter);

    void setFocusPoint( const QPoint & pos );
    void zoomIn();
    void zoomOut();
    void fitSize();
    void updateSize();

    inline int scaleToField( const double &len ) const {  return static_cast< int >( len * g_pitch_scale); }

    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent( QMouseEvent * event );
    void wheelEvent(QWheelEvent *event);
    void resizeEvent (QResizeEvent *);


};

//#define DEBUGFIELD 0

#endif // SPFIELD_H
