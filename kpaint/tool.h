// -*- c++ -*-

// $Id$

#ifndef TOOL_H
#define TOOL_H

#include <qevent.h>
#include <stdio.h>
#include <assert.h>
#include <qpen.h>
#include <qobject.h>
#include <qbrush.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qpixmap.h>
#include "canvas.h"

/** @short Tool - Abstract base class
 * Tool is the parent of all kpaint tools, it provides virtual methods
 * for all of the events that are passed to the current tool by the
 * canvas. The default implementation of all of these methods is to
 * ignore the event. Tools should override those methods to provide
 * functionality.
 * 
 * A tool can be either active or inactive, an inactive tool should
 * receive no events. When a tool is active it can access a pointer
 * to the current canvas which it can use to find the current image
 * in order to draw on it. The activating() method is called when a
 * tool is made active, the deactivating() method is called immediately
 * before the tool becomes inactive.
 */
class Tool : public QObject
{
  Q_OBJECT
public:
  // Page flags
  enum {HasLineProperties = 1, HasFillProperties = 2, HasCustomProperties = 4};

  Tool(void);
  void activate(Canvas *c); // Make the tool active for canvas c
  void deactivate(void); // Make the tool inactive
  bool isActive(void);
  void setPen(QPen *p);
  void setBrush(QBrush *b);
  const char *tip(void) { return tooltip; };
  virtual QPixmap *pixmap(void);

  // Used by the properties dialog
  int getPages(void);
  QWidget *getCustomPage(QWidget *) { return NULL; };

  // Event handlers (handle events on canvas)
  virtual void mousePressEvent(QMouseEvent *) {} ;
  virtual void mouseReleaseEvent(QMouseEvent *) {};
  virtual void mouseDoubleClickEvent(QMouseEvent *) {};
  virtual void mouseMoveEvent(QMouseEvent *) {};
  virtual void keyPressEvent(QKeyEvent *) {};
  virtual void keyReleaseEvent(QKeyEvent *) {};
  virtual void enterEvent(QEvent *) {};
  virtual void leaveEvent(QEvent *) {};

private:
  bool active;

protected:
  QPixmap *pix;
  const char *tooltip;
  Canvas *canvas; // Current canvas if active
  virtual void activating(void) {}; // Called by activate()
  virtual void deactivating(void) {}; // Called by deactivate()
  QPen *pen;
  QBrush *brush;
  unsigned int props; // Which pages are supported
};

#endif // TOOL_H

