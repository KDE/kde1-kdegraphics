// $Id$

#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include "ellipse.h"

Ellipse::Ellipse() : Tool()
{
  drawing= FALSE;

  //  props= propertiesDialog::displayLineProperties
  //    | displayFillProperties;
  props= 3;
}

void Ellipse::activating(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "Ellipse::activating() hook called\n");
#endif
  p= canvas->pixmap();
  canvas->setCursor(crossCursor);
}

void Ellipse::mousePressEvent(QMouseEvent *e)
{
  int x,y;


#ifdef KPDEBUG
  fprintf(stderr, "Ellipse::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
 	      "Ellipse: Warning Left Button press received when pressed\n");
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

void Ellipse::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    //#ifdef KPDEBUG
    //    if (drawing)
    //      fprintf(stderr, "Ellipse:: start (%d,%d) current (%d,%d) last (%d,%d)\n",
    //	      startx, starty, x, y, lastx, lasty);
    //#endif

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(p);
	paint.setPen(*pen);
	paint.setRasterOp(XorROP);

	// Erase old ellipse
	// The test is to prevent problems when the ellipse is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the ellipse
	// grows).
	if (abs(lastx-startx)*abs(lasty-starty) >= 4) 
	paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);
	// Draw new ellipse
	if (abs(x-startx)*abs(y-starty) > 4) 
		   paint.drawEllipse(startx, starty, x-startx, y-starty);

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

void Ellipse::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

#ifdef KPDEBUG
  fprintf(stderr, "Ellipse::mouseReleaseEvent() handler called\n");
#endif

  if (isActive() && (e->button() == LeftButton) && drawing) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old ellipse
    paint.begin(p);
    paint.setPen(*pen);
    paint.setRasterOp(XorROP);
	if (abs(lastx-startx)*abs(lasty-starty) > 4) 
	   paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);
    paint.setRasterOp(CopyROP);
    // Draw new ellipse
    if (abs(x-startx)*abs(y-starty) > 4) {
      paint.setBrush(*brush);
      paint.drawEllipse(startx, starty, x-startx, y-starty);
    }
    paint.end();
    drawing= FALSE;
    canvas->repaint(0);
  }
  else {
    fprintf(stderr, "Warning event received when inactive (ignoring)\n");
  }
}

