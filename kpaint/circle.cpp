// $Id$

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "math.h"
#include "circle.h"
#include "app.h"

extern MyApp *kpaintApp;

Circle::Circle() : Tool()
{
  drawing= FALSE;
  tooltip= klocale->translate("Circle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Circle::activating(void)
{
KDEBUG(KDEBUG_INFO, 3000, "Circle::activating() hook called\n");

  canvas->setCursor(crossCursor);
}

void Circle::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "Circle::mousePressEvent() handler called\n");
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "Circle: Warning Left Button press received when pressed\n");
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
KDEBUG(KDEBUG_WARN, 3000, "Warning event received when inactive (ignoring)\n");
  }
}

void Circle::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  int bbx, bby, r;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());
	paint.setPen(*pen);
	paint.setRasterOp(XorROP);

	// Erase old circle
	// The test is to prevent problems when the circle is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the circle
	// grows).
	r= (int) sqrt( (startx-lastx)*(startx-lastx)+(starty-lasty)*(starty-lasty) );

	bbx= startx-r;
	bby= starty-r;

	if (r >= 2)
	  paint.drawEllipse(bbx, bby, 2*r, 2*r);
	// Draw new circle
	r= (int) sqrt( (startx-x)*(startx-x)+(starty-y)*(starty-y) );

	bbx= startx-r;
	bby= starty-r;

	if (r >= 2) 
	  paint.drawEllipse(bbx, bby, 2*r, 2*r);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
KDEBUG(KDEBUG_WARN, 3000, "Warning move event received when inactive (ignoring)\n");
  }
}

void Circle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;
  int bbx, bby, r;

KDEBUG(KDEBUG_INFO, 3000, "Circle::mouseReleaseEvent() handler called\n");

  if (isActive() && (e->button() == LeftButton) && drawing) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old circle
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(*pen);
    paint.setRasterOp(XorROP);
    // Erase old circle
    // The test is to prevent problems when the circle is smaller
    // than 2 by 2 pixels. (It leaves a point behind as the circle
    // grows).

    r= (int) sqrt( (startx-lastx)*(startx-lastx)+(starty-lasty)*(starty-lasty) );

    bbx= startx-r;
    bby= starty-r;

    if (r >= 2)
      paint.drawEllipse(bbx, bby, 2*r, 2*r);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    paint.setWorldMatrix(m);
    paint.setPen(*pen);
    paint.setRasterOp(CopyROP);
    // Draw new circle
    r= (int) sqrt( (startx-x)*(startx-x)+(starty-y)*(starty-y) );

    bbx= startx-r;
    bby= starty-r;

    if (r >= 2) 
      paint.setBrush(*brush);
      paint.drawEllipse(bbx, bby, 2*r, 2*r);

    lastx= x;
    lasty= y;

    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
}

QPixmap *Circle::pixmap()
{
  QString pixdir;

  pixdir= kpaintApp->kdedir().copy();
  pixdir.append("/share/apps/kpaint/toolbar/");
  pixdir.append("circle.xpm");
  return new QPixmap(pixdir);
}

