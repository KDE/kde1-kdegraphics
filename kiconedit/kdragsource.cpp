#include <qwidget.h>
#include <qimage.h>
#include <qstring.h>
#include <kurl.h>
#include <qevent.h>
#include "kdragsource.h"
#include "kicongrid.h"

KDragSource::KDragSource( const char *dragtype, QObject *dataprovider, const char *method, 
                                               QWidget *parent, const char * name )
    : QLabel( "DragSource", parent, name )
{
  provider = dataprovider;
  type = dragtype;
  if(type.left(6) == "image/")
  {
    ok = connect(this, SIGNAL(getimage(QImage*)), provider, method);
  }
}

KDragSource::~KDragSource()
{
}

void KDragSource::mousePressEvent( QMouseEvent * /*e*/ )
{
  debug("KDragSource::mousePressEvent");

  if(!ok) return;

  if(type.left(6) == "image/")
  {
    debug("Type: image");
    QImage img;
    //emit getimage(img);
    img = ((KIconEditGrid*)provider)->image();
    if(img.isNull())
      debug("Bad image");
    else
    {
      QImageDrag *di = new QImageDrag( img, this );
      debug("KDragSource::mousePressEvent - before dragCopy");
      di->dragCopy();
    }
    debug("KDragSource::mousePressEvent - done");
  }
  else debug("Bad datatype");
}

void KDragSource::mouseMoveEvent( QMouseEvent * /*e*/ )
{
}
