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
#include <qrect.h>
#include "canvas.h"
#include "manager.h"

class KPaint : public KTopLevelWidget
{
  Q_OBJECT

public:
  KPaint(const char *url_= NULL);
  ~KPaint();

  void setPixmap(QPixmap *);

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
  void toolProperties();

  // Help
  void helpAbout();
  void helpContents();
  void helpIndex();

public slots:
  void setTool(int);
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

// Define generic command codes
#define ID_OPEN 100
#define ID_NEW 101
#define ID_SAVE 102
#define ID_SAVEAS 103
#define ID_CLOSE 104
#define ID_NEWWINDOW 105
#define ID_CLOSEWINDOW 106
#define ID_COPY 107
#define ID_CUT 108
#define ID_PASTE 109
#define ID_OPTIONS 110
#define ID_EXIT 111
#define ID_HELPCONTENTS 112
#define ID_ABOUT 113
#define ID_OPENURL 114
#define ID_SAVEURL 115
#define ID_SHOWTOOLBAR 116
#define ID_SHOWMENUBAR 117
#define ID_SHOWSTATUSBAR 118
#define ID_SAVEOPTIONS 119

// Define app specific command codes
#define ID_FORMAT 200
#define ID_PASTEIMAGE 201
#define ID_ZOOMIN 202
#define ID_ZOOMOUT 203
#define ID_MASK 204
#define ID_INFO 205
#define ID_PALETTE 206
#define ID_DEPTH 207
#define ID_RELEASENOTES 208

// Tags for statusbar items
#define ID_FILESIZE 300
#define ID_ZOOMFACTOR 301
#define ID_FILENAME 302

// Tags for toolbars
#define ID_FILETOOLBAR 0
#define ID_TOOLTOOLBAR 1

#endif


