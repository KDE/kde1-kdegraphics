// $Id$

#ifndef MANAGER_H
#define MANAGER_H

// Manager Class
// This class holds all the tools that can operate on a canvas, it ensures that
// one tool is always active, and responds to signals from the tool palette
// which control which tool is selected. A tool manager is associated with a
// single canvas.

#include <qlist.h>
#include <qobject.h>
#include <qpen.h>
#include <qbrush.h>
#include <qwidget.h>
#include "ktoolbar.h"
#include "canvas.h"
#include "properties.h"
#include "tools/tool.h"


class Manager : public QObject
{
  Q_OBJECT
public:
  Manager(Canvas *c, QWidget *top);
  ~Manager();

  int getCurrentTool(void);
  void showPropertiesDialog(void);
  KToolBar *toolbar(void);

public slots:
  void setCurrentTool(int);
  void updateProperties();

signals:
  void toolChanged(int);
protected:
  void createTools(void);
  void initToolbar(void);

private:
  QList<Tool> list;
  Canvas *canvas;
  QWidget *toplevel;
  propertiesDialog *props;
  KToolBar *mytoolbar;
  int currentTool;
  QPen *p;
  QBrush *b;  
};

#endif // MANAGER_H
