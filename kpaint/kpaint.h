// -*- c++ -*-

// $Id$

#ifndef KPAINT_H
#define KPAINT_H

#include <qwidget.h>
#include "QwViewport.h"
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfm.h>
#include "canvas.h"
#include "manager.h"

class KPaint : public KTopLevelWidget
{
  Q_OBJECT

public:
  KPaint(const char *url_= NULL);

  // File
  void fileNew();
  void fileOpen();
  void fileSave();
  void fileSaveAs();
  void fileFormat();
  void fileOpenURL();
  void fileSaveAsURL();
  void newWindow();
  void closeWindow();
  void fileExit();

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

public slots:
  void handleCommand(int command);

protected:
  // Load file
  bool loadLocal(const char *filename_, const char *url_= NULL);
  bool loadRemote(const char *url_= NULL);
  bool saveRemote(const char *url_);
  bool saveLocal(const char *filename_, const char *url_= NULL);
  void KFMgetFinished();
  void KFMputFinished();
  int exit();
  KFM *kfm;
  enum transferDirection { KfmNone, KfmGet, KfmPut };

protected slots:
  void KFMfinished();

private:
  // actual local filename
  QString filename;
  // URL it came from or empty if local
  QString url;
  transferDirection kfmOp;
  QString tempURL;
  QString format;
  int zoom;
  void closeEvent(QCloseEvent *e);
  bool modified;

  QwViewport *v;
  Canvas *c;
  KToolBar *toolbar;
  KStatusBar *statusbar;
  Manager *man;
  void initToolbar(void);
  void initMenus(void);
  void initStatus(void);
};

#endif


