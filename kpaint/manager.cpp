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
#include "roundangle.h"
#include "spraycan.h"
#include "areaselect.h"

Manager::Manager(Canvas *c) : QObject()
{
  assert(c != 0);
  assert(!(c->isActive()));

  KDEBUG(KDEBUG_INFO, 3000, "Manager:: Constructing...\n");

  canvas= c;
  createTools();
  currentTool= 0;
  list.first()->activate(c);

  lmbCol= red;
  rmbCol= green;

  // Initialise the pens and brushes
  p.setColor(lmbCol);
  b.setColor(lmbCol);
  list.first()->setLeftPen(p);
  list.first()->setLeftBrush(b);
  p.setColor(rmbCol);
  b.setColor(rmbCol);
  list.first()->setRightPen(p);
  list.first()->setRightBrush(b);

  c->activate(list.first());
  connect(this, SIGNAL(modified()), c, SLOT(markModified()));
}

Manager::~Manager()
{
}

void Manager::createTools()
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
  list.append(new Roundangle);

  // Create the properties dialog
  props= new propertiesDialog(1, 0, i18n("Tool Properties"));
  connect(props, SIGNAL(applyButtonPressed()),
	  this, SLOT(updateProperties()) );
}

void Manager::updateProperties()
{
  p= props->getPen();
  b= props->getBrush();

  p.setColor(lmbCol);
  b.setColor(lmbCol);
  list.at(currentTool)->setLeftPen(p);
  list.at(currentTool)->setLeftBrush(b);
  p.setColor(rmbCol);
  b.setColor(rmbCol);
  list.at(currentTool)->setRightPen(p);
  list.at(currentTool)->setRightBrush(b);
}

void Manager::setLMBcolour(const QColor &c)
{
warning("setLMBcolour\n");
  lmbCol= c;
  updateProperties();
}

void Manager::setRMBcolour(const QColor &c)
{
warning("setRMBcolour\n");
  rmbCol= c;
  updateProperties();
}

const QColor &Manager::lmbColour()
{
  return lmbCol;
}

const QColor &Manager::rmbColour()
{
  return rmbCol;
}

int Manager::getCurrentTool()
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

    p.setColor(lmbCol);
    b.setColor(lmbCol);
    list.at(tool)->setLeftPen(p);
    list.at(tool)->setLeftBrush(b);
    p.setColor(rmbCol);
    b.setColor(rmbCol);
    list.at(tool)->setRightPen(p);
    list.at(tool)->setRightBrush(b);

    canvas->activate(list.at(tool));
    props->setPages(list.at(tool)->getPages());
    props->repaint(0);
    emit toolChanged(tool);
  }
}


void Manager::showPropertiesDialog()
{
  props->show();
}

void Manager::populateToolbar(KToolBar *t)
{
  Tool *tool;
  QPixmap *pix;
  int i;
  const char *tip;
  for (i= 0, tool= list.first(); tool != 0; i++, tool= list.next()) {
    tip= tool->tip();
    pix= tool->pixmap();
    t->insertButton(*pix, i, TRUE, tip, i);
    connect(tool, SIGNAL(modified()), this, SIGNAL(modified()));
  }
  connect(t, SIGNAL(clicked(int)), SLOT(setCurrentTool(int)));
}

#include "manager.moc"

