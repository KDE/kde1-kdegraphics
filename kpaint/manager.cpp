// $Id$

#include <kdebug.h>
#include <qobject.h>
#include <stdio.h>
#include <assert.h>
#include <qlist.h>
#include <qpixmap.h>
#include <klocale.h>
#include "manager.h"
#include "app.h"

#include "pen.h"
#include "line.h"
#include "ellipse.h"
#include "circle.h"
#include "rectangle.h"
#include "spraycan.h"
#include "areaselect.h"

Manager::Manager(Canvas *c, QWidget *top) : QObject()
{
  assert(c != NULL);
  assert(top != NULL);
  assert(!(c->isActive()));

KDEBUG(KDEBUG_INFO, 3000, "Manager:: Constructing...\n");

  canvas= c;
  toplevel= top;
  createTools();
  initToolbar();
  currentTool= 0;
  p= new QPen(blue);
  b= new QBrush(NoBrush);
  list.first()->activate(c);
  list.first()->setPen(p);
  list.first()->setBrush(b);
  c->activate(list.first());
}

Manager::~Manager()
{
  delete b;
  delete p;
}

void Manager::createTools(void)
{
  // Create the Tools
  list.setAutoDelete(TRUE);
  list.append(new Ellipse);
  list.append(new Circle);
  list.append(new Pen);
  list.append(new Rectangle);
  list.append(new Line);
  list.append(new SprayCan);
  list.append(new AreaSelect);

  // Create the properties dialog
  props= new propertiesDialog(1, 0, klocale->translate("Tool Properties"));
  connect(props, SIGNAL(applyButtonPressed()),
	  this, SLOT(updateProperties()) );
}

void Manager::updateProperties(void)
{
  QPen *tp;
  QBrush *tb;
  tp= p; tb= b;
  p= props->getPen();
  assert(p);
  b= props->getBrush();
  assert(b);
  list.at(currentTool)->setPen(p);
  list.at(currentTool)->setBrush(b);
  delete tp;
  delete tb;
}

int Manager::getCurrentTool(void)
{
  return currentTool;
}

void Manager::setCurrentTool(int tool)
{
KDEBUG1(KDEBUG_INFO, 3000, "got setCurrentTool %d\n", tool);

  if (currentTool != tool) {
    canvas->deactivate();
    list.at(currentTool)->deactivate();
    currentTool= tool;
    list.at(tool)->activate(canvas);
    list.at(tool)->setPen(p);
    list.at(tool)->setBrush(b);
    canvas->activate(list.at(tool));
    props->setPages(list.at(tool)->getPages());
    props->repaint(0);
    emit toolChanged(tool);
  }
}


void Manager::showPropertiesDialog(void)
{
  props->show();
}

KToolBar *Manager::toolbar()
{
  return mytoolbar;
}


void Manager::initToolbar()
{
  mytoolbar= new KToolBar(toplevel);
  Tool *tool;
  QPixmap *pix;
  int i;
  const char *tip;
  for (i= 0, tool= list.first(); tool != NULL; i++, tool= list.next()) {
    tip= tool->tip();
    pix= tool->pixmap();
KDEBUG2(KDEBUG_INFO, 3000, "Adding Pix: %p  // %s\n", pix, tip);
KDEBUG2(KDEBUG_INFO, 3000, "Pix Size: %d, %d", pix->width(), pix->height());
    mytoolbar->insertButton(*pix, i, TRUE, tip, i);
  }
  connect(mytoolbar, SIGNAL(clicked(int)), SLOT(setCurrentTool(int)));
}

#include "manager.moc"

