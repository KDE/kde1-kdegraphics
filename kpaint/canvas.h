// -*- c++ -*-

// $Id$

#ifndef CANVAS_H
#define CANVAS_H

#include <qwidget.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qwmatrix.h>

class Tool;

class Canvas : public QWidget
{
  Q_OBJECT

public:
  Canvas(int width, int height,
	 QWidget *parent= 0, const char *name=0);
  Canvas(const char *filename, QWidget *parent= 0, const char *name=0);
  void paintEvent(QPaintEvent *e);

  QPixmap *pixmap(void);
  void setPixmap(QPixmap *);

  const QRect &selection();
  void setSelection(const QRect&);
  void clearSelection();
  QPixmap *selectionData();

  QPixmap *zoomedPixmap(void);
  void setZoom(int);
  int zoom(void);
  void updateZoomed(void);

  bool load(const char *filename= 0, const char *format= 0);
  bool save(const char *filename= 0, const char *format= 0);

  bool isActive(void);

  // This controls which tool the events go to (if any)
  void activate(Tool *tool);
  void deactivate();

  // Event redirectors
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

signals:
  void sizeChanged();

protected:
  enum state {
    ACTIVE,
    INACTIVE
  };

  QWMatrix *matrix;
  Tool *currentTool;
  QPixmap *zoomed; // Zoomed copy
  QPixmap *pix; // Master copy
  int zoomFactor; // % of original size
  bool modified; // Has the pixmap been modified?
  state s;
  QRect selection_;
  bool haveSelection_;
};

#endif // CANVAS_H


