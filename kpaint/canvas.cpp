// $Id$

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include "canvas.h"
#include "tools/tool.h"

#include "canvas.moc"

Canvas::Canvas(const char *myname, int width, int height,
	       QWidget *parent= 0, const char *name=0)
  : QWidget(parent)
{
  currentTool= 0;
  s= INACTIVE;

  // Create pixmap
  pix= new QPixmap(width, height);
  if (!pix) {
    fprintf(stderr, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }
  resize(width, height);

  imageName= new QString(myname);
  pix->fill(backgroundColor());

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
}

Canvas::Canvas(const char *file, QWidget *parent= 0, const char *name=0)
  : QWidget(parent)
{
  currentTool= 0;
  s= INACTIVE;

  // Create pixmap
  pix= new QPixmap(file);
  if (!pix) {
    fprintf(stderr, "Canvas::Canvas(): Cannot create pixmap\n");
    exit(1);
  }

  imageName= new QString(file);
  resize(pix->size());

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
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

const char *Canvas::filename()
{
  const char *name= *imageName;
  return name;
}

void Canvas::paintEvent(QPaintEvent *e)
{
  // Check size, do we have a scroll bar?

  bitBlt(this, 0, 0, pix);
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

void Canvas::setFilename(const char *filename)
{
	delete imageName;
    imageName= new QString(filename);
}

bool Canvas::load(const char *filename, char *format= 0)
{
  bool s;
  s= pix->load(filename, format);

  if (s) {
   setFilename(filename);
   resize(pix->size());
  }

  repaint(0);

  return s;
}

bool Canvas::save(const char *file=0, char *format= 0)
{
  bool s;

#ifdef KPDEBUG
    fprintf(stderr, "Canvas::save() file= %s, format= %s\n", file, format);
#endif	  
  if (file == 0)
    s= pix->save(filename(), format);
  else {
        s= pix->save(file, "BMP");
    delete imageName;
    imageName= new QString(file);
  }
#ifdef KPDEBUG
    fprintf(stderr, "Canvas::save() returning %d\n", s);
#endif	  

  return s;
}

/*
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

*/


