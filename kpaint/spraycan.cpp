// $Id$

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "spraycan.h"
#include "app.h"

extern MyApp *kpaintApp;


SprayCan::SprayCan() : Tool()
{
  drawing= FALSE;
  brushsize= 10;
  density= 100;
  tooltip= klocale->translate("Spray Can");
  props= Tool::HasLineProperties | Tool::HasCustomProperties;

  timer= new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(drawDot()) );
}

SprayCan::~SprayCan(void)
{
  delete timer;
}

void SprayCan::activating(void)
{
KDEBUG1(KDEBUG_INFO, 3000, "SprayCan::activating() hook called canvas=%p\n", canvas);
  drawing= FALSE;

  canvas->setCursor(crossCursor);
}

void SprayCan::mouseMoveEvent(QMouseEvent *e)
{
  if (isActive() && drawing) {
      x= (e->pos()).x();
      y= (e->pos()).y();
  }
}

void SprayCan::mousePressEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "SprayCan::mousePressEvent() handler called\n");
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      KDEBUG(KDEBUG_INFO, 3000, "SprayCan: Warning Left Button press received when pressed\n");
    }
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Start the timer (multishot)
    timer->start(50, FALSE);

    drawing= TRUE;
  }
}


void SprayCan::mouseReleaseEvent(QMouseEvent *e)
{
KDEBUG(KDEBUG_INFO, 3000, "SprayCan::mouseReleaseEvent() handler called\n");
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      // Stop the timer
      timer->stop();
    }
    drawing= FALSE;
    canvas->updateZoomed();
  }
}

void SprayCan::drawDot(void)
{
  int dx,dy;
  int i;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

  m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
  painter1.begin(canvas->pixmap());
  painter1.setPen(*pen);
  painter1.setWorldMatrix(m);

  painter2.begin(canvas->zoomedPixmap());
  painter2.setPen(*pen);

  painter1.drawPoint(x, y);
  painter2.drawPoint(x, y);

  for (i= 0; i < (density/20); i++) {
    dx= (rand() % (2*brushsize))-brushsize;
    dy= (rand() % (2*brushsize))-brushsize;
    painter1.drawPoint(x+dx, y+dy);
    painter2.drawPoint(x+dx, y+dy);
  }
  painter1.end();
  painter2.end();
  canvas->repaint(0);
}

QPixmap *SprayCan::pixmap()
{
  QString pixdir;

  pixdir= kpaintApp->kdedir().copy();
  pixdir.append("/share/apps/kpaint/toolbar/");

  pixdir.append("spraycan.xpm");
  return new QPixmap(pixdir);
}

#include "spraycan.moc"




