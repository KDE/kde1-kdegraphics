// $Id$

#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include "pen.h"

Pen::Pen() : Tool()
{
  drawing= FALSE;
   
  props= 1;
}

void Pen::activating(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "Pen::activating() hook called canvas=%p\n", canvas);
#endif
  drawing= FALSE;
  p= canvas->pixmap();
  canvas->setCursor(crossCursor);
}

void Pen::mousePressEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

#ifdef KPDEBUG
  fprintf(stderr, "Pen::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
 	      "Pen: Warning Left Button press received when pressed\n");
    }
    x= (e->pos()).x();
    y= (e->pos()).y();

    paint.begin(p);
    paint.setPen(*pen);
    paint.drawPoint(x, y);
    paint.end();
    canvas->repaint(0);
    lastx= x;
    lasty= y;
    drawing= TRUE;
  } 
  if (!isActive()) {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}


void Pen::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if (drawing) {
      if ((x != lastx) || (y != lasty)) {
		 paint.begin(p);
		 paint.setPen(*pen);
		 paint.drawLine(lastx, lasty, x, y);
		 paint.end();
		 lastx= x;
		 lasty= y;
		 canvas->repaint(0);
	  }
    }
  }
  else {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

void Pen::mouseReleaseEvent(QMouseEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "Pen::mouseReleaseEvent() handler called\n");
#endif
  if (isActive() && (e->button() == LeftButton) && drawing)
    drawing= FALSE;
  if (!isActive()) {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

