////////////////
//
// jpeg.h -- QImage IO handler declaration for JFIF JPEG graphic format,
//           using IJG JPEG library.
//

// $Id$

#ifndef _SSK_JPEG_QHANDLERS_H
#define _SSK_JPEG_QHANDLERS_H

class QImageIO;

//////
// JPEG IO handlers for QImage.
//

void read_jpeg_jfif(QImageIO *image);
void write_jpeg_jfif(QImageIO *image);

#endif
