/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

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

#include "debug.h"
#include "kicontools.h"

Preview::Preview(QWidget *parent) : QTableView(parent)
{
  ppixmap = 0L;
  pw = ph = 0;
  setTableFlags(Tbl_autoScrollBars);
  setAutoUpdate(true);
  setNumRows(1);
  setNumCols(1);
}

Preview::~Preview()
{
  if(ppixmap)
    delete ppixmap;
}

void Preview::setPixmap( const QPixmap & pixmap)
{
  if(!ppixmap)
    ppixmap = new QPixmap;

  *ppixmap = pixmap;

  setCellWidth(ppixmap->width());
  setCellHeight(ppixmap->height());

  updateTableSize();
  repaint(viewRect());
}

void Preview::paintCell( QPainter *paint, int, int)
{
  if(!ppixmap)
    return;

  paint->drawPixmap( 0, 0, *ppixmap );
}

KIconToolsView::KIconToolsView(QWidget *parent, const char *name)
 : QFrame(parent, name)
{
  debug("KIconTools - constructor");
  initMetaObject();
  setFrameStyle(QFrame::Panel|QFrame::Raised);
  preview = new Preview(this);
  preview->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  
  syslabel = new QLabel(i18n("System Colors"), this);
  debug("KIconTools - before syscolors");
  syscolors = new KSysColors(this);
  debug("KIconTools - after syscolors");
  syscolors->adjustSize();
  connect( syscolors, SIGNAL( newcolor(uint)), SLOT(newColor(uint)));

  customlabel = new QLabel(i18n("Custom Colors"), this);
  debug("KIconTools - before customcolors");
  customcolors = new KCustomColors(this);
  debug("KIconTools - after customcolors");
  customcolors->adjustSize();
  connect( customcolors, SIGNAL( newcolor(uint)), SLOT(newColor(uint)));

  preview->setFixedSize(syscolors->width(), 60);

  setFixedWidth(sizeHint().width());
  debug("KIconTools - constructor");
}

KIconToolsView::~KIconToolsView()
{
  debug("KIconTools - destructor");
  delete syslabel;
  delete customlabel;
  delete preview;
  delete syscolors;
  delete customcolors;
  debug("KIconTools - destructor: done");
}

QSize KIconToolsView::sizeHint()
{
  int w = preview->width();
  if(syslabel->width() > w)
    w = syslabel->width();
  if(syscolors->width() > w)
    w = syscolors->width();
  if(customlabel->width() > w)
    w = customlabel->width();
  if(customcolors->width() > w)
    w = customcolors->width();
  w += 10;
  int h = 110 + preview->height() + syscolors->height();
  h += customcolors->height() + syslabel->height() + customlabel->height();

  return QSize(w, h);
}

void KIconToolsView::resizeEvent(QResizeEvent*)
{
  int y = 5;
  preview->move(5, y);
  y += preview->height() + 5;
  syslabel->move(5, y);
  y += syslabel->height();
  syscolors->move(5, y);
  y += syscolors->height() + 5;
  customlabel->move(5, y);
  y += customlabel->height();
  customcolors->move(5, y);
}

void KIconToolsView::newColor(uint color)
{
  emit newcolor(color);
}

void KIconToolsView::addColors(uint n, uint *c)
{
  //debug("KIconTools::addColors()");
  customcolors->clear();
  for(uint i = 0; i < n; i++)
    addColor(c[i]);
}

void KIconToolsView::addColor(uint color)
{
  //debug("KIconTools::addColor()");
  if(!syscolors->contains(color))
    customcolors->addColor(color);
}

void KIconToolsView::setPreview( const QPixmap &pixmap )
{
  preview->setPixmap( pixmap );
}


