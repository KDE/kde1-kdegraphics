// $Id$

#ifndef SPRAYCAN_H
#define SPRAYCAN_H

#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>
#include "tool.h"

class SprayCan : public Tool
{
  Q_OBJECT
public:
  SprayCan(void);
  ~SprayCan();
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating(void);
  QPixmap *pixmap(void);
private:
  int density; // Dots per second (approx)
  QTimer *timer;
  int x, y;
  int brushsize;
  bool drawing;
private slots:
  void drawDot();
};


#endif // SPRAYCAN_H
