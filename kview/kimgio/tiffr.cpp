
#ifdef HAVE_CONFIG_H
#include"config.h"
#endif

#ifdef HAVE_LIBTIFF

#include<tiffio.h>
#include<qimage.h>

#include"tiffr.h"

void kimg_read_tiff( QImageIO *io )
{
	TIFF *tiff;
	uint32 width, height;
	uint32 *data;

	// FIXME: use qdatastream

	// open file
	tiff = TIFFOpen( io->fileName(), "r" );

	if( tiff == 0 ) {
		return;
	}

	// create image with loaded dimensions
	TIFFGetField( tiff, TIFFTAG_IMAGEWIDTH,	&width );
	TIFFGetField( tiff, TIFFTAG_IMAGELENGTH, &height );

	QImage image( width, height, 32 );
	data = (uint32 *)image.bits();

	// read data
	bool stat =TIFFReadRGBAImage( tiff, width, height, data );

	if( stat == 0 ) {
		TIFFClose( tiff );
		return;
	}

	for( int ctr = image.numBytes(); ctr ; ctr-- ) {
		// TODO: manage alpha with TIFFGetA
		*data = qRgb( TIFFGetR( *data ), 
			TIFFGetG( *data ), TIFFGetB( *data ) );
		data++;
	}
	TIFFClose( tiff );

	io->setImage( image );
	io->setStatus ( 0 );
}

void kimg_write_tiff( QImageIO * )
{
	// TODO: stub
}
#endif
