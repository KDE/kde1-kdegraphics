// Line class
// This class is a subclass of Tool and provides a rubber line drawing tool.

// $Id$

#ifndef LINE_H
#define LINE_H

#include <qpixmap.h>
#include "tool.h"

class Line : public Tool
{
public:
  Line(void);
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

#endif // LINE_H

