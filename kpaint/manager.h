// $Id$

#ifndef MANAGER_H
#define MANAGER_H

#include <qlist.h>
#include <qobject.h>
#include <qpen.h>
#include <qbrush.h>
#include <qwidget.h>
#include "ktoolbar.h"
#include "canvas.h"
#include "properties.h"
#include "tool.h"

/**
 * @short Manager Class
 * This class holds all the tools that can operate on a canvas, it
 * ensures that one tool is always active, and responds to signals
 * from the tool palette which control which tool is selected. A
 * tool manager is associated with a single canvas.
 */
class Manager : public QObject
{
  Q_OBJECT
public:
  Manager(Canvas *c);
  ~Manager();

  int getCurrentTool();
  void showPropertiesDialog();
  void populateToolbar(KToolBar *);
  const QColor &lmbColour();
  const QColor &rmbColour();

public slots:
  void setCurrentTool(int);
  void updateProperties();
  void setLMBcolour(const QColor &);
  void setRMBcolour(const QColor &);

signals:
  void toolChanged(int);
  void modified();

protected:
  void createTools();

private:
  QList<Tool> list;
  Canvas *canvas;
  propertiesDialog *props;
  int currentTool;

  QPen p;
  QBrush b;
  QColor lmbCol;
  QColor rmbCol;
};

#endif // MANAGER_H
