#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <qpixmap.h>
#include "tool.h"

class Rectangle : public Tool
{
public:
  Rectangle(void);
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

#endif // RECTANGLE_H

