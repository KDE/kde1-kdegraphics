// $Id$

#include <stdio.h>
#include <assert.h>
#include "tool.h"

Tool::Tool()
{
  active= FALSE;
  canvas= 0;
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
#ifdef KPDEBUG
  fprintf(stderr, "Tool::setPen()\n");
#endif
  pen= p;
}

void Tool::setBrush(QBrush *b)
{
#ifdef KPDEBUG
  fprintf(stderr, "Tool::setBrush()\n");
#endif
  brush= b;
}
