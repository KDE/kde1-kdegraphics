// $Id$

#include <kdebug.h>
#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "rectangle.h"
#include "app.h"

Rectangle::Rectangle() : Tool()
{
  drawing= FALSE;

  tooltip= i18n("Rectangle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Rectangle::activating()
{
KDEBUG(KDEBUG_INFO, 3000, "Rectangle::activating() hook called\n");
  canvas->setCursor(crossCursor);
}

void Rectangle::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Rectangle::mousePressEvent() handler called\n");
  
  if (isActive()) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "RubberLine: Warning Left Button press received when pressed\n");
    }
    else {
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      activeButton= e->button();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    } 
  }
  if (!isActive()) {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void Rectangle::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(XorROP);

	// Erase old rectangle
	paint.drawRect(startx, starty, lastx-startx, lasty-starty);
	// Draw new rectangle
	paint.drawRect(startx, starty, x-startx, y-starty);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void Rectangle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

KDEBUG(KDEBUG_INFO, 3000, "Rectangle::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == activeButton)) {
    emit modified();
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);

    paint.setRasterOp(XorROP);
    paint.drawRect(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());

    if (activeButton == LeftButton) {
      paint.setPen(leftpen);
      paint.setBrush(leftbrush);
    }
    else {
      paint.setPen(rightpen);
      paint.setBrush(rightbrush);
    }

    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);

    paint.drawRect(startx, starty, x-startx, y-starty);
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Rectangle::pixmap()
{
  QString pixdir;

  pixdir= myapp->kde_datadir().copy();
  pixdir.append("/kpaint/toolbar/");
  pixdir.append("rectangle.xpm");
  return new QPixmap(pixdir);
}

#include "rectangle.moc"

