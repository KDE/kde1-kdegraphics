// -*- c++ -*-

// $Id$

#ifndef KPAINT_H
#define KPAINT_H

#include <qwidget.h>
#include "QwViewport.h"
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kfm.h>
#include <qrect.h>
#include "canvas.h"
#include "manager.h"

class KPaint : public KTopLevelWidget
{
  Q_OBJECT

public:
  KPaint(const char *url_= 0);
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
  // Load/Save files
  bool loadLocal(const char *filename_, const char *url_= 0);
  bool loadRemote(const char *url_= 0);
  bool saveRemote(const char *url_);
  bool saveLocal(const char *filename_, const char *url_= 0);

  // remote file results
  void KFMgetFinished();
  void KFMputFinished();

  // Should we really?
  int exit();

  enum transferDirection { KfmNone, KfmGet, KfmPut };

protected slots:
  void KFMfinished();

private:
  // Initialisation
  void initToolbars();
  void initMenus();
  void initStatus();

  // Catch close events
  void closeEvent(QCloseEvent *);

  // Update status item
  void canvasSizeChanged();

  // Read/write the options
  void readOptions();
  void writeOptions();

  // Update toolbars/status bars etc.
  void updateControls();
  void updateCommands();

  // Info about the currently open image
  QString filename; // actual local filename
  QString url; // URL it came from or empty if local
  QString tempURL;
  QString format;
  int zoom;
  bool modified;

  // UI configuation
  bool showStatusBar;
  bool showCommandsToolBar;
  bool showToolsToolBar;

  // Command status
  bool allowEditPalette;

  // Kfm ipc stuff
  KFM *kfm;
  transferDirection kfmOp;

  // Tool manager
  Manager *man;

  // Child widgets
  QwViewport *v;
  Canvas *c;
  KToolBar *toolsToolbar;
  KToolBar *commandsToolbar;
  KStatusBar *statusbar;
  KMenuBar *menu;
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
#define ID_SHOWTOOLSTOOLBAR 209
#define ID_SHOWCOMMANDSTOOLBAR 210

// Tags for statusbar items
#define ID_FILESIZE 300
#define ID_ZOOMFACTOR 301
#define ID_FILENAME 302

// Tags for toolbars
#define ID_COMMANDSTOOLBAR 0
#define ID_TOOLSTOOLBAR 1

#endif


