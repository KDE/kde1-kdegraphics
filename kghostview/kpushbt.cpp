/****************************************************************************
** $Id$
**
** Implementation of KPushButton class
**
** Created : 940221
**
** Copyright (C) 1992-1996 Troll Tech AS.  All rights reserved.
**
** This file is part of the non-commercial distribution of Qt 1.1.
**
** See the file LICENSE included in the distribution for the usage
** and distribution terms, or http://www.troll.no/qt/license.html.
**
** (This file differs from the one in the commercial distribution of
** Qt only by this comment.)
**
*****************************************************************************/

#include "kpushbt.h"
#include <qwidget.h>
#include <qpushbt.h>
#include <qdialog.h>
#include <qfontmet.h>
#include <qpainter.h>
#include <qdrawutl.h>
#include <qpixmap.h>
#include <qpmcache.h>

#include "kpushbt.moc"

const int extraMotifWidth = 10;
const int extraMotifHeight = 10;


KPushButton::KPushButton(  const char *name, QWidget *parent)
	: QPushButton( parent, name )
{
}



void KPushButton::drawButton( QPainter *paint )
{
    register QPainter *p = paint;
    GUIStyle	gs = style();
    QColorGroup g  = colorGroup();
    bool	updated = isDown();
    QColor	fillcol = g.background();
    int		x1, y1, x2, y2;

    rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

#define SAVE_PUSHBUTTON_PIXMAPS
#if defined(SAVE_PUSHBUTTON_PIXMAPS)
    QString pmkey;				// pixmap key
    int w, h;
    w = x2 + 1;
    h = y2 + 1;
    pmkey.sprintf( "$qt_push_%d_%d_%d_%d_%d_%d_%d", gs,
		   palette().serialNumber(), isDown(), 0, w, h,
		   isToggleButton() && isOn() );
    QPixmap *pm = QPixmapCache::find( pmkey );
    QPainter pmpaint;
    if ( pm ) {					// pixmap exists
	QPixmap pm_direct = *pm;
	pmpaint.begin( &pm_direct );
	pmpaint.drawPixmap( 0, 0, *pm );
	if ( text() )
	    pmpaint.setFont( font() );
	drawButtonLabel( &pmpaint );
	pmpaint.end();
	p->drawPixmap( 0, 0, pm_direct );
	
	if ( hasFocus() ) {
	    if ( style() == WindowsStyle ) {
		p->drawWinFocusRect( x1+3, y1+3, x2-x1-5, y2-y1-5 );
	    } else {
		p->setPen( black );
		p->drawRect( x1+3, y1+3, x2-x1-5, y2-y1-5 );
	    }
	}
	return;
    }
    bool use_pm = TRUE;
    if ( use_pm ) {
	pm = new QPixmap( w, h );		// create new pixmap
	CHECK_PTR( pm );
	pmpaint.begin( pm );
	p = &pmpaint;				// draw in pixmap
	p->setBackgroundColor( fillcol );
	p->eraseRect( 0, 0, w, h );
    }
#endif

    p->setPen( g.foreground() );
    p->setBrush( QBrush(fillcol,NoBrush) );

    if ( gs == WindowsStyle ) {		// Windows push button
	if ( isDown() ) {
	    if ( 0 ) {
		p->setPen( black );
		p->drawRect( x1, y1, x2-x1+1, y2-y1+1 );
		p->setPen( g.dark() );
		p->drawRect( x1+1, y1+1, x2-x1-1, y2-y1-1 );
	    }
	    else
		qDrawWinButton( p, x1, y1, w, h, g, TRUE );
	} else {
	    if ( 0 ) {
		p->setPen( black );
		p->drawRect( x1, y1, w, h );
		x1++; y1++;
		x2--; y2--;
	    }
	    if ( isToggleButton() && isOn() ) {
		qDrawWinButton( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE );
		if ( updated ) {
		    p->setPen( NoPen );
		    p->setBrush( g.mid() );
		    p->drawRect( x1+1, y1+1, x2-x1-2, y2-y1-2 );
		    updated = FALSE;
		}
	    } else {
		//qDrawWinButton( p, x1, y1, x2-x1+1, y2-y1+1, g, FALSE );
	    }
	}
	if ( updated )
	    p->fillRect( x1+1, y1+1, x2-x1-1, y2-y1-1, g.background() );
    }
    else if ( gs == MotifStyle ) {		// Motif push button
	if ( 0 ) {			// default Motif button
	    qDrawShadePanel( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE );
	    x1 += extraMotifWidth/2;
	    y1 += extraMotifHeight/2;
	    x2 -= extraMotifWidth/2;
	    y2 -= extraMotifHeight/2;
	}
	QBrush fill( fillcol );
	if ( isDown() ) {
	    qDrawShadePanel( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE, 2,
			     updated ? &fill : 0 );
	} else if ( isToggleButton() && isOn() ) {
	    qDrawShadePanel( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE, 2, 0 );
	    if ( updated ) {
		p->setPen( NoPen );
		p->setBrush( g.mid() );
		p->drawRect( x1+2, y1+2, x2-x1-3, y2-y1-3 );
	    }
	} else {
	   // qDrawShadePanel( p, x1, y1, x2-x1+1, y2-y1+1, g, FALSE, 2,
		//	     updated ? &fill : 0 );
	}
    }
    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );

#if defined(SAVE_PUSHBUTTON_PIXMAPS)
    if ( use_pm ) {
	pmpaint.end();
	p = paint;				// draw in default device
	p->drawPixmap( 0, 0, *pm );
	QPixmapCache::insert( pmkey, pm );	// save for later use
    }
#endif
    drawButtonLabel( p );
    if ( gs == MotifStyle && hasFocus() ) {
	p->setPen( black );
	p->drawRect( x1+3, y1+3, x2-x1-5, y2-y1-5 );
    }
    
}


