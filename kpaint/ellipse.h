// $Id$

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Ellipse : public Tool
{
public:
  Ellipse(void);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating(void);
private:
  QPixmap *p;
  int startx, starty;
  int lastx, lasty;
  bool drawing;
};

#endif // ELLIPSE_H

