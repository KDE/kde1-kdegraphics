/*  
    kiconedit - a small graphics drawing program the KDE.

    Copyright (C) 1998 Thomas Tanghus (tanghus@earthling.net)

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

#ifndef __PROPS_H__
#define __PROPS_H__

#include <qlist.h>
#include <qwidget.h>
#include <qimage.h>
#include <kaccel.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include "knew.h"

#define props(x) KIconEditProperties::getProperties(x)

enum BackgroundMode { PixMap, Color };

struct Properties
{
  KAccel *keys;
  QDict<KKeyEntry> keydict;
  QList<KIconTemplate> *tlist;
  QStrList *recentlist;
  QString BackgroundPixmap;
  QColor BackgroundColor;
  BackgroundMode bgmode;
  bool maintoolbarstat;
  bool drawtoolbarstat;
  bool statusbarstat;
  bool showgrid;
  KToolBar::BarPosition maintoolbarpos;
  KToolBar::BarPosition drawtoolbarpos;
  KMenuBar::menuPosition menubarpos;
  int winwidth;
  int winheight;
  int gridscaling;
};

class KIconEditProperties : public QObject
{
  Q_OBJECT
public:
  KIconEditProperties(QWidget *);
  ~KIconEditProperties();

  /**
  * The QList must not be deleted! Delete the object instead as it automatically
  * deletes the list when there are no more instances.
  */
  static struct Properties *getProperties(QWidget*);

protected:
  static struct Properties *pprops;
  static int instances;
};


#endif //__PROPS_H__



