/***************************************************************************//**
 * @file qfi_NAV.cpp
 * @author  Marek M. Cel <marekcel@marekcel.pl>
 *
 * @section LICENSE
 *
 * Copyright (C) 2015 Marek M. Cel
 *
 * This file is part of QFlightInstruments. You can redistribute and modify it
 * under the terms of GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Further information about the GNU General Public License can also be found
 * on the world wide web at http://www.gnu.org.
 *
 * ---
 *
 * Copyright (C) 2015 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#include "Nav.hpp"

#include <QGraphicsSvgItem>

#include <iostream>
#include <cmath>

namespace qfi {

Nav::Nav(QWidget* parent) : QGraphicsView(parent)
{
#ifdef WIN32
    m_crsTextFont.setFamily( "Courier" );
    m_crsTextFont.setPointSizeF( 12.0f );
    m_crsTextFont.setStretch( QFont::Condensed );
    m_crsTextFont.setWeight( QFont::Bold );

    m_hdgTextFont.setFamily( "Courier" );
    m_hdgTextFont.setPointSizeF( 12.0f );
    m_hdgTextFont.setStretch( QFont::Condensed );
    m_hdgTextFont.setWeight( QFont::Bold );

    m_dmeTextFont.setFamily( "Courier" );
    m_dmeTextFont.setPointSizeF( 10.0f );
    m_dmeTextFont.setStretch( QFont::Condensed );
    m_dmeTextFont.setWeight( QFont::Bold );
#else
    m_crsTextFont.setFamily( "courier" );
    m_crsTextFont.setPointSizeF( 12.0f );
    m_crsTextFont.setStretch( QFont::Condensed );
    m_crsTextFont.setWeight( QFont::Bold );

    m_hdgTextFont.setFamily( "courier" );
    m_hdgTextFont.setPointSizeF( 12.0f );
    m_hdgTextFont.setStretch( QFont::Condensed );
    m_hdgTextFont.setWeight( QFont::Bold );

    m_dmeTextFont.setFamily( "courier" );
    m_dmeTextFont.setPointSizeF( 10.0f );
    m_dmeTextFont.setStretch( QFont::Condensed );
    m_dmeTextFont.setWeight( QFont::Bold );
#endif

    m_scene = new QGraphicsScene( this );
    setScene( m_scene );
    m_scene->clear();
    init();
}

Nav::~Nav()
{
    if (m_scene) {
        m_scene->clear();
        delete m_scene;
        m_scene = nullptr;
    }

    reset();
}

void Nav::reinit()
{
    if (m_scene) {
        m_scene->clear();
        init();
    }
}

void Nav::update()
{
    updateView();

    m_devBarDeltaX_old = m_devBarDeltaX_new;
    m_devBarDeltaY_old = m_devBarDeltaY_new;
}

void Nav::setHeading(const float heading)
{
    m_heading = heading;

    while ( m_heading <   0.0f ) m_heading += 360.0f;
    while ( m_heading > 360.0f ) m_heading -= 360.0f;
}

void Nav::setHeadingBug(const float headingBug)
{
    m_headingBug = headingBug;

    while ( m_headingBug <   0.0f ) m_headingBug += 360.0f;
    while ( m_headingBug > 360.0f ) m_headingBug -= 360.0f;
}

void Nav::setCourse(const float course)
{
    m_course = course;

    while ( m_course <   0.0f ) m_course += 360.0f;
    while ( m_course > 360.0f ) m_course -= 360.0f;
}

void Nav::setBearing(const float bearing, const bool visible)
{
    m_bearing        = bearing;
    m_bearingVisible = visible;

    while ( m_bearing <   0.0f ) m_bearing += 360.0f;
    while ( m_bearing > 360.0f ) m_bearing -= 360.0f;
}

void Nav::setDeviation(const float deviation, const bool visible)
{
    m_deviation        = deviation;
    m_deviationVisible = visible;

    if ( m_deviation < -1.0f ) m_deviation = -1.0f;
    if ( m_deviation >  1.0f ) m_deviation =  1.0f;
}

void Nav::setDistance(const float distance, const bool visible)
{
    m_distance        = fabs( distance );
    m_distanceVisible = visible;
}

void Nav::resizeEvent( QResizeEvent* event)
{
    QGraphicsView::resizeEvent( event );
    reinit();
}

void Nav::init()
{
    m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
    m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

    m_itemBack = new QGraphicsSvgItem( ":/qfi/images/nav/nav_back.svg" );
    m_itemBack->setCacheMode( QGraphicsItem::NoCache );
    m_itemBack->setZValue( m_backZ );
    m_itemBack->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_scene->addItem( m_itemBack );

    m_itemMask = new QGraphicsSvgItem( ":/qfi/images/nav/nav_mask.svg" );
    m_itemMask->setCacheMode( QGraphicsItem::NoCache );
    m_itemMask->setZValue( m_maskZ );
    m_itemMask->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_scene->addItem( m_itemMask );

    m_itemMark = new QGraphicsSvgItem( ":/qfi/images/nav/nav_mark.svg" );
    m_itemMark->setCacheMode( QGraphicsItem::NoCache );
    m_itemMark->setZValue( m_markZ );
    m_itemMark->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_scene->addItem( m_itemMark );

    m_itemBrgArrow = new QGraphicsSvgItem( ":/qfi/images/nav/nav_brg_arrow.svg" );
    m_itemBrgArrow->setCacheMode( QGraphicsItem::NoCache );
    m_itemBrgArrow->setZValue( m_brgArrowZ );
    m_itemBrgArrow->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemBrgArrow->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemBrgArrow );

    m_itemCrsArrow = new QGraphicsSvgItem( ":/qfi/images/nav/nav_crs_arrow.svg" );
    m_itemCrsArrow->setCacheMode( QGraphicsItem::NoCache );
    m_itemCrsArrow->setZValue( m_crsArrowZ );
    m_itemCrsArrow->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemCrsArrow->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemCrsArrow );

    m_itemDevBar = new QGraphicsSvgItem( ":/qfi/images/nav/nav_dev_bar.svg" );
    m_itemDevBar->setCacheMode( QGraphicsItem::NoCache );
    m_itemDevBar->setZValue( m_devBarZ );
    m_itemDevBar->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemDevBar->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemDevBar );

    m_itemDevScale = new QGraphicsSvgItem( ":/qfi/images/nav/nav_dev_scale.svg" );
    m_itemDevScale->setCacheMode( QGraphicsItem::NoCache );
    m_itemDevScale->setZValue( m_devScaleZ );
    m_itemDevScale->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemDevScale->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemDevScale );

    m_itemHdgBug = new QGraphicsSvgItem( ":/qfi/images/nav/nav_hdg_bug.svg" );
    m_itemHdgBug->setCacheMode( QGraphicsItem::NoCache );
    m_itemHdgBug->setZValue( m_hdgBugZ );
    m_itemHdgBug->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemHdgBug->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemHdgBug );

    m_itemHdgScale = new QGraphicsSvgItem( ":/qfi/images/nav/nav_hdg_scale.svg" );
    m_itemHdgScale->setCacheMode( QGraphicsItem::NoCache );
    m_itemHdgScale->setZValue( m_hdgScaleZ );
    m_itemHdgScale->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemHdgScale->setTransformOriginPoint( m_originalNavCtr );
    m_scene->addItem( m_itemHdgScale );

    m_itemCrsText = 0;

    m_itemCrsText = new QGraphicsTextItem( QString( "CRS 999" ) );
    m_itemCrsText->setCacheMode( QGraphicsItem::NoCache );
    m_itemCrsText->setZValue( m_crsTextZ );
    m_itemCrsText->setDefaultTextColor( m_crsTextColor );
    m_itemCrsText->setFont( m_crsTextFont );
    m_itemCrsText->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemCrsText->moveBy( m_scaleX * ( m_originalCrsTextCtr.x() - m_itemCrsText->boundingRect().width()  / 2.0f ),
                           m_scaleY * ( m_originalCrsTextCtr.y() - m_itemCrsText->boundingRect().height() / 2.0f ) );
    m_scene->addItem( m_itemCrsText );

    m_itemHdgText = new QGraphicsTextItem( QString( "HDG 999" ) );
    m_itemHdgText->setCacheMode( QGraphicsItem::NoCache );
    m_itemHdgText->setZValue( m_hdgTextZ );
    m_itemHdgText->setDefaultTextColor( m_hdgTextColor );
    m_itemHdgText->setFont( m_hdgTextFont );
    m_itemHdgText->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemHdgText->moveBy( m_scaleX * ( m_originalHdgTextCtr.x() - m_itemHdgText->boundingRect().width()  / 2.0f ),
                           m_scaleY * ( m_originalHdgTextCtr.y() - m_itemHdgText->boundingRect().height() / 2.0f ) );
    m_scene->addItem( m_itemHdgText );

    m_itemDmeText = new QGraphicsTextItem( QString( "99.9 NM" ) );
    m_itemDmeText->setCacheMode( QGraphicsItem::NoCache );
    m_itemDmeText->setZValue( m_dmeTextZ );
    m_itemDmeText->setDefaultTextColor( m_dmeTextColor );
    m_itemDmeText->setFont( m_dmeTextFont );
    m_itemDmeText->setTransform( QTransform::fromScale( m_scaleX, m_scaleY ), true );
    m_itemDmeText->moveBy( m_scaleX * ( m_originalDmeTextCtr.x() - m_itemDmeText->boundingRect().width()  / 2.0f ),
                           m_scaleY * ( m_originalDmeTextCtr.y() - m_itemDmeText->boundingRect().height() / 2.0f ) );
    m_scene->addItem( m_itemDmeText );

    updateView();
}

void Nav::reset()
{
    m_itemBrgArrow = nullptr;
    m_itemCrsArrow = nullptr;
    m_itemDevBar   = nullptr;
    m_itemDevScale = nullptr;
    m_itemHdgBug   = nullptr;
    m_itemHdgScale = nullptr;

    m_itemCrsText = nullptr;
    m_itemHdgText = nullptr;
    m_itemDmeText = nullptr;

    m_heading    = 0.0f;
    m_headingBug = 0.0f;
    m_course     = 0.0f;
    m_bearing    = 0.0f;
    m_deviation  = 0.0f;
    m_distance   = 0.0f;

    m_bearingVisible   = true;
    m_deviationVisible = true;
    m_distanceVisible  = true;

    m_devBarDeltaX_new = 0.0f;
    m_devBarDeltaX_old = 0.0f;
    m_devBarDeltaY_new = 0.0f;
    m_devBarDeltaY_old = 0.0f;
}

void Nav::updateView()
{
    m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
    m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

    m_itemCrsArrow->setRotation( -m_heading + m_course );
    m_itemHdgBug->setRotation( -m_heading + m_headingBug );
    m_itemHdgScale->setRotation( -m_heading );

    if (m_bearingVisible) {
        m_itemBrgArrow->setVisible( true );
        m_itemBrgArrow->setRotation( -m_heading + m_bearing );
    } else {
        m_itemBrgArrow->setVisible( false );
    }

    if ( m_deviationVisible ) {
        m_itemDevBar->setVisible( true );
        m_itemDevScale->setVisible( true );

        float angle_deg = -m_heading + m_course;
#ifndef M_PI
        const float angle_rad{static_cast<float>(3.14159265358979323846 * angle_deg / 180.0f)};
#else
        const float angle_rad{static_cast<float>(M_PI * angle_deg / 180.0f)};
#endif
        const float sinAngle{static_cast<float>(std::sin(angle_rad))};
        const float cosAngle{static_cast<float>(std::cos(angle_rad))};

        m_itemDevBar->setRotation( angle_deg );
        m_itemDevScale->setRotation( angle_deg );

        const float delta{static_cast<float>(m_originalPixPerDev * m_deviation)};

        m_devBarDeltaX_new = m_scaleX * delta * cosAngle;
        m_devBarDeltaY_new = m_scaleY * delta * sinAngle;

        m_itemDevBar->moveBy( m_devBarDeltaX_new - m_devBarDeltaX_old, m_devBarDeltaY_new - m_devBarDeltaY_old );
    } else {
        m_itemDevBar->setVisible( false );
        m_itemDevScale->setVisible( false );

        m_devBarDeltaX_new = m_devBarDeltaX_old;
        m_devBarDeltaY_new = m_devBarDeltaY_old;
    }

    m_itemCrsText->setPlainText( QString("CRS %1").arg( m_course     , 3, 'f', 0, QChar('0') ) );
    m_itemHdgText->setPlainText( QString("HDG %1").arg( m_headingBug , 3, 'f', 0, QChar('0') ) );

    if ( m_distanceVisible ) {
        m_itemDmeText->setVisible( true );
        m_itemDmeText->setPlainText( QString("%1 NM").arg( m_distance, 5, 'f', 1, QChar(' ') ) );
    } else {
        m_itemDmeText->setVisible( false );
    }

    m_scene->update();

    centerOn( width() / 2.0f , height() / 2.0f );
}

}
