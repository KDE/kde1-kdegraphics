// $Id$

// Tool - Abstract base class
// Tool is the parent of all kpaint tools, it provides virtual methods for all
// of the events that are passed to the current tool by the canvas. The default
// implementation of all of these methods is to ignore the event. Tools should
// override those methods to provide functionality.
// A tool can be either active or inactive, an inactive tool should receive no
// events. When a tool is active it can access a pointer to the current canvas
// which it can use to find the current image in order to draw on it. The
// activating() method is called when a tool is made active, the
// deactivating() method is called immediately before the tool becomes
// inactive.

#ifndef TOOL_H
#define TOOL_H

#include <qevent.h>
#include <stdio.h>
#include <assert.h>
#include <qpen.h>
#include <qbrush.h>
#include "../canvas.h"

class Tool {
public:
  Tool(void);
  void activate(Canvas *c); // Make the tool active for canvas c
  void deactivate(); // Make the tool inactive
  bool isActive();
  void setPen(QPen *p);
  void setBrush(QBrush *b);

  // Event handlers (handle events on canvas)
  virtual void mousePressEvent(QMouseEvent *e) {} ;
  virtual void mouseReleaseEvent(QMouseEvent *e) {};
  virtual void mouseDoubleClickEvent(QMouseEvent *e) {};
  virtual void mouseMoveEvent(QMouseEvent *e) {};
  virtual void keyPressEvent(QKeyEvent *e) {};
  virtual void keyReleaseEvent(QKeyEvent *e) {};
  virtual void enterEvent(QEvent *e) {};
  virtual void leaveEvent(QEvent *e) {};

private:
  bool active;

protected:
  Canvas *canvas; // Current canvas if active
  virtual void activating(void) {}; // Called by activate()
  virtual void deactivating(void) {}; // Called by deactivate()
  QPen *pen;
  QBrush *brush;
  unsigned int props; // Which pages are supported
};

#endif // TOOL_H

