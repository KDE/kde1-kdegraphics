/*
    KDE Icon Editor - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef __KICONEDITGRID_H__
#define __KICONEDITGRID_H__

#include <qdir.h>
#include <qdrawutl.h>
#include <qbrush.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qbuffer.h>
#include <qclipbrd.h>
#include <qfileinf.h>
#if QT_VERSION >= 140
#include <qgroupbox.h>
#else
#include <qgrpbox.h>
#endif 
#include <qbttngrp.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qpntarry.h>
#include <qdialog.h>
#include <qstrlist.h>
#include <qtimer.h>
#include <kcolordlg.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kintegerline.h>
#include <kpixmap.h>
#include "utils.h"
#include "kcolorgrid.h"
#include "kresize.h"
#include "properties.h"

class KRuler;
class KIconEditGrid;

enum Direction { In = 0, Out = 1, Up = In, Down = Out, Left, Right };

class KGridView : public QFrame
{
    Q_OBJECT
public:
  KGridView( QWidget * parent = 0, const char *name = 0);

  KRuler *hruler() { return _hruler;}
  KRuler *vruler() { return _vruler;}
  QFrame *corner() { return _corner;}
  KIconEditGrid *grid() { return _grid; }
  void setShowRulers(bool mode);
  bool rulers() { return pprops->showrulers; }

public slots:
  void sizeChange(int, int);
  void scalingChange(int, bool);

protected:
  void resizeEvent(QResizeEvent*);
  void setSizes();

  QFrame *_corner;
  KIconEditGrid *_grid;
  KRuler *_hruler, *_vruler;
  Properties *pprops;
};

/**
* KIconEditGrid
* @short KIconEditGrid
* @author Thomas Tanghus <tanghus@earthling.net>
* @version 0.3
*/
class KIconEditGrid : public KColorGrid
{
    Q_OBJECT
public:
  KIconEditGrid( QWidget * parent = 0, const char *name = 0);
  virtual ~KIconEditGrid();

  enum DrawTool { Line, Freehand, FloodFill, Spray, Rect, FilledRect, Circle,
        FilledCircle, Ellipse, FilledEllipse, Eraser, SelectRect, SelectCircle, Find };
  enum DrawAction { Mark, UnMark, Draw };

  bool isModified() { return modified; };
  void setModified(bool m) { modified = m; }
  const QPixmap &pixmap();
  const QImage &image() { return *img; }
  const QImage *clipboardImage(bool &ok);
  QImage *getSelection(bool);
  virtual QSize sizeHint();
  int rows() { return numRows(); };
  int cols() { return numCols(); };
  uint getColors( uint *_colors) { return colors(_colors); }
  bool isMarked(QPoint p);
  bool isMarked(int x, int y);
  int scaling() { return cellSize(); }

public slots:
  void loadBlank( int w = 0, int h = 0);
  void load( QImage *);
  void editCopy(bool cut = false);
  void editPaste(bool paste = false);
  void editPasteAsNew();
  void editSelectAll();
  void editClear();
//#if QT_VERSION <= 140
  void editResize();
//#endif
  void setSize(const QSize s);
  void grayScale();
  void mapToKDEPalette();
  void setTool(DrawTool tool);
  bool zoom(Direction direct);
  bool zoomTo(int);

signals:
  void scalingchanged(int, bool);
  void changed( const QPixmap & );
  void sizechanged( int, int );
  void poschanged( int, int );
  void xposchanged( int );
  void yposchanged( int );
  void colorschanged( uint, uint* );
  void addingcolor(uint);
  void newmessage(const char *);
  void clipboarddata(bool);
  void selecteddata(bool);

protected slots:
  void setColorSelection( uint );
  void checkClipboard();

protected:
  virtual void paintCell( QPainter*, int, int );
  //virtual void resizeEvent(QResizeEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  void setColor(int colNum, uint, bool update = true );
  void createCursors();
  void drawPointArray(QPointArray, DrawAction);
  void drawEllipse(bool);
  void drawLine(bool);
  void drawRect(bool);
  void drawSpray(QPoint);
  void drawFlood(int x, int y, uint oldcolor);

  uint currentcolor;
  QPoint start, end;
  QRect insrect;
  QSize cbsize;
  QImage *img;
  QPixmap p;
  int selected, tool; //, numrows, numcols;
  bool modified, btndown, ispasting, isselecting;
  QPointArray pntarray;
  KColorArray iconcolors;
  QCursor cursor_normal, cursor_aim, cursor_flood, cursor_spray, cursor_erase, cursor_paint;
};



#endif //__KICONEDITGRID_H__



