// $Id$

#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "areaselect.h"
#include "app.h"

#define KPDEBUG

extern MyApp *kpaintApp;

AreaSelect::AreaSelect() : Tool()
{
  drawing= FALSE;

  tooltip= klocale->translate("Area Selection");
  props= 0;
}

AreaSelect::~AreaSelect()
{
}

void AreaSelect::activating()
{
#ifdef KPDEBUG
  fprintf(stderr, "AreaSelect::activating() hook called\n");
#endif
  canvas->setCursor(crossCursor);
}

void AreaSelect::mousePressEvent(QMouseEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "AreaSelect::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
 	      "AreaSelect: Warning Left Button press received when pressed\n");
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

void AreaSelect::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());
	paint.setPen(QPen(green, 0, DashLine));
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

void AreaSelect::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

#ifdef KPDEBUG
  fprintf(stderr, "AreaSelect::mouseReleaseEvent() handler called\n");
#endif

  if (isActive() && (e->button() == LeftButton)) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(QPen(green, 0, DashLine));
    paint.setRasterOp(XorROP);
    paint.drawRect(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    paint.setPen(*pen);
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    canvas->setSelection(QRect(startx, starty, x-startx, y-starty));
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

QPixmap *AreaSelect::pixmap()
{
  QString pixdir;

  pixdir= kpaintApp->kdedir();
  pixdir.append("/share/apps/kpaint/toolbar/");
  pixdir.append("areaselect.xpm");
  return new QPixmap(pixdir);
}
