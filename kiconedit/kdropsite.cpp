
#include "kdropsite.h"
#include <kapp.h>
#include <kiconloader.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qdragobject.h>
#include <qimage.h>


KDropSite::KDropSite( QWidget * parent ) : QObject( parent ), QDropSite( parent )
{
  debug("KDropSite constructor");
  createCursors();
}


KDropSite::~KDropSite()
{
    // nothing necessary
}


void KDropSite::dragMoveEvent( QDragMoveEvent *e )
{
  //debug("dragMove");
  emit dragMove(e);
}


void KDropSite::dragEnterEvent( QDragEnterEvent *e )
{
  //debug("dragEnter");
  emit dragEnter(e);
  if ( QTextDrag::canDecode( e ) )
    QApplication::setOverrideCursor(cursorText);
  else if ( QImageDrag::canDecode( e ) )
    QApplication::setOverrideCursor(cursorImage);

}

void KDropSite::dragLeaveEvent( QDragLeaveEvent *e )
{
  //debug("dragLeave");
  emit dragLeave(e);
  QApplication::restoreOverrideCursor();
}


void KDropSite::dropEvent( QDropEvent *e )
{
  //debug("drop");
  emit dropAction(e);
}

void KDropSite::createCursors()
{
  QBitmap bmp, mask(32, 32);
  QPixmap pix;

  pix = Icon("image.xpm");
  if(pix.isNull())
  {
    cursorImage = arrowCursor;
    debug("Error loading pixmap: image.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursorImage = QCursor(bmp, mask, 6, 18);
  }

  pix = Icon("txt.xpm");
  if(pix.isNull())
  {
    cursorText = arrowCursor;
    debug("Error loading pixmap: txt.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursorText = QCursor(bmp, mask, 5, 16);
  }
}


