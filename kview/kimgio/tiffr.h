/**
* QImageIO Routines to read/write TIFF images.
* Sirtaj Singh Kang, Oct 1998.
*
* $Id$
*/

#ifndef KIMG_TIFFR_H
#define KIMG_TIFFR_H

class QImageIO;

void kimg_read_tiff( QImageIO *io );
void kimg_write_tiff( QImageIO *io );

#endif
