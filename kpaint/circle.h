// $Id$

#ifndef CIRCLE_H
#define CIRCLE_H

#include "tool.h"

class Circle : public Tool
{
public:
  Circle(void);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating(void);
  QPixmap *pixmap(void);
private:
  int startx, starty;
  int lastx, lasty;
  bool drawing;
};

#endif // CIRCLE_H

