/****************************************************************************
**
** A simple widget to mark and select entries in a list.
**
** Copyright (C) 1997 by Markku Hihnala. 
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include "marklist.h"
#include <qtabdlg.h>
#include <qmlined.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <qlabel.h>
#include <qcombo.h>
#include <qlayout.h>
#include <stdio.h>
#include <qstring.h>
#include <qpainter.h>
#include <qdrawutl.h>
#include <kapp.h>
#include <kpopmenu.h>

MarkList::MarkList( QWidget * parent , const char * name )
	: QTableView( parent, name ), sel(-1), drag(-1), items()
{
	setFrameStyle( Panel | Sunken );
        setTableFlags( Tbl_autoVScrollBar | Tbl_cutCellsV
                         | Tbl_snapToVGrid | Tbl_clipCellPainting);
        setLineWidth( 1 );
        setCellHeight( fontMetrics().lineSpacing()+4 );
   	setNumCols( 3 );
//	setBackgroundColor( colorGroup().normal().light(120) );
	pup = new QPopupMenu(0, "pup");
	pup->insertItem( i18n("Mark current page"), this, SLOT(markSelected()) );
	pup->insertItem( i18n("Mark all pages"), this, SLOT(markAll()) );
	pup->insertItem( i18n("Mark even pages"), this, SLOT(markEven()) );
	pup->insertItem( i18n("Mark odd pages"), this, SLOT(markOdd()) );
	pup->insertItem( i18n("Toggle page marks"), this, SLOT(toggleMarks()) );
	pup->insertItem( i18n("Remove page marks"), this, SLOT(removeMarks()) );
//	items.setAutoDelete( TRUE );
	selectColor = QColor( black );
	selectTextColor = QColor( white );
	
}

void	MarkList::insertItem ( const char *text, int index)
{
	MarkListItem *mli = new MarkListItem( text );
	items.insert( index, mli );
	setNumRows( items.count() );
}

void	MarkList::setAutoUpdate ( bool enable)
{
	QTableView::setAutoUpdate( enable );
	if (enable) update();
}

void	MarkList::clear()
{
	QColorGroup cg = QApplication::palette()->normal();
	if( backgroundColor() != cg.base() )
		setBackgroundColor(cg.base());
	items.clear();
	sel = -1;
	update();
}

int	MarkList::cellWidth( int col )
{
//	return col==0 ? 16 : fontMetrics().maxWidth()*6;
	if( col==0 )
		return 16;
	else if ( col==1 )
		return cellHeight();
	else
		return width()-cellHeight()-16-2*frameWidth();
}

void MarkList::paintCell( QPainter *p, int row, int col)
{
	QColorGroup cg = QApplication::palette()->normal();
	
	if ( col == 0  )
	{
	
		if ( items.at( row )->select() )
			{
				p->setBrush( selectColor );
				p->setPen( selectColor );
			}
		else {
			p->setBrush(cg.base());
				p->setPen(cg.base());
		}	
		p->drawRect(0,0,cellWidth(0),cellHeight());
		
		if( items.at( row )->mark() ) {
		
		int xOffset=6; int yOffset=3;
		
		if ( items.at( row )->select() )
			p->setPen( selectTextColor );
		else
			p->setPen( cg.text() );
		p->drawLine( xOffset+4, yOffset, xOffset+4, yOffset+9 );
		p->setPen( red );
		p->drawLine( xOffset+3, yOffset+1, xOffset, yOffset+4 );
		p->drawLine( xOffset+3, yOffset+1, xOffset+3, yOffset+4 );
		p->drawLine( xOffset, yOffset+4, xOffset+3, yOffset+4 );
		} else {
			int xOffset=4; int yOffset=5;
			
			p->setPen( cg.dark() );
			p->setBrush( cg.dark() );
			p->drawEllipse( xOffset+4, yOffset, 4, 4 );
			
			//p->setPen( cg.mid() );
			//p->setBrush( cg.mid() );
			//p->drawEllipse( xOffset+5, yOffset+1, 2, 2 );
			
			p->setPen( white );
			p->drawPoint( xOffset+5, yOffset+1);
		}	
	}

	if ( col == 1 )
	{
		if ( items.at( row )->select() )
		{
			p->setBrush(selectColor);
			p->setPen(selectColor);
			p->drawRect(0,0,cellWidth(1),cellHeight());
			
			//qDrawShadePanel( p, 3, 2, cellHeight( 1 ) - 6, cellHeight(1)-4,
			//	cg, FALSE, 1, &QBrush(colorGroup().light()));
			
			p->setBrush(white);
			p->setPen(black);	
			p->drawRect(3,2,cellHeight( 1 ) - 7, cellHeight(1)-5);
			
		} else {
			
			p->setBrush(cg.base());
				p->setPen(cg.base());
			p->drawRect(0,0,cellWidth(1),cellHeight());
			
			//qDrawShadePanel( p, 3, 2, cellHeight( 1 ) - 6, cellHeight(1)-4,
			//	cg, FALSE,1 );
				
			p->setBrush(white);
			p->setPen(black);	
			p->drawRect(3,2,cellHeight( 1 ) - 7, cellHeight(1)-5);
			
		}

		/*QFontMetrics fm = p->fontMetrics();
		int yPos;   // vertical text position
		if ( 10 < fm.height() )
			yPos = fm.ascent() + fm.leading()/2;
		else
			yPos = 5 - fm.height()/2 + fm.ascent();
		p->drawText( 4, yPos, items.at( row )->text() );*/
	}
	
	if ( col == 2 )
	{
		if ( items.at( row )->select() )
			{
				p->setBrush(selectColor);
				p->setPen(selectColor);
			}
		else {
			p->setBrush(cg.base());
				p->setPen(cg.base());
		}	
		p->drawRect(0,0,cellWidth(2),cellHeight());
		
		
		/*  if ( items.at( row )->select() )
		{
			QColorGroup cg = QApplication::palette()->normal();
			qDrawShadePanel( p, 0, 0, cellWidth( 1 ) - 1, cellHeight(),
				cg, FALSE, 1, &QBrush(colorGroup().light()));
		}*/
		
		if ( items.at( row )->select() )
			p->setPen(selectTextColor);
		else
			p->setPen(cg.text());
		QFontMetrics fm = p->fontMetrics();
		int yPos;   // vertical text position
		/*if ( 10 < fm.height() )
			yPos = fm.ascent() + fm.leading()/2;
		else*/
			yPos = cellHeight()-fm.leading()/2;
			yPos = fm.ascent() + fm.leading()/2+1;
		p->drawText( 2, yPos, items.at( row )->text() );
	}
}

void MarkList::mousePressEvent ( QMouseEvent *e )
{
	int i = findRow( e->pos().y() );
	if ( i == -1 )
		return;
	MarkListItem *it = items.at( i );

	if ( e->button() == LeftButton )
		select( i );
	else if ( e->button() == RightButton )
		pup->popup( mapToGlobal( e->pos() ) );
	else if ( e->button() == MidButton )
	{
		it->setMark( !it->mark() );
		updateCell( i, 0 );
		drag = i;
	}
}

void MarkList::mouseMoveEvent ( QMouseEvent *e )
{
	if (e->state() != MidButton)
		return;

	int i = findRow( e->pos().y() );
	if ( i == drag || i == -1 )
		return;
	do {
		drag += i > drag ? 1 : -1;
		items.at( drag )->setMark( !items.at( drag )->mark() );
		updateCell( drag, 0 );
	} while ( i != drag );
}

void MarkList::select( int i )
{
	if ( i < 0 || i >= (signed) items.count() || i == sel )
		return;

	MarkListItem *it = items.at( i );
	if ( sel != -1 )
	{
		items.at( sel )->setSelect( FALSE );
		updateCell( sel, 0 );
		updateCell( sel, 1 );
		updateCell( sel, 2 );
	}
	it->setSelect( TRUE );
	sel = i;
	updateCell( i, 0 );
	updateCell( i, 1 );
	updateCell( i, 2 );
	emit selected( i );
	emit selected( it->text() );
	if ( ( i<=0 || rowIsVisible( i-1 ) ) &&
	     ( i>= (signed) items.count()-1 || rowIsVisible( i+1 ) ) )
		return;
	setTopCell( QMAX( 0, i - viewHeight()/cellHeight()/2 ) );
}

void MarkList::markSelected()
{
	if ( sel == -1 )
		return;
	MarkListItem *it = items.at( sel );
	it->setMark( ! it->mark() );
	updateCell( sel, 0 );
}

void MarkList::markAll()
{
	changeMarks( 1 );
}

void MarkList::markEven()
{
	changeMarks( 1, 2 );	
}

void MarkList::markOdd()
{
	changeMarks( 1, 1 );
}


void MarkList::removeMarks()
{
	changeMarks( 0 );
}

void MarkList::toggleMarks()
{
	changeMarks( 2 );
}

void MarkList::changeMarks( int how, int which  )
{
	MarkListItem *it;
	QString t;

	setUpdatesEnabled( FALSE );
	for ( int i=items.count(); i-->0 ; )
	{
		if ( which )
		{
			t = items.at( i )->text();
			if ( t.toInt() % 2 == which - 1 )
				continue;
		}
		it = items.at( i );
		if ( how == 2 )
			it->setMark( ! it->mark() );
		else	it->setMark( how );
		updateCell( i, 0 );
	}
	setUpdatesEnabled( TRUE );
}

QStrList *MarkList::markList()
{
	QStrList *l = new QStrList;

	for ( int i=0 ; i < (signed) items.count(); i++ )
		if ( ( items.at( i ) )->mark() )
			l->append( items.at( i )->text() );
	return l;
}
