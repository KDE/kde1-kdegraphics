/*
* gridbagl.cpp -- Implementation of class KGridBagLayout.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Mon May  4 15:57:51 EST 1998
*/

#include<assert.h>

#include"baglayout.h"

KBagLayout::KBagLayout( QWidget *parent, int x, int y )
	: QObject( parent ), 
	_sizeSet ( false ), _grid( x, y ),  _min( 0, 0 ),
	_topSpace( 2 ),	_bottomSpace( 2 ),
	_leftSpace( 2 ), _rightSpace( 2 )
{
	assert( parent );
	_parent = parent;

	_cstr = new QPtrDict<KBagConstraints>;
	_cstr->setAutoDelete( true );
	
	_widgets = new QList<QWidget>;
	_widgets->setAutoDelete( false );

	if( x != 0 || y != 0 ) {
		_sizeSet = true;
	}

	_parent->installEventFilter( this );
}

KBagLayout::~KBagLayout()
{
	delete _cstr;
	delete _widgets;
}

KBagConstraints *KBagLayout::addWidget( QWidget *widget )
{
	KBagConstraints * c = new KBagConstraints;

	_widgets->append( widget );
	_cstr->insert( widget, c );

	return c;
}

KBagConstraints *KBagLayout::addWidget( QWidget *widget, int x, int y,
				     int xspan, int yspan )
{
	KBagConstraints * c = addWidget( widget );

	c->setGridX( x );
	c->setGridY( y );
	c->setXSpan( xspan );
	c->setYSpan( yspan );

	return c;
}


KBagConstraints *KBagLayout::addWidget( QWidget *widget, 
		KBagConstraints& constr )
{
	KBagConstraints * c = new KBagConstraints( constr );

	_widgets->append( widget );
	_cstr->insert( widget, c );

	return c;
}

void KBagLayout::activate()
{
	if ( _sizeSet == false ) {
		calcGridExtent();
	}

	if( !_grid.isValid() ) {
		warning( "KBagLayout: illegal layout constraints." );
		return;
	}

	// arrange each widget

	QListIterator<QWidget> list( *_widgets );

	for( ; list.current(); ++list ) {
		arrangeWidget( list.current(), 
			(KBagConstraints *)
			_cstr->find ( list.current() ) );
	}
}

void KBagLayout::calcGridExtent()
{
	QListIterator<QWidget> list( *_widgets );

	_grid.setWidth(  0 );
	_grid.setHeight(  0 );

	for( ; list.current(); ++list ) {
		KBagConstraints *c = constraints( list.current() );
		int cx = c->gridX() + c->xSpan();
		int cy = c->gridY() + c->ySpan();

		if( cx > _grid.width() ) {
			_grid.setWidth(  cx );
		}

		if( cy > _grid.height() ) {
			_grid.setHeight(  cy );
		}
	}
}

void KBagLayout::arrangeWidget( QWidget *w, KBagConstraints *c )
{
	assert ( w );
	assert ( c );

	if( _grid.width() == 0 || _grid.height() == 0 ) {
		warning( "KBagLayout::arrangeWidget called with "
				"invalid grid size (%d,%d)",
				_grid.width(), _grid.height() );
	}

	int x=0, y=0, width=0, height=0;

	int cellx = _parent->rect().width()/_grid.width()
		- ( _leftSpace + _rightSpace );
	int celly = _parent->rect().height()/_grid.height()
		- ( _topSpace + _bottomSpace );

	_min.setWidth ( 0 );
	_min.setHeight( 0 );

	//
	// X layout
	//
	
	if( c->resizePolicy() & KBagConstraints::Horizontal ) {
		// offset from left by X
		x = c->xSpace();
		width = (cellx*c->xSpan()) - (2 * c->xSpace()) + 1;
	}
	else {
		// centre in X
		if ( c->align() & KBagConstraints::HCenter ) {
			x = (cellx - w->width())/ 2;

			_min.rwidth() += w->width() + 2 * c->xSpace();
		}
		// align left
		else if ( c->align() & KBagConstraints::Left ) {
			x = c->xSpace()/2;
		}
		// align right
		else if ( c->align() & KBagConstraints::Right ) {
			x = cellx - w->width() - (c->xSpace()/2) - 1;
		}

		width = w->width();

		_min.rwidth() += w->width() + 2 * c->xSpace() + 1;
	}

	//
	// Y layout
	//

	if( c->resizePolicy() & KBagConstraints::Vertical ) {
		// offset from top by Y
		y = c->ySpace();
		height = (celly*c->ySpan()) - (2 * c->ySpace());
	}
	else {
		// centre in Y
		if ( c->align() & KBagConstraints::VCenter ) {
			y = (celly - w->height()) /2;
		}
		// align top
		else if ( c->align() & KBagConstraints::Top ) {
			y = c->ySpace()/2;
		}
		// align bottom
		else if ( c->align() & KBagConstraints::Bottom ) {
			y = celly - w->height() - (c->ySpace()/2) - 1;
		}

		height = w->height();

		_min.rheight() += w->height() + 2 * c->ySpace() + 1;
	}

	// move/resize widget.
	w->setGeometry( _leftSpace + _parent->rect().left()	
			+ cellx * c->gridX() + x,
			_topSpace + _parent->rect().top()	
			+ celly * c->gridY() + y,
			width,
			height );

}

bool KBagLayout::eventFilter( QObject *receiver, QEvent *event ) 
{
	if( receiver == _parent && event->type() == Event_Resize ) {
		activate();
	}

	return false;
}
