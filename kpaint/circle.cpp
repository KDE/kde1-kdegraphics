// $Id$

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "circle.h"
#include <qcursor.h>
#include <qpainter.h>

Circle::Circle() : Tool()
{
  drawing= FALSE;

  //  props= propertiesDialog::displayLineProperties
  //    | displayFillProperties;
  props= 3;
}

void Circle::activating(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "Circle::activating() hook called\n");
#endif

  p= canvas->pixmap();
  canvas->setCursor(crossCursor);
}

void Circle::mousePressEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

#ifdef KPDEBUG
  fprintf(stderr, "Circle::mousePressEvent() handler called\n");
#endif
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      fprintf(stderr,
 	      "Circle: Warning Left Button press received when pressed\n");
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

void Circle::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  int bbx, bby, r;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    //#ifdef KPDEBUG
    //    if (drawing)
    //      fprintf(stderr, "Circle:: start (%d,%d) current (%d,%d) last (%d,%d)\n",
    //	      startx, starty, x, y, lastx, lasty);
    //#endif

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(p);
	paint.setPen(*pen);
	paint.setRasterOp(XorROP);

	// Erase old circle
	// The test is to prevent problems when the circle is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the circle
	// grows).
	r= sqrt( (startx-lastx)*(startx-lastx)+(starty-lasty)*(starty-lasty) );

	bbx= startx-r;
	bby= starty-r;

	if (r >= 2)
	  paint.drawEllipse(bbx, bby, 2*r, 2*r);
	// Draw new circle
	r= sqrt( (startx-x)*(startx-x)+(starty-y)*(starty-y) );

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
    fprintf(stderr, "Warning move event received when inactive (ignoring)\n");
  }
}

void Circle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  int bbx, bby, r;

#ifdef KPDEBUG
  fprintf(stderr, "Circle::mouseReleaseEvent() handler called\n");
#endif

  if (isActive() && (e->button() == LeftButton) && drawing) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old circle
    paint.begin(p);
    paint.setPen(*pen);
    paint.setRasterOp(XorROP);
    // Erase old circle
    // The test is to prevent problems when the circle is smaller
    // than 2 by 2 pixels. (It leaves a point behind as the circle
    // grows).

    r= sqrt( (startx-lastx)*(startx-lastx)+(starty-lasty)*(starty-lasty) );

    bbx= startx-r;
    bby= starty-r;

    if (r >= 2)
      paint.drawEllipse(bbx, bby, 2*r, 2*r);


    paint.setRasterOp(CopyROP);
    // Draw new circle
    r= sqrt( (startx-x)*(startx-x)+(starty-y)*(starty-y) );

    bbx= startx-r;
    bby= starty-r;

    if (r >= 2) 
      paint.setBrush(*brush);
      paint.drawEllipse(bbx, bby, 2*r, 2*r);

    lastx= x;
    lasty= y;

    paint.end();
    drawing= FALSE;
    canvas->repaint(0);
  }
}





























