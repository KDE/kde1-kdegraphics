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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <qlist.h>
#include <qwidget.h>
#include <qimage.h>


#define ABOUTSTR i18n("KDE Icon Editor\nWritten by Thomas Tanghus <tanghus@earthling.net>\n")
#define UNNAMED  i18n("UnNamed.xpm")

/** Global list of top level windows. */
typedef QList<QWidget> WindowList;
extern WindowList* windowList;

#endif //__MAIN_H__



