/*
* viewer.cpp -- Implementation of class KImageViewer.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Wed Oct 15 11:37:16 EST 1997
*/

#include<assert.h>
#include<stdio.h>

#include<qpopmenu.h>
#include<qmsgbox.h>
#include<qprintdialog.h>
#include<qprinter.h>
#include<qstrlist.h>
#include<qaccel.h>
#include<qtimer.h>
#include<qevent.h>

#include<ktopwidget.h>
#include<kmenubar.h>
#include<kapp.h>
#include<drag.h>
#include<kfiledialog.h>
#include<kstring.h>
#include<kfm.h>

#include"viewer.h"
#include"canvas.h"
#include"version.h"
#include"numdlg.h"

#include"filter.h"
#include"filtmenu.h"
#include"filtlist.h"

#include"ilistdlg.h"

enum {
	StatProgress,
	StatStatus
};

const int MessageDelay = 3000;

KImageViewer::KImageViewer()
	: KTopLevelWidget(),
	_imageLoaded( false ),
	_barFilterID( 0 ),
	_popFilterID( 0 ),

	_file( 0 ),
	_edit( 0 ),
	_zoom( 0 ),
	_transform( 0 ),
	_desktop( 0 ),
	_aggreg( 0 ),
	_help( 0 ),

	_kfm( 0 ),
	_transSrc( 0 ),
	_transDest( 0 ),
	_menuFact( 0 ),
	_pctBuffer( new QString ),
	_lastPct( - 1 ),
	_msgTimer( 0 ),
	_imageList( new ImgListDlg ),
	_zoomFactor( 100 ),

	_autoMaxpect( false )
{

	_canvas = new KImageCanvas( this );
	
	connect(_canvas, SIGNAL( contextPress(const QPoint&) ), 
		this, SLOT ( contextPress(const QPoint&) ));

	assert( _canvas );
	setView( _canvas, FALSE );
	
	// set caption

	QString cap;
	cap = kapp->appName().data();
	cap += " - ";
	cap += i18n( "no image loaded" );
	setCaption( cap );

	// status bar
	_statusbar = new KStatusBar( this );
	setStatusBar( _statusbar );

	_statusbar->insertItem( "                 ", StatProgress );
	_statusbar->insertItem( i18n( "Ready" ), StatStatus );

	// list dialog
	connect( _imageList, SIGNAL(selected(const char *)),
		this, SLOT(loadURL(const char *)) );

	// accelerators, enabled only when the menubar is
	// turned off.
	_accel = new QAccel( this );
	_accel->setEnabled( false );


	_accel->connectItem( _accel->insertItem( Key_F ), // full screen
		this, SLOT(fullScreen()) );
	_accel->connectItem( _accel->insertItem( Key_Escape ),
			this, SLOT(fullScreen()) );
	_accel->connectItem( _accel->insertItem( Key_I ), // image list
			this, SLOT(toggleImageList()) );

	_accel->connectItem( _accel->insertItem( Key_Z ), // zoom
			this, SLOT(zoomCustom()) );
	_accel->connectItem( _accel->insertItem( Key_C ), // crop
			_canvas, SLOT(cropImage()) );
	_accel->connectItem( _accel->insertItem( Key_Period ), // 10% zoom
			this, SLOT(zoomIn10()) );
	_accel->connectItem( _accel->insertItem( Key_Comma ),
			this, SLOT(zoomOut10()) );

	_accel->connectItem( _accel->insertItem(  Key_BracketLeft ), // 50%
			this, SLOT(zoomOut50()) );
	_accel->connectItem( _accel->insertItem( Key_BracketRight ), // 200%
			this, SLOT(zoomIn200()) );

	_accel->connectItem( _accel->insertItem(  Key_A ), // Maxpect
			_canvas, SLOT(maxpectToWin()) );

	_accel->connectItem( _accel->insertItem( Key_M ), // Max
			_canvas, SLOT(maxToWin()) );

	_accel->connectItem( _accel->insertItem( Key_S ), // slideshow
			_imageList, SLOT(toggleSlideShow()) );
	_accel->connectItem( _accel->insertItem( Key_Tab ), // next
			_imageList, SLOT(next()) );
	_accel->connectItem( _accel->insertItem( Key_Backspace ), // prev
			_imageList, SLOT(prev()) );

	_accel->connectItem( _accel->insertItem( CTRL + Key_R ),
			this, SLOT(reset()) );

	//
	// perennially active accelerators
	//
	
	// keyboard scrolling
	_paccel = new QAccel( this );

	_paccel->connectItem( _paccel->insertItem( Key_Down ),
			_canvas, SLOT(lineDown()) );

	_paccel->connectItem( _paccel->insertItem( Key_Up ),
			_canvas, SLOT(lineUp()) );

	_paccel->connectItem( _paccel->insertItem( Key_PageDown ),
			_canvas, SLOT(pageDown()) );

	_paccel->connectItem( _paccel->insertItem( Key_PageUp ),
			_canvas, SLOT(pageUp()) );


	_paccel->connectItem( _paccel->insertItem( Key_Right ),
			_canvas, SLOT(lineRight()) );

	_paccel->connectItem( _paccel->insertItem( Key_Left ),
			_canvas, SLOT(lineLeft()) );

	_paccel->connectItem( _paccel->insertItem( Key_Q ),	// quit
			this, SLOT(quitApp()) );

	// Drop events forwarded to image list object

	KDNDDropZone *dropObserver = new KDNDDropZone( this, DndURL );

	QObject::connect( dropObserver, SIGNAL(dropAction(KDNDDropZone *)),
			_imageList, SLOT(dropEvent(KDNDDropZone *)) );

	restoreOptions( kapp->getConfig() );
}

KImageViewer::~KImageViewer()
{
	delete _canvas; _canvas = 0;

	delete _file;
	delete _edit;
	delete _zoom;
	delete _transform;
	delete _desktop;
	delete _aggreg;
	delete _help;

	delete _contextMenu;

	delete _pctBuffer;
	delete _imageList;

	delete _kfm;
	_kfm = 0;

	delete _transSrc;
	_transSrc = 0;

	delete _transDest;
	_transDest = 0;
}

void KImageViewer::load()
{
	KFileDialog dlg( "", 0, 0, 0, true, true );
	QString URL = dlg.getOpenFileURL();

	if( URL.length() <= 0 ) {
		return;
	}

	_imageList->addURL( URL );
}

void KImageViewer::saveAs()
{
	KFileDialog dlg( "", 0, 0, 0, true, false );
	QString urls = dlg.getSaveFileURL();


	if ( urls.length() <= 0 ) {
		return;
	}

	KURL url( urls );
	bool stat = false;

	if( url.isMalformed() ) {
		stat = _canvas->save( urls );
	}
	else if( url.isLocalFile() ) {
		stat = _canvas->save( url.path() );
	}
	else {
		message( i18n("Net saving not yet implemented") );
	}

	if( stat == false ) {
		return;
	}

	QString msg = urls;
	ksprintf( &msg, i18n( "%s: written" ), urls.data() );
	message( msg );
}

void KImageViewer::zoomIn10()
{
	_mat.scale( 1.1, 1.1 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomOut10()
{
	_mat.scale( 0.9, 0.9 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomIn200()
{
	_mat.scale( 2.0, 2.0 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomOut50()
{
	_mat.scale( 0.5, 0.5 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomCustom()
{
	KNumDialog num;
	_zoomFactor = (int)(_mat.m11() * 100);

	if( !num.getNum( _zoomFactor, i18n(
			"Enter Zoom factor (100 = 1x):" ) ) )
		return;

	if( _zoomFactor <= 0 ) {
		message( i18n( "Illegal zoom factor" ) );
		return;
	}
		
	double val = (double)_zoomFactor/100;
	_mat.reset();
	_mat.scale( val, val );
	_canvas->transformImage( _mat );
}

void KImageViewer::rotateClock()
{
	_mat.rotate( 90 );

	_canvas->transformImage( _mat );
}

void KImageViewer::rotateAntiClock()
{
	_mat.rotate( -90 );

	_canvas->transformImage( _mat );
}

void KImageViewer::flipVertical()
{
	QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );

	_mat *= m;

	_canvas->transformImage( _mat );
}

void KImageViewer::flipHorizontal()
{
	QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );

	_mat *= m;

	_canvas->transformImage( _mat );
}

void KImageViewer::help()
{
	kapp->invokeHTMLHelp( "kview/kview.html", "" );	
}

void KImageViewer::about()
{
	QMessageBox::about( this, i18n("About KView"),
		"KView -- Graphics viewer. " KVIEW_VERSION "\n"
		"\nSirtaj S. Kang (taj@kde.org)\n" );
}

void KImageViewer::makeRootMenu(QMenuData *menu)
{
    /*
	menu->insertItem( i18n( "&File" ),	_file );
	menu->insertItem( i18n( "&Edit" ),	_edit );
	menu->insertItem( i18n( "&Zoom" ),	_zoom );
	menu->insertItem( i18n( "&Transform" ),	_transform );
	menu->insertItem( i18n( "To &Desktop"),	_desktop );
	
	_popFilterID = menu->insertItem( i18n( "Fi&lter"),	
		_menuFact->filterMenu() );
	menu->setItemEnabled( _popFilterID, false );

	menu->insertItem( i18n( "&Options" ), _aggreg );
	
	menu->insertSeparator();
	menu->insertItem( i18n( "&Help" ), _help );
	
    */	
    
    menu->insertItem( i18n("&List..."), this,
			 SLOT(toggleImageList()), Key_I );
    menu->insertSeparator();
    menu->insertItem( i18n("&Previous"), _imageList, SLOT(prev()),
			 Key_Backspace );
    menu->insertItem( i18n("&Next"), _imageList, SLOT(next()),
			 Key_Tab );
    menu->insertSeparator();
    menu->insertItem( i18n("&Slideshow On/Off"), _imageList,
			 SLOT(toggleSlideShow()), Key_S );

}

void KImageViewer::makeRootMenu(KMenuBar *menu)
{
	menu->insertItem( i18n( "&File" ),	_file );
	menu->insertItem( i18n( "&Edit" ),	_edit );
	menu->insertItem( i18n( "&Zoom" ),	_zoom );
	menu->insertItem( i18n( "&Transform" ),	_transform );
	menu->insertItem( i18n( "To &Desktop"),	_desktop );

	_barFilterID = menu->insertItem( i18n( "F&ilter"),	
		_menuFact->filterMenu() );
	menu->setItemEnabled( _barFilterID, false );
	
	menu->insertItem( i18n( "&Images" ), _aggreg );

	menu->insertSeparator();
	menu->insertItem( i18n( "&Help" ),  _help );
}

void KImageViewer::makePopupMenus()
{
	_file		= new QPopupMenu;
	_edit		= new QPopupMenu;
	_zoom		= new QPopupMenu;
	_transform	= new QPopupMenu;
	_desktop	= new QPopupMenu;
	_aggreg	= new QPopupMenu;
	_help		= new QPopupMenu;

	// file pulldown

	_file->insertItem( i18n( "&Open..." ), this, SLOT(load()) );
	_file->insertItem( i18n( "&Save As..." ), this, SLOT(saveAs()) );
	_file->insertSeparator();
	_file->insertItem( i18n( "&Print..." ), this, SLOT(printImage()) );
	_file->insertSeparator();
	_file->insertItem( i18n( "&New Window" ), this, SLOT(newViewer()),
		CTRL + Key_N );
	_file->insertItem( i18n( "&Close Window" ), this, SLOT(closeViewer()));
	_file->insertSeparator();
	_file->insertItem( i18n( "E&xit" ), this, SLOT(quitApp()),
		CTRL + Key_Q );

	// edit pulldown
	_edit->insertItem( i18n( "&Full Screen" ), this, SLOT(fullScreen()),
			Key_F );
	_edit->insertSeparator();
	_edit->insertItem( i18n( "&Crop" ), 	_canvas, SLOT(cropImage()),
			Key_C );
	_edit->insertSeparator();
	_edit->insertItem( i18n( "&Reset"), this, SLOT(reset()),
		CTRL + Key_R );

	// zoom pulldown

	_zoom->insertItem( i18n( "&Zoom..." ), 	this, SLOT(zoomCustom()),
			Key_Z );
	_zoom->insertItem( i18n( "Zoom &in 10%" ), this, SLOT(zoomIn10()),
			Key_Period );
	_zoom->insertItem( i18n( "Zoom &out 10%"), this, SLOT(zoomOut10()),
			Key_Comma );
	_zoom->insertItem( i18n( "&Double size" ), this, SLOT(zoomIn200()),
			Key_BracketRight );
	_zoom->insertItem( i18n( "&Half size"), this, SLOT(zoomOut50()),
			Key_BracketLeft );
	_zoom->insertItem( i18n( "&Max size"), _canvas, SLOT(maxToWin()),
			Key_M );
	_zoom->insertItem( i18n( "Max/&aspect"), _canvas, SLOT(maxpectToWin()),
			Key_A );

	// transform pulldown

	_transform->insertItem( i18n( "Rotate &clockwise" ),
			this, SLOT(rotateClock()) );
	_transform->insertItem( i18n( "Rotate &anti-clockwise" ),
			this, SLOT(rotateAntiClock()) );
	_transform->insertItem( i18n( "Flip &vertical" ),
			this, SLOT(flipVertical()) );
	_transform->insertItem( i18n( "Flip &horizontal" ),
			this, SLOT(flipHorizontal()) );

	// desktop pulldown

	_desktop->insertItem( i18n( "&Tile" ), this, SLOT( tile() )  );
	_desktop->insertItem( i18n( "&Max" ),  this, SLOT( max() )   );
	_desktop->insertItem( i18n("Max&pect"),this, SLOT(maxpect()) );


	int id = _aggreg->insertItem( i18n("&List..."), this,
		SLOT(toggleImageList()), Key_I );
	_aggreg->setItemChecked( id, false );
	_aggreg->insertSeparator();
	_aggreg->insertItem( i18n("&Previous"), _imageList, SLOT(prev()),
			Key_Backspace );
	_aggreg->insertItem( i18n("&Next"), _imageList, SLOT(next()),
			Key_Tab );
	_aggreg->insertSeparator();
	_aggreg->insertItem( i18n("&Slideshow On/Off"), _imageList,
		SLOT(toggleSlideShow()), Key_S );

	_help->insertItem( i18n( "&Contents" ), this, SLOT(help()) );
	_help->insertSeparator();
	_help->insertItem( i18n( "&About KView..." ), this, SLOT(about()) );
}

void KImageViewer::quitApp()
{
	KApplication::getKApplication()->quit();
}

void KImageViewer::tile()
{
	_canvas->tileToDesktop();
}

void KImageViewer::max()
{
	_canvas->maxToDesktop();
}

void KImageViewer::maxpect()
{
	_canvas->maxpectToDesktop();
}

void KImageViewer::loadURL( const char *url )
{
	if( url == 0 ) {
		warning( "loadURL: called with null url" );
		return;
	}

	KURL kurl( url );

	if( kurl.isMalformed() ) {
		loadFile( url );
	}
	else if( kurl.isLocalFile() ) {
		// local file - open with loadFile
		loadFile( kurl.path(), url );
	}
	else {
		// connect to kfm
		if( _kfm != 0 ) {
			message( i18n( "Existing transfer not yet complete."));
			return;
		}

		_kfm = new KFM;

		if( !_kfm || !_kfm->isKFMRunning() || !_kfm->isOK() ) {
			message( i18n( "Existing transfer not yet complete."));
			return;
		}

		// save transfer files
		if( _transSrc == 0 )
			_transSrc = new QString;
		if( _transDest == 0 )
			_transDest = new QString;

		_transSrc->setStr( url );
		_transDest->setStr( tempnam( 0, "kview_") );

		if( _transSrc->isEmpty() || _transDest->isEmpty() ) {
			message( i18n( "Couldn't get temporary transfer name." ) );
			return;
		}

		// start transfer
		connect( _kfm, SIGNAL( finished() ),
			this, SLOT( urlFetchDone() ) );
		connect( _kfm, SIGNAL( error(int, const char *) ),
			this, SLOT( urlFetchError(int, const char *) ) );

		_kfm->copy( _transSrc->data(), _transDest->data() );
		_transDir = Get;
	}
}

void KImageViewer::urlFetchDone()
{
	if( _kfm == 0 ) {
		warning( "KImageViewer: urlFetchDone() called but no transfer in progress." );
		return;
	}

	switch ( _transDir ) {
		case Get:	loadFile( _transDest->data(),
				_transSrc->data() );
				unlink( _transDest->data() );
				break;
		case Put:	break;
	}

	delete _kfm;
	_kfm = 0;
}

void KImageViewer::urlFetchError( int,  const char *text )
{
	delete _kfm;
	_kfm = 0;

	QString msg( i18n("Transfer error: ") );
	msg += text;

	message( msg );
}

void KImageViewer::appendURL( const char *url, bool show )
{
	_imageList->addURL( url, show );
}

void KImageViewer::invokeFilter( KImageFilter *f )
{
	assert( f != 0 );

	QObject::disconnect( f, SIGNAL(changed(const QImage&)), 0, 0 );

	connect( f, SIGNAL(changed(const QImage&)),
			_canvas, SLOT(setImage(const QImage&)) );

	f->invoke( _canvas->getImage() );
}

void KImageViewer::setFilterMenu( KFiltMenuFactory *filtmenu )
{
	connect( filtmenu, SIGNAL(selected(KImageFilter *)),
		this, SLOT(invokeFilter(KImageFilter *)) );

	// connect all filters
	KFilterList *filters = filtmenu->filterList();
	
	for( int i = 0; i < filters->count(); i++ ) {
		KImageFilter *filter = filters->filter( i );

		connect( filter, SIGNAL(changed(const QImage&)),
				_canvas, SLOT(setImage(const QImage&)) );
		connect( filter, SIGNAL(progress(int)),
			this, SLOT(setProgress(int)) );
		connect( filter, SIGNAL(status(const char *)),
			this, SLOT(setStatus(const char *)) );
		connect( filter, SIGNAL(message(const char *)),
			this, SLOT(message(const char *)) );
	}

	_menuFact = filtmenu;


	makePopupMenus();
	_menubar = new KMenuBar(this);
	makeRootMenu( _menubar );

	makePopupMenus();
	_contextMenu = new QPopupMenu;
	makeRootMenu( _contextMenu );

	setMenu( _menubar );
	_menubar->show();
	_canvas->show();
}

void KImageViewer::setStatus( const char *status )
{
	if ( status == 0 ) {
		status = i18n( "Ready" );
	}

	_statusbar->changeItem( status, StatStatus );
}

void KImageViewer::loadFile( const char *file, const char *url )
{
	if( url == 0 ) {
		url = file;
	}

	bool slide = _imageList->slideShowRunning();

	if( slide ) {
		_imageList->pauseSlideShow();
	}

	setStatus( i18n( "Loading..." ) );
	_canvas->load( file, 0, _autoMaxpect );


	setStatus( 0 );

	if( _canvas->status() != KImageCanvas::OK ) {

		QString msg;
		msg.sprintf(i18n("Couldn't load %s"), url);
		message( msg );
	}
	else {
		// set caption
		QString cap = url;
		cap += " (";
		cap += kapp->appName().data();
		cap += ")";
		setCaption( cap );

		// enable filters
		if( !_imageLoaded ) {
			_menubar->setItemEnabled( _barFilterID, true );
			_contextMenu->setItemEnabled( _popFilterID, true );
			_imageLoaded = true;
		}
		// reset matrix
		_mat.reset();
	}

	if ( slide ) {
		_imageList->continueSlideShow();
	}
}

void KImageViewer::setSize()
{
		// size
		QWidget *desk = kapp->desktop();

		if( _canvas->height() < desk->height()
			&& _canvas->width() < desk->width()
			&& _canvas->height() ) {

			resize( _canvas->size() );
		}
}

void KImageViewer::setProgress( int pct )
{
	if( pct == _lastPct )
		return;

	const char *buf = "";

	if( pct > 0 ) {
		_pctBuffer->setNum( pct );
		*_pctBuffer += '%';
		buf = _pctBuffer->data();
	}

	_lastPct = pct;

	_statusbar->changeItem( buf, StatProgress );
}

void KImageViewer::message( const char *message )
{
	_statusbar->message( message, MessageDelay );

	if( _msgTimer == 0 ) {
		_msgTimer = new QTimer( this );
		connect( _msgTimer, SIGNAL(timeout()),
			_statusbar, SLOT(clear()) );
	}

	_msgTimer->start( MessageDelay, true );
}

void KImageViewer::toggleImageList()
{
	if ( _imageList->isVisible() ) {
		_imageList->hide();
	}
	else {
		_imageList->show();
	}
}


void KImageViewer::contextPress(const QPoint& p)
{
    _contextMenu->popup(p);

}

void KImageViewer::fullScreen()
{
	if( _statusbar->isVisible() ) {
		// change to full

		_accel->setEnabled( true );
			
		_menubar->hide();
		_statusbar->hide();

		_posSave = pos();
		_sizeSave = size();

		move( frameGeometry().x() - geometry().x(),
			frameGeometry().y() - geometry().y() );

		resize( qApp->desktop()->size() );

		_canvas->resize( size() );
		_canvas->move( 0, 0 );

		// Grab WM focus. We need to do this for some
		// reason, atleast with KWM. Otherwise the
		// window loses focus and keyboard accelerators
		// don't work unless the mouse is clicked once.
		setActiveWindow();
	}
	else {
		// change to normal
		move( _posSave );
		resize( _sizeSave );
		_menubar->show();
		_statusbar->show();
		updateRects();

		_accel->setEnabled( false );
	}
}

void KImageViewer::reset()
{
	_canvas->reset();
	_mat.reset();
}

void KImageViewer::saveProperties( KConfig *cfg ) const
{
	if( _statusbar->isVisible() ) {
		cfg->writeEntry( "ViewerFullScreen", false );
		cfg->writeEntry( "ViewerPos", _posSave );
		cfg->writeEntry( "ViewerSize", _sizeSave );
	}
	else {
		cfg->writeEntry( "ViewerFullScreen", true );
		cfg->writeEntry( "ViewerPos", pos() );
		cfg->writeEntry( "ViewerPos", size() );
	}
	
	_imageList->saveProperties( cfg );

}

void KImageViewer::restoreProperties( KConfig *cfg )
{

	bool full = cfg->readBoolEntry( "ViewerFullScreen" );
	
	if( full ) {
		fullScreen();
		_posSave = cfg->readPointEntry( "ViewerPos" );
		_sizeSave = cfg->readSizeEntry( "ViewerSize" );
	}
	else {
		move( cfg->readPointEntry( "ViewerPos" ) );
		resize( cfg->readSizeEntry( "ViewerSize" ) );
	}

	_imageList->restoreProperties( cfg );
}

void KImageViewer::saveOptions( KConfig *cfg ) const
{
	KConfigGroupSaver save( cfg, "kview" );
	cfg->writeEntry( "AutoMaxpect", _autoMaxpect );
}

void KImageViewer::restoreOptions( KConfig *cfg )
{
	KConfigGroupSaver save( cfg, "kview" );
	_autoMaxpect = cfg->readBoolEntry( "AutoMaxpect", false );
}

void KImageViewer::printImage()
{
	QPrinter printer;

	// get settings
	if( QPrintDialog::getPrinterSetup( &printer ) == false )
		return;

	// print

	setStatus( i18n( "Printing..." ) );
	QApplication::setOverrideCursor( WaitCursor );
	_canvas->copyImage( &printer );
	printer.newPage();
	QApplication::restoreOverrideCursor();
	setStatus( 0 );
}

void KImageViewer::newViewer()
{
	emit wantNewViewer();
}

void KImageViewer::closeViewer()
{
	emit wantToDie( this );
}

void KImageViewer::closeEvent( QCloseEvent * )
{
	closeViewer();
}

void KImageViewer::cut()
{
	// TODO: stub
}

void KImageViewer::copy()
{
	// TODO: stub
}

void KImageViewer::paste()
{
	// TODO: stub
}
