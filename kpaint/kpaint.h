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
  void fileNew();
  void fileOpen();
  void fileSave();
  void fileSaveAs();
  void fileInfo();
  void newWindow();
  void closeWindow();
  void fileQuit();

  // Edit
  void editCopy();
  void editCut();
  void editPaste();
  void editPasteImage();
  void editZoomIn();
  void editZoomOut();
  void editMask();
  void editOptions();
  
  // Image
  void imageInfo();
  void imageEditPalette();
  void imageChangeDepth();

  // Tool
  void setTool(int);
  void toolProperties();

  // Help
  void helpAbout();
  void helpContents();
  void helpIndex();

private:
  int zoom;
  QwViewport *v;
  Canvas *c;
  KToolBar *toolbar;
  Manager *man;
  void initToolbar(void);
  void initMenus();
};

#endif


