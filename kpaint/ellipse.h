// $Id$

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Ellipse : public Tool
{
  Q_OBJECT
public:
  Ellipse();
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap *pixmap();
signals:
  void modified();
private:
  int startx, starty;
  int lastx, lasty;
  int activeButton;
  bool drawing;
};

#endif // ELLIPSE_H

