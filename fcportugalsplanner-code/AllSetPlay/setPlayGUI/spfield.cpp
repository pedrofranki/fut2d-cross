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

#include "spfield.h"

#include <QDebug>
#include <QMouseEvent>
#include <QLayout>
#include "spstep.h"
#include "spplayer.h"
#include "spaction.h"
#include "spsetplay.h"
#include <QTextDocument>


//TODO this class could be a parent class of some kinds of field: field 2D, 3D, or half field etc


SpField::SpField(QFrame *f) :
    g_pitch_center(0.0,0.0),
    g_pitch_scale(1.0),
    g_zoomed(false),
    g_focus_point(0.0,0.0),
    g_background_brush(QColor( 14, 204, 21 ), Qt::SolidPattern),
    g_lines_pen(QColor( 255, 255, 255 ), 1, Qt::SolidLine)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setScene(scene);
    setAcceptDrops(true);
    setMouseTracking(true);

    outPlayers = QList<SpPlayer *>();
    inPlayers = QList<SpPlayer *>();
    offSideLineX = 0;
    drawingAction = NULL;
    frame =f;



    positionLabel = new QGraphicsSimpleTextItem();
    positionLabel->setFont(QFont("Monospace",8));
    positionLabel->setBrush(Qt::red);


}

SpField::~SpField()
{
    foreach (SpPlayer *p, inPlayers)
        scene()->removeItem(p);
    inPlayers.clear();
    foreach (SpPlayer *p, outPlayers)
        scene()->removeItem(p);
    outPlayers.clear();
    setPlay = NULL;
}

void SpField::init(SpSetPlay * ssp, int sit, QList <int> regions, QList <int> transverse_regions)
{
    setPlay = ssp;

    setFieldDimensions("2D"); //TODO
    setMinimumSize(pitch_length* 255 / pitch_width , 255);
    setSituationAndRegion(sit, regions, transverse_regions);

    scene()->addItem(positionLabel);
}

void SpField::init(SpSetPlay * ssp)
{
    setPlay = ssp;
    setFieldDimensions("2D"); //TODO
    setMinimumSize(pitch_length* 255 / pitch_width , 255);
    scene()->addItem(positionLabel);


}

void SpField::setSituationAndRegion(int sit, QList <int> reg, QList <int> transverse_reg)
{
    situation = sit;
    specialAreasDef = reg;
    specialTransverseAreasDef = transverse_reg;
    QMap<int, SpPlayer *> players = setPlay->getPlayers();
    for(int i = 1; i < 12; i++)
    {
       SpPlayer *player = players[i];
       outPlayers.append(player);
       scene()->addItem(player);
    }

    updateSize();
    calculateRects();
}

void SpField::setFieldDimensions(std::string league)
{
    if ( league == "3D" )
    {
        pitch_length = 18.0;
        pitch_width = 12.0;
        pitch_margin = 1.0;
        centerCircle_radius = 2.0;
        penaltyArea_length = 2.0;
        penaltyArea_width = 6.0;
        penaltyCircle_radius = 2.0;
        penalty_distance = 1.5;
        goalArea_length = 2.0;
        goalArea_width = 6.0;
        goal_depth = 1.0;
        goal_width = 2.0;
        goal_post_radius = 0.06;
        bench_length = 10.0;
        bench_width = 1.6;
    }
    else if ( league == "2D" )
    {
        pitch_length = 105.0;
        pitch_width = 68.0;
        pitch_margin = 5.0;
        centerCircle_radius = 9.15;
        penaltyArea_length = 16.5;
        penaltyArea_width = 40.32;
        penaltyCircle_radius = 9.15;
        penalty_distance = 11.0;
        goalArea_length = 5.5;
        goalArea_width = 18.2;
        goal_depth = 2.5;
        goal_width = 14.02;
        goal_post_radius = 0.06;
        bench_width = 6.0;
        bench_length= 41.0;
    }

    //TODO CHECK if others leagues...
    //TODO get this values from file
}

void SpField::changePlayers(QList <SpPlayer *> players)
{
    foreach(SpPlayer *p , players)
        removePlayerFromBench(p);
}

void SpField::removePlayerFromBench(SpPlayer *p)
{
    if(outPlayers.contains(p))
    {
        if(p->getAction())
            scene()->addItem(p->getAction());
        outPlayers.removeOne(p);
        inPlayers.append(p);
    }
}

void SpField::playerToBench(SpPlayer *p)
{
    int id = p->getNumber();

    int width = benchRect.width();
    int playerSize = 30*width/510;
    int spaceSize = (width - 11*playerSize) / 12;
    p->setPos(benchRect.left()+ spaceSize*(id-1) + playerSize*id , benchRect.center().y() );
    //p->setPos(benchRect.center().x(), benchRect.top() + spaceSize*(id-1) + playerSize*id );

    if(!outPlayers.contains(p))
    {
        if(p->getAction())
            scene()->removeItem(p->getAction());
        outPlayers.append(p);
        inPlayers.removeOne(p);
    }
}

void SpField::addAction(SpPlayer *p, int type)
{
    drawingAction = new SpAction(p, type, this);
    scene()->addItem(drawingAction);
}

void SpField::addActionPassForward(SpPlayer *p, int type, SpPlayer *destP)
{
    drawingAction = new SpAction(p, type, this);
    drawingAction->setDestPlayer(destP);
    scene()->addItem(drawingAction);
}

void SpField::cleanField()
{
    foreach(SpPlayer *player, inPlayers)
        playerToBench(player);
}

void SpField::actualize(SpStep * step)
{
    foreach(SpPlayer *player, outPlayers)
        if (player->isParticipantOnStep(step->getIdNumber()))
            removePlayerFromBench(player);

    replacePlayers();
    itemMoved();
    scene()->update(sceneRect());
}

QPointF SpField::getSpecialAreaPosition()
{
    if(specialAreas.size() > 0)
    {
        int area = rand() % specialAreas.size();
        return specialAreas[area].center();
    }
    else
    {
        QPointF a(  (rand() % (int) (fieldRect.height() - fieldRect.height() * 0.5)) + fieldRect.x(),
                      (rand() % (int) (fieldRect.width()  - fieldRect.width()  * 0.5)) + fieldRect.y())  ;
        return a;
    }
}

SpPlayer * SpField::getPlayerBallHolderOnStep(int stepNumber)
{
    return(setPlay->getBallHolderOnStep(stepNumber));
}

bool SpField::isOnSpecialAreas(QPointF pos)
{
    foreach(QRectF s, specialAreas)
        if(s.contains(pos))
            return true;

    return false;
}
bool SpField::isInsideField(QPointF pos)
{
    return fieldRect.contains(pos);
}

void SpField::calculateRects()
{
    // set screen coordinates of field
    const int left_x   = screenX( - pitch_length * 0.5 );
    const int right_x  = screenX( + pitch_length * 0.5 );
    const int top_y    = screenY( - pitch_width * 0.5 );
    const int bottom_y = screenY( + pitch_width * 0.5 );

    fieldRect = QRect(QPoint(left_x, top_y), QPoint(right_x, bottom_y) );

    benchRect = QRect( left_x + scaleToField((pitch_length * 0.5 - bench_length)/2),
                          top_y  - scaleToField(bench_width*2),
                          scaleToField(bench_length), scaleToField(bench_width) );

    /*benchRect = QRect( left_x - scaleToField(bench_length *2),
                      g_pitch_center.y() - scaleToField(bench_width/2),
                      scaleToField(bench_length), scaleToField(bench_width) );*/

    specialAreas.clear();

    double dif = scaleToField(goal_depth);
    if(situation == corner_kick)
    {
        if(specialTransverseAreasDef.contains(right_region))
            specialAreas.append(QRectF(right_x - dif, bottom_y - dif,
                                 dif*2,dif*2));
        else if(specialTransverseAreasDef.contains(left_region))
            specialAreas.append(QRectF(right_x - dif, top_y - dif,
                                  dif*2, dif*2));
    }
    else if(situation == kick_off)
    {
       specialAreas.append(QRectF(g_pitch_center.x()- dif, g_pitch_center.y() -dif,
                                 dif*2,dif*2));
    }
    else if(situation == goal_kick || situation == keeper_catch)
    {
        const int pen_top_y    = screenY( - penaltyArea_width / 2 );
        const int pen_bottom_y = screenY( + penaltyArea_width / 2 );
        int pen_x = screenX( -( pitch_length * 0.5 - penaltyArea_length ) );
        specialAreas.append(QRectF(QPointF(pen_x, pen_top_y), QPointF(left_x, pen_bottom_y)));
    }
    else if(situation == throw_in)
    {
        //JAF2014 Merge specialAreasDef with specialTransverseAreasDef
        foreach(int specialTransverseAreaDef, specialTransverseAreasDef)
          if(!specialAreasDef.contains(specialTransverseAreaDef))
            specialAreasDef.append(specialTransverseAreaDef);

        int side = rightSide;
        if(specialAreasDef.contains(left_region))
            side =leftSide;

        foreach(int specialAreaDef, specialAreasDef)
        {
            if( specialAreaDef == right_region || specialAreaDef == left_region)
                 continue;
            if(side == rightSide)
            {
                if(specialAreaDef == field || specialAreaDef == point)
                    specialAreas.append(QRectF(left_x, bottom_y - dif, right_x-left_x, dif*2));
                else
                {
                    if(specialAreaDef == our_middle_field)
                        specialAreas.append(QRectF(left_x, bottom_y - dif, (right_x-left_x)/2, dif*2));
                    else
                    {
                        if(specialAreaDef == our_back)
                            specialAreas.append(QRectF(left_x, bottom_y - dif, (right_x-left_x)/6, dif*2));
                        if(specialAreaDef == our_middle)
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/6, bottom_y - dif, (right_x-left_x)/6, dif*2));
                        if(specialAreaDef == our_front)
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/3, bottom_y - dif, (right_x-left_x)/6, dif*2));
                    }

                    if(specialAreaDef == their_middle_field)
                        specialAreas.append(QRectF(left_x +(right_x-left_x)/2, bottom_y - dif, (right_x-left_x)/2, dif*2));
                    else
                    {
                        if(specialAreaDef == their_front )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/2, bottom_y - dif, (right_x-left_x)/6, dif*2));
                        if(specialAreaDef == their_middle )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)*2/3, bottom_y - dif, (right_x-left_x)/6, dif*2));
                        if(specialAreaDef == their_back )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)*5/6, bottom_y - dif, (right_x-left_x)/6, dif*2));
                    }
                }
            }

            if(side == leftSide)
            {
                if(specialAreaDef == field  || specialAreaDef == point)
                    specialAreas.append(QRectF(left_x, top_y - dif, right_x-left_x, dif*2));
                else
                {
                    if(specialAreaDef == our_middle_field)
                        specialAreas.append(QRectF(left_x, top_y - dif, (right_x-left_x)/2, dif*2));
                    else
                    {
                        if(specialAreaDef == our_back )
                            specialAreas.append(QRectF(left_x, top_y - dif, (right_x-left_x)/6, dif*2));
                        else if(specialAreaDef == our_middle)
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/6, top_y - dif, (right_x-left_x)/6, dif*2));
                        else if(specialAreaDef == our_front )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/3, top_y - dif, (right_x-left_x)/6, dif*2));
                    }

                    if(specialAreaDef == their_middle_field)
                        specialAreas.append(QRectF(left_x +(right_x-left_x)/2, top_y - dif, (right_x-left_x)/2, dif*2));
                    else
                    {
                        if(specialAreaDef == their_front )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)/2, top_y - dif, (right_x-left_x)/6, dif*2));
                        else if(specialAreaDef == their_middle )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)*2/3, top_y - dif, (right_x-left_x)/6, dif*2));
                        else if(specialAreaDef == their_back )
                            specialAreas.append(QRectF(left_x +(right_x-left_x)*5/6, top_y - dif, (right_x-left_x)/6, dif*2));
                    }
                }
            }
        }
    }
    else if(situation == dir_free_kick || situation == ind_free_kick || situation == play_on)
    {//TODO Refactor the code for easy of understanding!!! JAF2014
#ifdef DEBUGFIELD
        cerr << "SpField::calculateRects(): Enter!" << endl;
        cerr << "SpField::calculateRects(): situation (dir_free_kick || ind_free_kick || play_on) = ";
        if(situation == play_on) cerr << "play_on" << endl;
        else if(situation == ind_free_kick) cerr << "ind_free_kick" << endl;
        else if(situation == dir_free_kick) cerr << "dir_free_kick" << endl;
#endif
        QRectF *a = new QRectF(0.0,0.0,0.0,0.0);//JAF2014

        QSet <int> specialAreasSet = specialAreasDef.toSet();//JAF2014 Conversion to Sets ->subtract Transverse areas!
        QSet <int> specialTransverseAreasSet = specialTransverseAreasDef.toSet();
        QSet <int> newspecialAreasSet = specialAreasSet.operator-(specialTransverseAreasSet);
        QList <int> newspecialAreasDef = newspecialAreasSet.toList();

#ifdef DEBUGFIELD
        foreach(int specialAreaDef, newspecialAreasDef)
            cerr << "specialAreaDef = " << setPlayRegionsNames[specialAreaDef] << endl;
        cerr << endl;
#endif

        if(newspecialAreasDef.isEmpty())
        {
            a->setRect(left_x, top_y, right_x-left_x, bottom_y-top_y);// All the field JAF2014
            newspecialAreasDef.append(9999);//Not a valid area - only to enter the loop bellow!!! JAF2014!!!
        }
        foreach(int specialAreaDef, newspecialAreasDef)
        {
            if(specialAreaDef == field || specialAreaDef == point)
                specialAreas.append(fieldRect);
            else
            {
                if(specialAreaDef == our_middle_field)
                    a->setRect(left_x, top_y, (right_x-left_x)/2, bottom_y-top_y);
                else
                {
                    if(specialAreaDef == our_back)
                        a->setRect(left_x, top_y, (right_x-left_x)/6, bottom_y-top_y);
                    if(specialAreaDef == our_middle)
                        a->setRect(left_x + (right_x-left_x)/6, top_y, (right_x-left_x)/6, bottom_y-top_y);
                    if(specialAreaDef == our_front)
                        a->setRect(left_x + (right_x-left_x)/3, top_y, (right_x-left_x)/6, bottom_y-top_y);
                }

                if(specialAreaDef == their_middle_field)
                    a->setRect(left_x + (right_x-left_x)/2, top_y, (right_x-left_x)/2, bottom_y-top_y);
                else
                {
                    if(specialAreaDef == their_front)
                        a->setRect(left_x + (right_x-left_x)/2, top_y, (right_x-left_x)/6, bottom_y-top_y);
                    if(specialAreaDef == their_middle)
                        a->setRect(left_x+ (right_x-left_x) *2/3, top_y, (right_x-left_x)/6, bottom_y-top_y);
                    if(specialAreaDef == their_back)
                        a->setRect(left_x+ (right_x-left_x) *5/6, top_y, (right_x-left_x)/6, bottom_y-top_y);
                }

                if(specialTransverseAreasDef.isEmpty())
                    specialAreas.append(QRectF(a->left(), a->top(), a->width(), a->height()));
                else
                {
#ifdef DEBUGFIELD
                    foreach(int TransverseAreaDef, specialTransverseAreasDef)
                        cerr << "TransverseArea = " << setPlayRegionsNames[TransverseAreaDef] <<endl;
                    cerr << endl;
#endif
                    foreach(int specialTransverseAreaDef, specialTransverseAreasDef)
                    {
                        if(specialTransverseAreaDef == left_region){
                            if(a->intersects(QRectF(left_x, top_y, (right_x-left_x), (bottom_y-top_y)/2)))
                            {
#ifdef DEBUGFIELD
                             cerr << "Intersected with left_region!!!" << endl;
#endif
                             specialAreas.append(a->intersected(QRectF(left_x, top_y, (right_x-left_x), (bottom_y-top_y)/2)));
                            }
                        }
                        else
                        {
                            if(specialTransverseAreaDef == far_left){
                                if(a->intersects(QRectF(left_x, top_y, (right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with far_left!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y, (right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //(QRectF(left_x, top_y, (right_x-left_x), (bottom_y-top_y)/6));
                            if(specialTransverseAreaDef == mid_left){
                                if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)/6,(right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with mid_left!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)/6,(right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //(QRectF(left_x, top_y + (bottom_y-top_y)/6,(right_x-left_x), (bottom_y-top_y)/6));
                            if(specialTransverseAreaDef == centre_left){
                                if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)/3,(right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with centre_left!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)/3,(right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //(QRectF(left_x, top_y + (bottom_y-top_y)/3,(right_x-left_x), (bottom_y-top_y)/6));
                        }

                        if(specialTransverseAreaDef == right_region){
                            if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/2)))
                            {
#ifdef DEBUGFIELD
                             cerr << "Intersected with right_region!!!" << endl;
#endif
                             specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/2)));
                            }
                        }
                           //QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/2);
                        else
                        {
                            if(specialTransverseAreaDef == centre_right){
                                if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with centre_right!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //QRectF(left_x, top_y + (bottom_y-top_y)/2,(right_x-left_x), (bottom_y-top_y)/6));
                            if(specialTransverseAreaDef == mid_right){
                                if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)*2/3,(right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with mid_right!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)*2/3,(right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //QRectF(left_x, top_y + (bottom_y-top_y)*2/3,(right_x-left_x), (bottom_y-top_y)/6));
                            if(specialTransverseAreaDef == far_right){
                                if(a->intersects(QRectF(left_x, top_y + (bottom_y-top_y)*5/6,(right_x-left_x), (bottom_y-top_y)/6)))
                                {
#ifdef DEBUGFIELD
                                 cerr << "Intersected with far_right!!!" << endl;
#endif
                                 specialAreas.append(a->intersected(QRectF(left_x, top_y + (bottom_y-top_y)*5/6,(right_x-left_x), (bottom_y-top_y)/6)));
                                }
                            }
                                //QRectF(left_x, top_y + (bottom_y-top_y)*5/6,(right_x-left_x), (bottom_y-top_y)/6);
                        }

                    }//end of foreach(int specialTransverseAreaDef, specialTransverseAreasDef)
                }
            } // end of else -> if(if(specialAreaDef == field || specialAreaDef == point)
         } // end of foreach(int specialAreaDef, specialAreasDef)
        delete a;//JAF2014, temporary QRectF for intersections in play_on, direct and indirect_free_kick
    }
}

void SpField::replacePlayers()
{
    replaceBenchPlayers();
    replaceParticipantPlayers();
}

void SpField::replaceBenchPlayers()
{


    int width = benchRect.width();
    int playerSize = 30*width/510;
    int spaceSize = (width - 11*playerSize) / 12;

    foreach(SpPlayer *player, outPlayers)
    {
        int n = player->getNumber();
        player->setScale(getScale());
        player->setPos(benchRect.left()+ spaceSize*(n-1) + playerSize*n , benchRect.center().y() );
    }
}

void SpField::replaceParticipantPlayers()
{
    foreach(SpPlayer *player, inPlayers)
        player->updatePosOnField(getScale());

}



void SpField::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    drawBackgroundContainedArea( painter );
    drawFieldLines( painter );
}

void SpField::drawBackgroundContainedArea( QPainter * painter )
{
    painter->setPen(Qt::NoPen );
    painter->setViewport(this->frameRect());
    painter->fillRect( painter->window(), g_background_brush );
}

void SpField::drawFieldLines(QPainter *painter)
{
    // set screen coordinates of field
    const int left_x   = screenX( - pitch_length * 0.5 );
    const int right_x  = screenX( + pitch_length * 0.5 );
    const int top_y    = screenY( - pitch_width * 0.5 );
    const int bottom_y = screenY( + pitch_width * 0.5 );

    fieldRect = QRect(QPoint(left_x, top_y), QPoint(right_x, bottom_y) );

    //--------------------------------------
    // draw lines
    painter->setPen( g_lines_pen );

    // side lines & goal lines
    painter->drawLine( left_x, top_y, right_x, top_y );
    painter->drawLine( right_x, top_y, right_x, bottom_y );
    painter->drawLine( right_x, bottom_y, left_x, bottom_y );
    painter->drawLine( left_x, bottom_y, left_x, top_y );

    // center line
    painter->drawLine( g_pitch_center.x(), top_y, g_pitch_center.x(), bottom_y );

    // center circle
    const int center_radius = scaleToField( centerCircle_radius );
    painter->drawEllipse( g_pitch_center.x() - center_radius,
                         g_pitch_center.y() - center_radius,
                         center_radius * 2,
                         center_radius * 2 );

    // draw penalty area box
    const int pen_top_y    = screenY( - penaltyArea_width / 2 );
    const int pen_bottom_y = screenY( + penaltyArea_width / 2 );

    // left penalty box
    int pen_x = screenX( -( pitch_length * 0.5 - penaltyArea_length ) );
    painter->drawLine( left_x, pen_top_y, pen_x, pen_top_y );
    painter->drawLine( pen_x, pen_top_y, pen_x, pen_bottom_y );
    painter->drawLine( pen_x, pen_bottom_y, left_x, pen_bottom_y );

    // right penalty box
    pen_x = screenX( +( pitch_length * 0.5 - penaltyArea_length ) );
    painter->drawLine( right_x, pen_top_y, pen_x, pen_top_y );
    painter->drawLine( pen_x, pen_top_y, pen_x, pen_bottom_y );
    painter->drawLine( pen_x, pen_bottom_y, right_x, pen_bottom_y );

    // draw goal area box
    const int goal_area_y_abs = scaleToField( goalArea_width *0.5 );
    const int goal_area_top_y = g_pitch_center .y() - goal_area_y_abs;
    const int goal_area_bottom_y = g_pitch_center.y() + goal_area_y_abs;

    // left goal area
    int goal_area_x = screenX( - pitch_length * 0.5 + goalArea_length );
    painter->drawLine( left_x, goal_area_top_y, goal_area_x, goal_area_top_y );
    painter->drawLine( goal_area_x, goal_area_top_y, goal_area_x, goal_area_bottom_y );
    painter->drawLine( goal_area_x, goal_area_bottom_y, left_x, goal_area_bottom_y );

    // right goal area
    goal_area_x = screenX( pitch_length * 0.5 - goalArea_length );
    painter->drawLine( right_x, goal_area_top_y, goal_area_x, goal_area_top_y );
    painter->drawLine( goal_area_x, goal_area_top_y, goal_area_x, goal_area_bottom_y );
    painter->drawLine( goal_area_x, goal_area_bottom_y, right_x, goal_area_bottom_y );

    //--------------------------------------
    // draw goal boxes
    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );
    const int goal_top_y = screenY( - goal_width * 0.5 );
    const int goal_size_x = scaleToField( goal_depth );
    const int goal_size_y = scaleToField( goal_width );

    // left goal
    painter->drawRect( screenX( - pitch_length * 0.5 - goal_depth),
                      goal_top_y, goal_size_x, goal_size_y );

    // right goal
    painter->drawRect( screenX( pitch_length * 0.5 ) + 1,
                      goal_top_y, goal_size_x + 1, goal_size_y );

    //bench
    painter->setPen(QColor(82,41,13));
    painter->setBrush(QBrush(QColor(92,51,23)));
    painter->drawRect(benchRect);

    //special area
    painter->setPen(Qt::black);
    if(!setPlay->getCurrentStep()->getIdNumber() )
    {
        painter->setBrush(Qt::DiagCrossPattern);
        foreach(QRectF sa, specialAreas)
            if(sa != fieldRect)
                painter->drawRect(sa);
    }
}

void SpField::setFocusPoint( const QPoint & pos )
{
    g_focus_point = pos;
    updateSize();
    update();
}

void SpField::zoomIn()
{
    if(g_pitch_scale *1.3 <=  20)
    {
        g_pitch_scale *= 1.3;
        g_zoomed = true;
        updateSize();
        update();
    }
}

void SpField::zoomOut()
{
    if(g_pitch_scale / 1.3 >=  2)
    {
        g_pitch_scale /= 1.3;
        g_zoomed = true;
        updateSize();
        update();
    }
}

void SpField::fitSize()
{
    g_zoomed = false;
    g_focus_point = QPoint(0.0, 0.0 );
    updateSize();
    update();
}

void SpField::updateSize()
{
    g_pitch_center.setX( this->width() / 2 - scaleToField( g_focus_point.x()  ) );
    g_pitch_center.setY( (this->height()) / 2  +  bench_width * 2- scaleToField(  g_focus_point.y()  )  );

    const double pitch_l = ( pitch_length + pitch_margin*2  );
    const double pitch_w = ( pitch_width + pitch_margin*3 + bench_width);

    if ( ! g_zoomed )
    {
        g_pitch_scale  = this->width() / pitch_l;

        if ( pitch_w * g_pitch_scale > this->height() )
        {
            g_pitch_scale = this->height() / pitch_w;
        }
    }
    positionLabel->setPos(g_pitch_center.x() - 90,sceneRect().bottom()-20);
}


void SpField::mouseMoveEvent(QMouseEvent* e)
{

    if(drawingAction != NULL)
    {
        QPointF mousePos = e->posF();
        QPointF point = mousePos;

        if (!fieldRect.contains(point) )
        {
            point.setX(qMin(fieldRect.right(), qMax(point.x(), fieldRect.left())));
            point.setY(qMin(fieldRect.bottom(), qMax(point.y(), fieldRect.top())));
            setToolTip(QString("Destination of action will stay inside field..."));
            drawingAction->setDest(point);
            drawingAction->update();
            this->setCursor(Qt::ForbiddenCursor);
            update();
            return ;
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
            update();
        }

        int actType = drawingAction->getType();

        if(actType == SpAction::pass || actType == SpAction::passForward)
        {
            if(actType == SpAction::pass )
                setToolTip(QString("Choose target player moving mouse pointer to him..."));
            else
                setToolTip(QString("Choose point to pass the ball..."));

            QPointF actorPoint = drawingAction->getActor()->getPosition()->getRealPoint();


            foreach(SpPlayer * player , inPlayers)
            {

                if(player->contains(player->mapFromScene(point)))
                {
                    if(player == drawingAction->getActor()         )
                    {
                        setPlay->setStatusMessage(QString("Can't choice the ball holder as target!..."));
                        player->setHighLight(false);
                        player->setCursor(Qt::ForbiddenCursor);
                           update();
                        break;
                    }
                    QLineF *line;
                    if(actType == SpAction::pass)
                        line = new QLineF(actorPoint, player->getNextPosition()->getRealPoint());
                    else
                        line = new QLineF(actorPoint, drawingAction->getSpDest()->getRealPoint());

                    if(line->length() <= SpAction::maxBallDistance)
                    {
                        player->setHighLight(true);
                        setPlay->setStatusMessage(QString("If is the player") + QString::number(player->getNumber())
                                                  + QString("the target player press left mouse button..."));
                        player->setCursor(Qt::ArrowCursor);

                    }
                    else
                    {
                        setPlay->setStatusMessage(QString("Can't choice this player, it's to far away (distance > 35)..."));
                        player->setHighLight(false);
                        player->setCursor(Qt::ForbiddenCursor);
                    }
                    update();
                    break;
                }
                else
                {
                    player->setCursor(Qt::ArrowCursor);
                    player->setHighLight(false);
                }
            }

        }
        else if(actType == SpAction::goToOffsideLine)
        {
            double xMax = 0;
            foreach(SpPlayer * player , inPlayers)
            {
                if(player->isGhost())
                    continue;
                xMax = qMax(player->pos().x(), xMax);
            }
            point.setX( offSideLineX);

        }

        drawingAction->setDest(point);
        drawingAction->update();
    }

    if (fieldRect.contains(e->posF() ))
    {
        //TODO..
        QPointF p = SpPoint::roundPoint(this, QPointF(e->pos().x(), e->pos().y()));
        QString x = QString("%1").arg(p.x(), 5);
        QString y = QString("%1").arg(p.y(), 5);
        QString positionText = QString("   X: %1 , Y: %2" ).arg(x, y);
        positionLabel->setText(positionText);
    }
    else
          positionLabel->setText( "... outside the field ...");

    return QGraphicsView::mouseMoveEvent(e);
}

bool SpField::canRemove(SpPlayer *player)
{
    foreach (SpPlayer *p, inPlayers)
    {
        if(p->getPosition()->isRelativeTo(player))
            return false;
        SpAction *act = p->getAction();
        if(act)
        {

            if( act->getSpDest()->isRelativeTo(player)  ||
                act->getDestActor() == player           ||
                (act->getActor() == player && act->getDestActor() ) )
                return false;
        }
    }
        return true;
}


bool SpField::canMove(SpPlayer *player)
{

    foreach (SpPlayer *p, inPlayers)
    {
        SpAction *act = p->getAction();
        if(act)
        {
            QLineF line;
            if(act->getDestActor() == player)
                line = QLineF(p->getPosition()->getRealPoint(),
                        player->getNextPosition()->getRealPoint());

            else if(act->getActor() == player && act->getDestActor())
                line = QLineF(player->getPosition()->getRealPoint(),
                        act->getDestActor()->getNextPosition()->getRealPoint());

            if(line.length() > SpAction::maxBallDistance)
                return false;

        }
    }

    return true;
}

bool SpField::actualizePlayerPositions(SpPlayer *player, bool movePlayers, bool final)
{

    if(movePlayers)
    {
        foreach (SpPlayer *p, inPlayers)
            if(p == player)
                continue;
            else if(! p->calcNewPosition(movePlayers))
                return false;
    }

    foreach (SpPlayer *p, inPlayers)
    {
        if(p == player)
            continue;
        SpAction *act = p->getAction();
        if(act)
        {
            SpPoint *sd = new SpPoint(act->getSpDest());
            sd->actualize();
            if(!isInsideField( sd->getScreenPoint()))
                return false;
        }
    }

    foreach (SpPlayer *p, inPlayers)
        if(p != player)
            p->changePosition(movePlayers, final);

    foreach (SpPlayer *p, inPlayers)
    {
        SpAction *act = p->getAction();
        if(act)
        {
            act->actualize();
        }
    }

    return true;
}

void SpField::itemMoved()
{
    double xMax = 0;
    double xHolder = fieldRect.right();
    foreach (SpPlayer *p, inPlayers)
    {
        if(p->isGhost())
            continue;

        if(p->isTheBallOwner())
            xHolder = p->pos().x();
        xMax = qMax(p->pos().x(), xMax);

        p->actualizePosition();

    }

    xMax = xHolder == fieldRect.right() ? xMax : xHolder;
    offSideLineX = qMax( fieldRect.center().x(),
                      qMin( fieldRect.right(), xMax + fieldRect.width() / 12));
}

void SpField::mousePressEvent(QMouseEvent* e)
{
    setPlay->clearStatusMessage();
    this->setCursor(Qt::ArrowCursor);
    if( drawingAction != NULL )
    {
        scene()->removeItem(drawingAction);

         if(e->button() == Qt::LeftButton)
        {
            QPointF mousePos = e->posF();
            QPointF point = mousePos;
            SpAction * newAction = new SpAction(drawingAction->getActor(), drawingAction->getType(), this);
            if (!fieldRect.contains(point) )
            {
                 point.setX(qMin(fieldRect.right(), qMax(point.x(), fieldRect.left())));
                 point.setY(qMin(fieldRect.bottom(), qMax(point.y(), fieldRect.top())));
            }
            int actType = drawingAction->getType();
            if(actType == SpAction::pass || actType == SpAction::passForward)
            {

                QPointF actorPoint = drawingAction->getActor()->getPosition()->getRealPoint();
                if(actType == SpAction::pass)
                {
                    foreach(SpPlayer * player , inPlayers)
                    {
                        player->setHighLight(false);
                        if(player == drawingAction->getActor())
                            continue;

                            if(player->contains(player->mapFromScene(point)))
                            {
                                SpAction *a = player->getAction();
                                if(a && a->getType() == SpAction::goToOffsideLine)
                                    break;
                                QLineF *line = new QLineF(actorPoint, player->getNextPosition()->getRealPoint());

                                if(line->length() <= SpAction::maxBallDistance)
                                {
                                    newAction->setDestPlayer(player);
                                    newAction->actualize();
                                    drawingAction->getActor()->addAction(newAction);
                                    if(player->isGhost())
                                        player->cameFromGhost();

                                }
                                else
                                    break;
                            }
                    }
                }
                else
                {
                    QLineF *line = new QLineF(actorPoint, drawingAction->getSpDest()->getRealPoint());
                    if(line->length() <= SpAction::maxBallDistance)
                    {
                        newAction->setDestPlayer(drawingAction->getDestActor());
                        newAction->actualize();
                        if(drawingAction->getDestActor()->isGhost())
                            drawingAction->getDestActor()->cameFromGhost();
                        drawingAction->getActor()->addAction(newAction);
                        if(actType == SpAction::passForward)
                        {
                            SpAction *a = new SpAction(drawingAction->getDestActor(),SpAction::run, this);
                            a->setDest(drawingAction->getSpDest()->getScreenPoint());
                            drawingAction->getDestActor()->addAction(a);
                        }
                    }
                }


            }
            else if(actType == SpAction::goToOffsideLine)
            {
                double xMax = 0;
                foreach(SpPlayer * player , inPlayers)
                {
                    if(player->isGhost())
                        continue;
                    xMax = qMax(player->pos().x(), xMax);
                }
                point.setX( offSideLineX);
                newAction->setDest(point);
                drawingAction->getActor()->addAction(newAction);
            }
            else
            {
                newAction->setDest(point);
                drawingAction->getActor()->addAction(newAction);
            }

        }
        drawingAction = NULL;
    }
    else
        return QGraphicsView::mousePressEvent(e);
}

void SpField::mouseDoubleClickEvent( QMouseEvent * event )
{
    /*if ( event->button() == Qt::LeftButton )
    {
        setFocusPoint( event->pos() );
        update();
    }*/
    //TODO
    return QGraphicsView::mouseDoubleClickEvent(event);
}

void SpField::wheelEvent(QWheelEvent *event)
{
   // -event->delta() > 0 ? zoomOut() : zoomIn();
    //TODO ZOOMS...
}


void SpField::resizeEvent (QResizeEvent *)
{
    scene()->setSceneRect(frameRect());
    updateSize();
    calculateRects();
    replacePlayers();
    setPlay->resizeSPLA();
}

SpPlayer * SpField::getCurrentBallPlayerHolder()
{
    return setPlay->getCurrentBallHolderPlayer();
}

