#include <qwidget.h>
#include <qimage.h>
#include <qstring.h>
#include <qevent.h>
#include "kdragsource.h"

KURLDrag::KURLDrag( QStrList urls,
		      QWidget * dragSource, const char * name )
    : QStoredDrag( "text/url", dragSource, name )
{
    setURL( urls );
}

KURLDrag::KURLDrag( QWidget * dragSource, const char * name )
    : QStoredDrag( "text/url", dragSource, name )
{
}

KURLDrag::~KURLDrag()
{
    // nothing
}


void KURLDrag::setURL( QStrList urls )
{
    QString text = "";

    for(uint i = 0; i < urls.count(); i++)
    {
      text += urls.at((int)i);
      text += "\n";
    }
    int l = text.length();
    QByteArray tmp(l);
    memcpy(tmp.data(),text.data(),l);
    setEncodedData( tmp );
}

bool KURLDrag::canDecode( QDragMoveEvent* e )
{
    return e->provides( "text/url" );
}

bool KURLDrag::decode( QDropEvent* e, QString& str )
{
    QByteArray payload = e->data( "text/url" );
    if ( payload.size() ) {
	e->accept();
	str = QString( payload.size()+1 );
	memcpy( str.data(), payload.data(), payload.size() );
	return TRUE;
    }
    return FALSE;
}

bool KURLDrag::decode( QDropEvent* e, QStrList urls )
{
    QString str, value;
    QByteArray payload = e->data( "text/url" );
    if ( payload.size() ) {
	e->accept();
	str = QString( payload.size()+1 );
	memcpy( str.data(), payload.data(), payload.size() );

        int l = str.length();
        for(int i = 0; i < l; i++ ) {
          if( str[i] != '\n') {
            value += str[i];
	    continue;
	  }
          urls.append( value );
          value.truncate(0);
        }

	return TRUE;
    }
    return FALSE;
}

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
  else debug("Unknown datatype: %s", dragtype);
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
    emit getimage(&img);
    if(img.isNull())
      debug("Bad image");
    else
    {
      QImageDrag di( img, this );
      debug("KDragSource::mousePressEvent - before dragCopy");
      di.dragCopy();
    }
  }
  else debug("Unknown datatype: %s", type.data());

  debug("KDragSource::mousePressEvent - done");
}

void KDragSource::mouseMoveEvent( QMouseEvent * /*e*/ )
{
}
