 
/**
* kimgio.h -- Implementation of interface to the KDE Image IO library.
* Sirtaj Singh Kang <taj@kde.org>, 23 Sep 1998.
*
* $Id$
*
* This library is distributed under the conditions of the GNU LGPL.
*/

static int registered = 0;

#include"jpeg.h"
#include"xview.h"
#include"eps.h"
#include"tiffr.h"

#ifdef HAVE_CONFIG_H
#include"config.h"
#endif

#include<qimage.h>

void kimgioRegister(void)
{
	if( registered ) {
		return;
	}

	registered = 1;

#ifdef HAVE_LIBJPEG
	// JPEG
	QImageIO::defineIOHandler("JFIF","^\377\330\377\340..JFIF", 0,
			read_jpeg_jfif, 0 );
#endif

	// XV thumbnails
	QImageIO::defineIOHandler( "XV", "^P7 332", 0, read_xv_file,
		write_xv_file );

	QImageIO::defineIOHandler("PS","^%!PS-Adobe-[1-2]", 0,
                read_ps_epsf, NULL);

#ifdef HAVE_LIBTIFF
	QImageIO::defineIOHandler("TIFF","MM", 0,
                kimg_read_tiff, kimg_write_tiff );
#endif
}
