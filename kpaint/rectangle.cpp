// $Id$

#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include "rectangle.h"
#include "../app.h"

extern MyApp *kpaintApp;

Rectangle::Rectangle() : Tool()
{
  drawing= FALSE;

  tooltip= "Rectangle";
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Rectangle::activating(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "Rectangle::activating() hook called\n");
#endif
  canvas->setCursor(crossCursor);
}

void Rectangle::mousePressEvent(QMouseEvent *e)
{
  int x,y;

#ifdef KPDEBUG
  fprintf(stderr, "Rectangle::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
 	      "RubberLine: Warning Left Button press received when pressed\n");
    }
    else {
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    } 
  }
  if (!isActive()) {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
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
	paint.setPen(*pen);
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
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

void Rectangle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

#ifdef KPDEBUG
  fprintf(stderr, "Rectangle::mouseReleaseEvent() handler called\n");
#endif

  if (isActive() && (e->button() == LeftButton)) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(*pen);
    paint.setRasterOp(XorROP);
    paint.drawRect(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    paint.setPen(*pen);
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    // Draw new rectangle
	paint.setBrush(*brush);
    paint.drawRect(startx, starty, x-startx, y-starty);
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *Rectangle::pixmap()
{
  QString pixdir;

  pixdir= kpaintApp->kdedir();
  pixdir.append("/lib/pics/toolbar/");
  pixdir.append("rectangle.xpm");
  return new QPixmap(pixdir);
}
