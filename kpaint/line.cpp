// $Id$

// Note: When used to draw more than one line segment should use draw poly line
// functions to avoid glitches at the joints.

#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include "line.h"

Line::Line() : Tool()
{
  drawing= FALSE;
  props= 1;
}

void Line::activating(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "Line::activating() hook called\n");
#endif

  p= canvas->pixmap();
  canvas->setCursor(crossCursor);
}

void Line::mousePressEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

#ifdef KPDEBUG
  fprintf(stderr, "RubberLine::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
	      "Line: Warning Left Button press received when pressed\n");
    }
    else {
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    }
  }
  else if (isActive() && (e->button() == RightButton) && drawing) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(p);
	paint.setPen(*pen);
    paint.setRasterOp(XorROP);
    paint.drawLine(startx, starty, lastx, lasty);
    paint.setRasterOp(CopyROP);
    // Draw new line
    paint.drawLine(startx, starty, lastx, lasty);
    paint.end();

    startx= x;
    starty= y;
    lastx= startx;
    lasty= starty;
  }

  canvas->repaint(0);
  
  if (!isActive()) {
    fprintf(stderr, "Line: Warning event received when inactive (ignoring)\n");
  }
  
}

void Line::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(p);
	paint.setPen(*pen);
	paint.setRasterOp(XorROP);

	// Draw new line
	paint.drawLine(startx, starty, x, y);
	// Erase old line
	paint.drawLine(startx, starty, lastx, lasty);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
    fprintf(stderr, "Line: Warning event received when inactive (ignoring)\n");
  }
}

void Line::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

#ifdef KPDEBUG
  fprintf(stderr, "Line::mouseReleaseEvent() handler called\n");
#endif

  if (isActive() && drawing && (e->button() == LeftButton)) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(p);
	paint.setPen(*pen);
    paint.setRasterOp(XorROP);
    paint.drawLine(startx, starty, lastx, lasty);
    paint.setRasterOp(CopyROP);
    // Draw new line
    paint.drawLine(startx, starty, lastx, lasty);
    paint.end();
    drawing= FALSE;
    canvas->repaint(0);
  }
  else {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

