/*
* colour.cpp -- Implementation of class ColourFilter.
* Author:	Sirtaj Singh Kang <taj@kde.org>
* Version:	$Id$
* Generated:	Tue Sep 22 02:26:05 EST 1998
*/

#include<qcolor.h>

#include"colour.h"
#include"kapp.h"

static void modifyIntensity( QImage& image, int delta );

void BriteFilter::invoke( QImage before )
{
	if ( before.depth() < 32 ) {
		warning( "cannot use rgb for image of depth %d", 
			before.depth() );
		return;
	}

	emit status( i18n( "Brightening.." ) );

	QApplication::setOverrideCursor( waitCursor );    
	QImage newimage = before;
	modifyIntensity( newimage, 10 );
	QApplication::restoreOverrideCursor();

	emit changed( newimage );

	emit status( 0 );
}

const char *BriteFilter::name() const
{
	return i18n( "Intensity:Brighten" );
}

KImageFilter *BriteFilter::clone() const
{
	return new BriteFilter;
}

void DarkFilter::invoke( QImage before )
{
	if ( before.depth() < 32 ) {
		warning( "cannot use rgb for image of depth %d", 
			before.depth() );
		return;
	}

	emit status( i18n( "Darkening.." ) );
	QApplication::setOverrideCursor( waitCursor );    
	QImage newimage = before;

	modifyIntensity( newimage, -10 );
	QApplication::restoreOverrideCursor();

	emit changed( newimage );
	emit status( 0 );
}

const char *DarkFilter::name() const
{
	return i18n( "Intensity:Darken" );
}

KImageFilter *DarkFilter::clone() const
{
	return new DarkFilter;
}

static void modifyIntensity( QImage& image, int delta )
{
	for( int y = 0; y < image.height(); y++ ) {
		QRgb *clr = (QRgb *) image.scanLine( y );
		for( int x = image.width(); x != 0; x--, clr++ ) {
			int red = qRed(*clr) + delta;
			int green = qGreen(*clr) + delta;
			int blue = qBlue(*clr) + delta;

			if( red > 255 ) red = 255;
			if( green > 255 ) green = 255;
			if( blue > 255 ) blue = 255;

			if( red < 0 ) red = 0;
			if( green < 0 ) green = 0;
			if( blue < 0 ) blue = 0;

			*clr = qRgb( red, green, blue );
		}
	}
}

void GreyFilter::invoke( QImage before )
{
	int olddepth = 0;

	emit status( i18n( "Converting to Greyscale.." ) );

	QApplication::setOverrideCursor( waitCursor );    

	if ( before.depth() < 32 ) {
		olddepth = before.depth();
		before.convertDepth( 32 );
	}

	int cells = before.height() * before.width();
	QRgb *c = (QRgb *)before.bits();

	for( int y = cells; y ; y--, c++ ) {
		int g = qGray( *c );
		*c = qRgb( g, g, g );
	}

	if ( olddepth != 0 ) {
		before.convertDepth( olddepth );
	}

	QApplication::restoreOverrideCursor();

	emit changed( before );
	emit status( 0 );
}

const char *GreyFilter::name() const
{
	return i18n( "Greyscale" );
}

KImageFilter *GreyFilter::clone() const
{
	return new GreyFilter;
}

void SmoothFilter::invoke( QImage before )
{
	int olddepth = 0;
	int delta[ 8 ];
	int i;

	emit status( i18n( "Smoothing.." ) );
	QApplication::setOverrideCursor( waitCursor );    

	QImage after = before;

	if ( after.depth() < 32 ) {
		olddepth = after.depth();
		after.convertDepth( 32 );
	}

	// init delta table

	delta[ 0 ] = - (after.width() - 1);
	delta[ 1 ] = - after.width();
	delta[ 2 ] = - ( after.width() + 1);
	delta[ 3 ] = - 1;

	for( i = 0; i < 4; i ++ )
		delta[ i + 4 ] = - delta[ i ];


	int cells = after.height() * after.width();
	QRgb *c = (QRgb *)after.bits();

	setMaxProgress( cells );

	for( int y = 0; y < cells ; y++, c++ ) {
		int r = qRed( *c );
		int g = qGreen( *c );
		int b = qBlue( *c );
		int cnt = 1;

		for (  i = 0; i < 8; i ++ ) {
			int v = y + delta[ i ];
			if ( v >= 0 && v < cells ) {
				r += qRed( *( c + delta[ i ]) );
				g += qGreen( *( c + delta[ i ]) );
				b += qBlue( *( c + delta[ i ]) );
				cnt++;
			}
		}

		if ( cnt > 1 ) {
			*c = qRgb( r / cnt, g / cnt, b / cnt );
		}

		if ( y % 50000 == 0 ) {
			setProgress( y );
			qApp->processEvents( 100 );
		}
	}


	if ( olddepth != 0 ) {
		after.convertDepth( olddepth );
	}

	QApplication::restoreOverrideCursor();
	setProgress( 0 );

	emit changed( after );
	emit status( 0 );
}

const char *SmoothFilter::name() const
{
	return i18n( "Smooth" );
}

KImageFilter *SmoothFilter::clone() const
{
	return new SmoothFilter;
}
