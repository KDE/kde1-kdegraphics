// $Id$

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <qpainter.h>
#include <math.h>
#include "canvas.h"
#include "tools/tool.h"

Canvas::Canvas(int width, int height,
	       QWidget *parent= 0, const char *name=0)
  : QWidget(parent)
{
  currentTool= 0;
  s= INACTIVE;
  matrix= new QWMatrix;
  zoomed= NULL;

  // Create pixmap
  pix= new QPixmap(width, height);
  if (!pix) {
    fprintf(stderr, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }

  pix->fill(QColor("white"));

  setZoom(100);

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
  emit sizeChanged();
}

Canvas::Canvas(const char *filename, QWidget *parent= 0, const char *name=0)
  : QWidget(parent, name)
{
  currentTool= 0;
  s= INACTIVE;
  zoomed= NULL;
  matrix= new QWMatrix;

  // Create pixmap
  pix= new QPixmap(filename);
  if (!pix) {
    fprintf(stderr, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }

  resize(pix->width(), pix->height());

  setZoom(100);

  emit sizeChanged();

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
}

void Canvas::setZoom(int z)
{
  QPainter p;
  int w, h;

  zoomFactor= z;
  matrix->reset();
  matrix->scale((float) z/100, (float) z/100);

  if (zoomed != NULL)
    delete zoomed;

  w= (int) (pix->width()* ((float) zoomFactor/100));
  h= (int) (pix->height()*((float) zoomFactor/100));

  zoomed= new QPixmap(w, h);
  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  if ((w != width()) || (h != height())) {
    resize(w,h);
    emit sizeChanged();
  }
  repaint(0);
}

void Canvas::updateZoomed(void)
{
  QPainter p;
  int w, h;

  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  repaint(0);
}

int Canvas::zoom(void)
{
  return zoomFactor;
}

void Canvas::activate(Tool *t)
{
  assert(!isActive());
  currentTool= t;
  s= ACTIVE;
}

void Canvas::deactivate()
{
  assert(isActive());
  s= INACTIVE;
  currentTool= 0;
}

QPixmap *Canvas::pixmap()
{
  return pix;
}

QPixmap *Canvas::zoomedPixmap(void)
{
  return zoomed;
}

void Canvas::setPixmap(QPixmap *px)
{
  QPainter p;
  int w, h;

  *pix= *px;

  delete zoomed;

  w= (int) (px->width()* ((float) zoomFactor/100));
  h= (int) (px->height()*((float) zoomFactor/100));

  zoomed= new QPixmap(w, h);

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  if ((w != width()) || (h != height())) {
    resize(w,h);
    emit sizeChanged();
  }
  repaint(0);
}

void Canvas::paintEvent(QPaintEvent *e)
{
  bitBlt(this, 0, 0, zoomed);
}

void Canvas::mousePressEvent(QMouseEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "Canvas::mousePressEvent() redirector called\n");
#endif
  if (isActive())
    currentTool->mousePressEvent(e);
} 

void Canvas::mouseMoveEvent(QMouseEvent *e)
{
  // #ifdef KPDEBUG
  //   fprintf(stderr, "Canvas::mouseMoveEvent() redirector called\n");
  // #endif
  if (isActive())
    currentTool->mouseMoveEvent(e);
}

void Canvas::mouseReleaseEvent(QMouseEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "Canvas::mouseReleaseEvent() redirector called\n");
#endif
  if (isActive())
    currentTool->mouseReleaseEvent(e);
}

bool Canvas::isActive()
{
  if (s == ACTIVE)
    return true;
  else
    return false;
}

bool Canvas::load(const char *filename= 0, const char *format= 0)
{
  bool s;
  QPixmap p;
  QPixmap q; // Fix UMR when reading transparent pixels (they hold junk)

  if (!format)
    s= p.load(filename);
  else
    s= p.load(filename, format);

  if (s) {
    q.resize(p.size());
    q.fill(QColor("white"));
    bitBlt(&q, 0, 0, &p);
    setPixmap(&q);
  }

  repaint(0);

  return s;
}

bool Canvas::save(const char *filename=0, const char *format= 0)
{
  bool s;

#ifdef KPDEBUG
  fprintf(stderr, "Canvas::save() file= %s, format= %s\n", filename, format);
#endif	  

  s= pix->save(filename, format);

#ifdef KPDEBUG
  fprintf(stderr, "Canvas::save() returning %d\n", s);
#endif	  

  return s;
}

void Canvas::keyPressEvent(QKeyEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "Canvas::keyPressEvent() redirector called\n");
#endif
  if (isActive())
    currentTool->keyPressEvent(e);
}


void Canvas::keyReleaseEvent(QKeyEvent *e)
{
#ifdef KPDEBUG
  fprintf(stderr, "Canvas::keyReleaseEvent() redirector called\n");
#endif
  if (isActive())
    currentTool->keyReleaseEvent(e);
}


#include "canvas.moc"
