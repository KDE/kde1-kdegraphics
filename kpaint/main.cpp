#include <qimage.h>
#include <qstring.h>
#include "version.h"
#include "app.h"

#ifdef HAVE_LIBGIF
#include "gif.h"
#endif

#ifdef HAVE_LIBJPEG
#include "jpeg.h"
#endif

Window mwin;

int main( int argc, char **argv )
{
  MyApp a( argc, argv, QString(APPNAME) );

#ifdef HAVE_LIBGIF
  QImageIO::defineIOHandler("GIF", "^GIF[0-9][0-9][a-z]",
			    0, read_gif_file, NULL);
#endif
#ifdef HAVE_LIBJPEG
  QImageIO::defineIOHandler("JFIF","^\377\330\377\340",
			    0, read_jpeg_jfif, NULL);
#endif

  return a.exec();
}

