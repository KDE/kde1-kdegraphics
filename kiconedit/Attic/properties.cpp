/*
    KDE Icon Editor - a small graphics drawing program for the KDE
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

#include "debug.h"
#include "properties.h"


struct Properties *KIconEditProperties::pprops = 0;
int KIconEditProperties::instances = 0;

KIconEditProperties::KIconEditProperties(QWidget *parent) : QObject()
{
  instances++;
  debug("KIconEditProperties: instances %d", instances);
  if(pprops)
    return;

  debug("KIconEditProperties: reading properties");

  pprops = new struct Properties;

  pprops->recentlist = new QStrList(true);
  CHECK_PTR(pprops->recentlist);
  pprops->recentlist->setAutoDelete(true);

  pprops->keys = new KAccel(parent);

  KConfig *config = kapp->getConfig();

  config->setGroup( "Files" );
  int n = config->readListEntry("RecentOpen", *pprops->recentlist);
  debug("Read %i recent files", n);

  config->setGroup( "Appearance" );

  // restore geometry settings
  QString geom = config->readEntry( "Geometry" );
  if ( !geom.isEmpty() )
    sscanf( geom, "%dx%d", &pprops->winwidth, &pprops->winheight );

  pprops->maintoolbarstat = config->readBoolEntry( "ShowMainToolBar", true );
  pprops->drawtoolbarstat = config->readBoolEntry( "ShowDrawToolBar", true );
  pprops->statusbarstat = config->readBoolEntry( "ShowStatusBar", true );

  pprops->maintoolbarpos = (KToolBar::BarPosition)config->readNumEntry( "MainToolBarPos", KToolBar::Top);
  pprops->drawtoolbarpos = (KToolBar::BarPosition)config->readNumEntry( "DrawToolBarPos", KToolBar::Left);
  //statusbarpos = config->readNumEntry( "StatusBarPos", KStatusBar::Bottom);
  pprops->menubarpos = (KMenuBar::menuPosition)config->readNumEntry( "MenuBarPos", KMenuBar::Top);

  pprops->backgroundmode = (QWidget::BackgroundMode)config->readNumEntry( "BackgroundMode", QWidget::FixedPixmap);
  debug("Reading bgmode: %d", (int)pprops->backgroundmode);
  pprops->backgroundcolor = config->readColorEntry( "BackgroundColor", &gray);
  pprops->backgroundpixmap = config->readEntry("BackgroundPixmap", "");

  debug("BackgroundPixmap: %s", pprops->backgroundpixmap.data());

  config->setGroup( "Grid" );
  pprops->showgrid = config->readBoolEntry( "ShowGrid", true );
  pprops->gridscaling = config->readNumEntry( "GridScaling", 10 );
}

KIconEditProperties::~KIconEditProperties()
{
  instances--;
  debug("KIconEditProperties: instances %d", instances);
  if(instances == 0)
  {
    debug("KIconEditProperties: Deleting properties");
    if(pprops->recentlist)
      delete pprops->recentlist;
    delete pprops;
    debug("KIconEditProperties: Deleted properties");
  }
}

struct Properties *KIconEditProperties::getProperties(QWidget *parent)
{
  KIconEditProperties p(parent);
  return p.pprops;
}

void KIconEditProperties::saveProperties(QWidget *parent)
{
  KIconEditProperties p(parent);
  pprops = p.pprops;
  KConfig *config = kapp->getConfig();

  pprops->keys->writeSettings(config);

  config->setGroup( "Files" );
  debug("Writing %d recent files", pprops->recentlist->count());
  config->writeEntry("RecentOpen", *pprops->recentlist);

  config->setGroup( "Appearance" );
  QString geom;
  geom.sprintf( "%dx%d", pprops->winwidth, pprops->winheight );
  config->writeEntry( "Geometry", geom );

  debug("Writing bgmode: %d", (int)pprops->backgroundmode);
  config->writeEntry("BackgroundMode", pprops->backgroundmode);
  config->writeEntry("BackgroundColor", pprops->backgroundcolor);
  config->writeEntry("BackgroundPixmap", pprops->backgroundpixmap);

  config->writeEntry("ShowMainToolBar", pprops->maintoolbarstat);
  config->writeEntry("ShowDrawToolBar", pprops->drawtoolbarstat);
  config->writeEntry("ShowStatusBar", pprops->statusbarstat);

  config->writeEntry("MainToolBarPos", (int)pprops->maintoolbarpos);
  config->writeEntry("DrawToolBarPos", (int)pprops->drawtoolbarpos);
  config->writeEntry("MenuBarPos", (int)pprops->menubarpos);

  config->setGroup( "Grid" );
  config->writeEntry("ShowGrid", pprops->showgrid);
  config->writeEntry("GridScaling", pprops->gridscaling);
}
