/*
* canvas.cpp -- Implementation of class KImageCanvas.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Thu Oct  9 09:03:54 EST 1997
*/

#include<qcolor.h>
#include<qpixmap.h>
#include<qapp.h>
#include<assert.h>
#include<qlabel.h>
#include<qimage.h>

#include"canvas.h"

KImageCanvas::KImageCanvas( QWidget *parent )
	: QScrollView( parent ),
	_status( OK ),
	_file( "" ),
	_client( 0 ),
	_orig( 0 )
{
	_client = new QLabel( this );
	addChild( _client );

	_client->show();
	_client->setAutoResize( TRUE );
}

KImageCanvas::~KImageCanvas()
{
	delete _orig;
}

KImageCanvas::Error KImageCanvas::status() const
{
	return _status;
}

int KImageCanvas::load( const char *file, const char *URL )
{
	if( file == 0 ) {
		setStatus( BadPath );
		return 0;
	}

	static int allocContext = 0;
	if( allocContext )  {
		QColor::destroyAllocContext( allocContext );
	}

	allocContext = QColor::enterAllocContext();

	QApplication::setOverrideCursor( WaitCursor );

	QPixmap newImage;
	bool loadOK = newImage.load( file );

	QColor::leaveAllocContext();

	if( loadOK ) {
		delete _orig; // delete old buffered image
		_orig = 0;

		_client->setPixmap( newImage );
		_client->move(0, 0 );
		_client->resize( newImage.width(),
				newImage.height() );

		_originalSize = size();

		updateScrollBars();

		_file = ( URL ? URL : file );
		setStatus( OK );
		setCaption( _file );
		QApplication::restoreOverrideCursor();
		return -1;
	}
	else {
		warning( "Couldn't open %s", file );
		setStatus( BadPath );
	}

	QApplication::restoreOverrideCursor();
	return 0;
}

bool KImageCanvas::save( const char *urls, const char *format )
{
	if( urls == 0 ) {
		warning( "KImageCanvas::save called with null url" );
	}

	if( format == 0 ) {
		format = "BMP";
	}

	return _client->pixmap()->save( urls, format );
}

void KImageCanvas::reset()
{
	if( _orig == 0 ) {
		return;
	}

	_client->setPixmap( *_orig );
	_client->resize( _orig->size() );
}

void KImageCanvas::clear()
{
	_client->setBackgroundColor( QColor(0,0,0) );
	_client->resize( 0, 0 );
}

void KImageCanvas::transformImage( const QWMatrix& mat )
{
	const QPixmap *image = transPixmap();

	if( image == 0 ) {
		return;
	}

	QPixmap newimage = _orig->xForm( mat );

	_client->resize( newimage.size() );
	_client->setPixmap( newimage );
}

void KImageCanvas::tileToDesktop() const
{

	const QPixmap *image = _client->pixmap();

	if( image == 0 ) {
		return;
	}

	qApp->desktop()->setBackgroundPixmap( 
			*image );
}

void KImageCanvas::maxToDesktop() const
{
	const QPixmap *image = _client->pixmap();

	if( image == 0 ) {
		return;
	}

	double dh = (double)qApp->desktop()->height()/(double)image->height();
	double dw = (double)qApp->desktop()->width()/(double)image->width();
	QWMatrix mat;

	mat.scale( dw, dh );

	qApp->desktop()->setBackgroundPixmap( image->xForm(mat) );

}

void KImageCanvas::maxpectToDesktop() const
{
	const QPixmap *image = _client->pixmap();

	if( image == 0 ) {
		return;
	}

	double dh = (double)qApp->desktop()->height()/(double)image->height();
	double dw = (double)qApp->desktop()->width()/(double)image->width();
	QWMatrix mat;
	double d = ( dh < dw ? dh : dw );
	
	mat.scale( d, d );

	qApp->desktop()->setBackgroundPixmap( image->xForm(mat) );
}

void KImageCanvas::resizeEvent( QResizeEvent *ev )
{
	QScrollView::resizeEvent( ev );

	emit imageSizeChanged();
}

QImage KImageCanvas::getImage() const
{
	return _client->pixmap()->convertToImage();
}

void KImageCanvas::setImage( const QImage& image )
{
	transPixmap();

	QPixmap pixmap;
	pixmap.convertFromImage( image );
	_client->setPixmap( pixmap );
	_client->resize( pixmap.size() );

	emit imageSizeChanged();
}

QPixmap *KImageCanvas::transPixmap()
{
	QPixmap *client = _client->pixmap();

	if( client == 0 ) {
		return 0;
	}

	if( _orig == 0 ) {
		_orig = new QPixmap( *client );
	}

	return client;
}
