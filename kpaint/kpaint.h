// $Id$

#ifndef KPAINT_H
#define KPAINT_H

#include <qwidget.h>
#include "QwViewport.h"
#include "ktopwidget.h"
#include "ktoolbar.h"
#include "canvas.h"
#include "manager.h"

class KPaint : public KTopLevelWidget
{
  Q_OBJECT

public:
  KPaint(char *name=0);

public slots:
   void handleCommand(int);
   
private:
  QwViewport *v;
  Canvas *c;
  KToolBar *toolbar;
  Manager *man;
  void initToolbar(void);
  void initMenus();
};

#endif

