#include <qstring.h>
#include "version.h"
#include "app.h"
#include "formats.h"

MyApp *kpaintApp;
FormatManager *formatMngr;

int main( int argc, char **argv )
{
  formatMngr= new FormatManager();
  kpaintApp= new MyApp ( argc, argv, APPNAME);

  return kpaintApp->exec();
}

