// $Id$

#ifndef CANVAS_H
#define CANVAS_H

#include <qwidget.h>
#include <qevent.h>
#include <qpixmap.h>

class Tool;

class Canvas : public QWidget
{
  Q_OBJECT

public:
  Canvas(const char *myname, int width, int height,
	 QWidget *parent= 0, const char *name=0);
  Canvas(const char *file, QWidget *parent= 0, const char *name=0);
  void paintEvent(QPaintEvent *e);

  // The following methods can be called when active or inactive
  QPixmap *pixmap(void);
  // backgroundColour()
  // foregroundColour()
  bool load(const char *filename, char *format= 0);
  bool save(const char *filename, char *format= 0);
  const char *filename(void);
   void setFilename(const char *filename);
   char *format();
  bool isActive();
  
  // This controls which tool the events go to (if any)
  void activate(Tool *tool);
  void deactivate();

  // Event redirectors
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  /**
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  */
protected:
  enum state {
    ACTIVE,
    INACTIVE
  };

  Tool *currentTool;
  QPixmap *pix;
  state s;
  QString *imageName;

};

#endif // CANVAS_H


