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

  QPixmap *pixmap();
  void setPixmap(QPixmap *);
  void setDepth(int);

  const QRect &selection();
  void setSelection(const QRect&);
  void clearSelection();
  QPixmap *selectionData();

  QPixmap *zoomedPixmap();
  void setZoom(int);
  int zoom();
  void updateZoomed();
  void resizeImage(int, int);

  bool load(const char *filename= 0, const char *format= 0);
  bool save(const char *filename= 0, const char *format= 0);

  bool isActive();
  bool isModified();
  void clearModified();

  // This controls which tool the events go to (if any)
  void activate(Tool *tool);
  void deactivate();

  // Handle paint events
  void paintEvent(QPaintEvent *e);

  // Redirect events tools can use
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

public slots:
  void markModified();

signals:
  void sizeChanged();
  void pixmapChanged(QPixmap *);
  void modified();

protected:
  enum state {
    ACTIVE,
    INACTIVE
  };

  QWMatrix *matrix;
  Tool *currentTool;

  /**
   * Zoomed copy
   */
  QPixmap *zoomed;

  /**
   * Master copy
   */
  QPixmap *pix;

  /**
   * % of original size
   */
  int zoomFactor;

  /**
   * Has the pixmap been modified?
   */
  bool modified_;
  state s;
  QRect selection_;
  bool haveSelection_;
};

#endif // CANVAS_H


