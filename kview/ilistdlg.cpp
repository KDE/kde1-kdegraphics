/*
* ilistdlg.cpp -- Implementation of class ImgListDlg.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Wed Sep 23 16:04:52 EST 1998
*/

#include<stdlib.h>
#include<time.h>

#include<qstrlist.h>
#include<qtimer.h>
#include<qlistbox.h>
#include<qpushbt.h>
#include<qcheckbox.h>

#include<kapp.h>
#include<drag.h>

#include"numdlg.h"
#include"typolayout.h"
#include"ilistdlg.h"

ImgListDlg::ImgListDlg( QWidget *parent )
	: QWidget( parent ),

	_slideTimer( 0 ),		// allocated by slide start
	_slideInterval( 5 ),
	_slideButton ( 0 ),
	_loop ( 0 ),
	_paused( false ),

	_list( new QStrList( true ) ), // deep copies
	_listBox( 0 )
{
	// drop events
	KDNDDropZone *dropZone = new KDNDDropZone( this, DndURL );

	connect( dropZone, SIGNAL(dropAction(KDNDDropZone *)),
		this, SLOT(dropEvent(KDNDDropZone*)) );

	// layout
	KTypoLayout *layout = new KTypoLayout( this );
	layout->setHSpace( 5, 0 );
	layout->setVSpace( 5, 5 );
	layout->setGridSize( 4, 1 );
	_listBox = layout->newListBox( 0, 0, 3, 1 ); // list
	connect( _listBox, SIGNAL(selected(int)),
		this, SLOT(select(int)) );

	layout = layout->newSubLayout( 3, 0, 1, 1 );
	layout->setGridSize( 1, 8 );

	// up btn
	QPushButton *up = layout->newButton( i18n("Prev"), 0, 1, 1, 1 );
	connect( up, SIGNAL(clicked()), this, SLOT(prev()) );

	// shuffle 
	QPushButton *shuffle = layout->newButton( i18n("Shu&ffle"), 
		0, 2, 1, 1 );
	connect( shuffle, SIGNAL(clicked()), this, SLOT(shuffle()) );

	// down btn
	QPushButton *down = layout->newButton( i18n("Next"), 0, 3, 1, 1 );
	connect( down, SIGNAL(clicked()), this, SLOT(next()) );

	// slide start btn
	_slideButton = layout->newButton( i18n("Start slide"), 
		0, 5, 1, 1 );
	connect( _slideButton, SIGNAL(clicked()), 
			this, SLOT(toggleSlideShow()) );

	QPushButton *interval= layout->newButton( i18n("Interval.."),
		0, 6, 1, 1 );
	connect( interval, SIGNAL(clicked()), 
			this, SLOT(setInterval()) );
	
	_loop = layout->newCheckBox( i18n( "Loop" ), 0, 7, 1, 1 );
	_loop->setChecked( false );

	// caption

	QString cap = kapp->appName();
	cap.detach();
	cap += ": ";
	cap += i18n( "Image List" );

	setCaption( cap );

	srand( time(0) );
}

ImgListDlg::~ImgListDlg()
{
	delete _slideTimer;
	_slideTimer = 0;

	delete _list;
	_list = 0;
}

void ImgListDlg::addURL( const char *url, bool show )
{
	if ( url == 0 ) {
		return;
	}

	if ( !show ) {
		// append only, don't display

		int current = _list->at();	
		_list->append( url );
		_listBox->insertItem( url );

		_list->at( current );
		_listBox->setCurrentItem( current );
	}
	else {
		// append and display

		_list->append( url );
		_listBox->insertItem( url );

		emit selected( _list->current() );
	}
}

void ImgListDlg::first()
{
	if( _list->current() == 0 ) {
		return;
	}
	const char *str = _list->first();

	if( str ) {
		_listBox->setCurrentItem( _list->at() );
		emit selected( str );
	}
}

void ImgListDlg::last()
{
	if( _list->current() == 0 ) {
		return;
	}
	const char *str = _list->last();

	if( str ) {
		_listBox->setCurrentItem( _list->count() - 1 );
		emit selected( str );
	}
	else {
		_list->first();
	}
}

void ImgListDlg::prev()
{
	if( _list->current() == 0 ) {
		return;
	}

	const char *str = _list->prev();

	if( str ) {
		_listBox->setCurrentItem( _list->at() );

		emit selected( str );
	}
	else {
		_list->first();
	}
}

void ImgListDlg::next()
{
	if( _list->current() == 0 ) {
		return;
	}
	const char *str = _list->next();

	if( str ) {
		_listBox->setCurrentItem( _list->at() );
		emit selected( str );

	}
	else {
		_list->last();
	}
}

void ImgListDlg::dropEvent( KDNDDropZone *drop )
{
	addURLList( drop->getURLList() );
}

void ImgListDlg::addURLList( const QStrList& list )
{
	 QStrListIterator iter( list );

        if ( iter.current() ) {
                // queue net load
                addURL( iter.current() );

		_listBox->setAutoUpdate( false );
                while( ++iter ) {
                        addURL( iter.current(), false );
                }
		_listBox->setAutoUpdate( true );
		_listBox->repaint();
        }
}

void ImgListDlg::select( int idx )
{
	if( idx < 0 || idx > _list->count() ) {
		return;
	}

	emit selected( _list->at( idx ) );
}

void ImgListDlg::shuffle()
{
	QStrList newlist;
	int c = _list->count();

	// fill list with randomized items
	for( int i = c; i > 0; --i  ) {
		int idx = (int) ( ((double)i * (double)rand()) 
			/ (RAND_MAX+1.0) );

		if( idx >= i ) {
			debug( "Index is %d of %d", idx, i );
		}

		newlist.append( _list->at( idx ) );

		_list->remove( idx );
	}

	// refill listbox
	_listBox->clear();
	addURLList( newlist );
}

void ImgListDlg::startSlideShow()
{
	if( _list->count() == 0 ) {
		// no images to show
		return;
	}

	if( _slideTimer == 0 ) {
		_slideTimer = new QTimer( this, "Slideshow Timer" );
		connect( _slideTimer, SIGNAL(timeout()), 
			this, SLOT(nextSlide()) );
	}
	
	if( !_slideTimer->isActive() ) {
		nextSlide();
		_slideTimer->start( _slideInterval * 1000 );
		_slideButton->setText( i18n( "Stop &slide" ) );
	}
}

void ImgListDlg::stopSlideShow()
{
	if( _slideTimer->isActive() ) {
		_slideTimer->stop();
		_slideButton->setText( i18n( "Start &slide" ) );
	}
}

void ImgListDlg::setSlideInterval( int seconds )
{
	if( seconds == _slideInterval )
		return;

	_slideInterval = seconds;

	if( _slideTimer !=0 && _slideTimer->isActive() ) {
		_slideTimer->changeInterval( seconds * 1000 );
	}
}


void ImgListDlg::nextSlide()
{
	if( _list->current() == _list->getLast() ) {
		if ( _loop->isChecked() ) {
			first();
		}
		else {
			stopSlideShow();
		}
	}
	else {
		next();
	}
}

void ImgListDlg::toggleSlideShow()
{
	if( _slideTimer && _slideTimer->isActive() ) {
		stopSlideShow();

	}
	else {
		startSlideShow();

	}
}
void ImgListDlg::pauseSlideShow()
{
	if ( slideShowRunning() ) {
		_slideTimer->stop();
		_paused = true;
	}
}

void ImgListDlg::continueSlideShow()
{
	if ( _paused ) {
		_slideTimer->start( _slideInterval * 1000 );
		_paused = false;
	}
}

void ImgListDlg::setInterval()
{
	KNumDialog dlg;
	dlg.getNum( _slideInterval, 
			i18n("Time between slides (sec):") );
}
