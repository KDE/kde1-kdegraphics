// $Id$

#ifndef PEN_H
#define PEN_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Pen : public Tool
{
public:
  Pen(void);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating(void);
  QPixmap *pixmap(void);
private:
  int lastx;
  int lasty;
  bool drawing;
};

#endif // PEN_H

