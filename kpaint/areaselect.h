// -*- c++ -*-

#ifndef AREASELECT_H
#define AREASELECT_H

#include <qpixmap.h>
#include "tool.h"

class AreaSelect : public Tool
{
public:
  AreaSelect();
  ~AreaSelect();
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap *pixmap();
private:
  int startx, starty;
  int lastx, lasty;
  bool drawing;
};

#endif // AREASELECT_H

