#include <qwidget.h>
#include <qimage.h>
#include <qstring.h>
#include <kurl.h>
#include "kdragsource.h"
#include <qevent.h>

KDragSource::KDragSource( const char *dragtype, QObject *provider, const char *method, 
                                               QWidget *parent, const char * name )
    : QLabel( "DragSource", parent, name )
{
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
    QImage *img = 0L;
    emit getimage(img);
    if(!img || img->isNull())
      debug("Bad image");
    else
    {
      QImageDrag *di = new QImageDrag( *img, this );
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
