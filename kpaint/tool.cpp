// $Id$

#include <kdebug.h>
#include <stdio.h>
#include <assert.h>
#include <qpixmap.h>
#include "tool.h"

Tool::Tool()
{
  active= FALSE;
  canvas= 0;
  props= 0;
  tooltip= 0;
}

int Tool::getPages(void)
{
  return props;
}

void Tool::activate(Canvas *c)
{
  assert(!c->isActive());
  canvas= c;
  active= TRUE;
  activating();
}

void Tool::deactivate()
{
  assert(active);
  deactivating();
  active= FALSE;
  canvas= 0;
  pen= 0;
  brush= 0;
}

bool Tool::isActive()
{
  return active;
}

void Tool::setPen(QPen *p)
{
KDEBUG(KDEBUG_INFO, 3000, "Tool::setPen()\n");
  pen= p;
}

void Tool::setBrush(QBrush *b)
{
KDEBUG(KDEBUG_INFO, 3000, "Tool::setBrush()\n");
  brush= b;
}

QPixmap *Tool::pixmap()
{
  return NULL;
}

#include "tool.moc"
