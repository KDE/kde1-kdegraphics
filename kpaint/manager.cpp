// $Id$

#include <qobject.h>
#include <stdio.h>
#include <assert.h>
#include <qlist.h>
#include "manager.h"

#include "tools/pen.h"
#include "tools/line.h"
#include "tools/ellipse.h"
#include "tools/circle.h"
#include "tools/rectangle.h"

Manager::Manager(Canvas *c) : QObject()
{
  assert(!(c->isActive()));
  assert(c != NULL);

#ifdef KPDEBUG
  fprintf(stderr, "Manager:: Constructing...\n");
#endif

  canvas= c;
  createTools();
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
   
  // Create the properties dialog
  props= new propertiesDialog(0, "Tool Properties");
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
#ifdef KPDEBUG
  fprintf(stderr, "got setCurrentTool %d\n", tool);
#endif

  if (currentTool != tool) {
    canvas->deactivate();
    list.at(currentTool)->deactivate();
    currentTool= tool;
    list.at(tool)->activate(canvas);
    list.at(tool)->setPen(p);
    list.at(tool)->setBrush(b);
    canvas->activate(list.at(tool));
    emit toolChanged(tool);
  }
}


void Manager::showPropertiesDialog(void)
{
  props->show();
#ifndef NEWQT
  // Fix for Qt 1.1 not needed for Qt >= 1.2
  props->repaint(0);
#endif
}


#include "manager.moc"

