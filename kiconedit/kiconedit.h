/*
    kiconedit - a small graphics drawing program for creating KDE icons
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef __KICONEDIT_H__
#define __KICONEDIT_H__

#include <qdir.h>
#include <qprinter.h>
#include <kapp.h>
#include <ktopwidget.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kmsgbox.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kwm.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <kruler.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qfileinf.h> 
#include <qlist.h>
#include <qkeycode.h>
#include <qtimer.h>
#include <qscrollview.h>
#include "kicontools.h"
#include "knew.h"
#include "kicon.h"
#include "kiconconfig.h"
#include "kicongrid.h"
#include "kresize.h"
#include "properties.h"

#define ID_FILE_NEWWIN  500
#define ID_FILE_NEWFILE 501
#define ID_FILE_OPEN    502
#define ID_FILE_RECENT  503
#define ID_FILE_SAVE    504
#define ID_FILE_SAVEAS  505
#define ID_FILE_PRINT   506
#define ID_FILE_CLOSE   507
#define ID_FILE_QUIT    508

#define ID_EDIT_CUT          510
#define ID_EDIT_COPY         511
#define ID_EDIT_PASTE        512
#define ID_EDIT_PASTE_AS_NEW 513
#define ID_EDIT_CLEAR        514
#define ID_EDIT_SELECT_ALL   515
		
#define ID_VIEW_ZOOM_IN      520
#define ID_VIEW_ZOOM_OUT     521
#define ID_VIEW_ZOOM_1TO1    522
#define ID_VIEW_ZOOM_1TO5    523
#define ID_VIEW_ZOOM_1TO10   524

#define ID_DRAW_FREEHAND     530
#define ID_DRAW_FILL         531
#define ID_DRAW_RECT         532
#define ID_DRAW_CIRCLE       533
#define ID_DRAW_ELLIPSE      534
#define ID_DRAW_SPRAY        535
#define ID_DRAW_RECT_FILL    536
#define ID_DRAW_CIRCLE_FILL  537
#define ID_DRAW_ELLIPSE_FILL 538
#define ID_DRAW_LINE         539
#define ID_DRAW_FIND         540
#define ID_DRAW_ERASE        541

#define ID_OPTIONS_TOGGLE_GRID  560
#define ID_OPTIONS_TOGGLE_TOOL1 561
#define ID_OPTIONS_TOGGLE_TOOL2 562
#define ID_OPTIONS_TOGGLE_STATS 563
#define ID_OPTIONS_CONFIGURE    564
#define ID_OPTIONS_SAVE         565

#define ID_IMAGE_RESIZE         570
#define ID_IMAGE_GRAYSCALE      571
#define ID_IMAGE_MAPTOKDE       572

#define ID_SELECT_RECT          580
#define ID_SELECT_CIRCLE        581

/**
* KIconEdit
* @short KIconEdit
* @author Thomas Tanghus <tanghus@kde.org>
* @version 0.4
*/
class KIconEdit : public KTopLevelWidget
{
    Q_OBJECT
public:
  KIconEdit( const char *name = "kiconedit", const char *xpm = 0);
  ~KIconEdit();

  virtual QSize sizeHint();

signals:

public slots:
  virtual void saveProperties(KConfig*);
  virtual void readProperties(KConfig*);
  void saveGoingDownStatus();
  void unsaved(bool flag);

protected slots:
  void slotNewWin();
  void slotNewWin(const char *url = 0);
  void slotNew();
  void slotOpen();
  void slotClose();
  void slotQuit();
  void slotSave();
  void slotSaveAs();
  void slotPrint();
  void slotView(int);
  void slotCopy();
  void slotCut();
  void slotPaste();
  void slotClear();
  void slotImage(int id);
  void slotSaved();
  void slotSelectAll();
  void slotOpenRecent(int id);
  void slotConfigure(int);
  void slotTools(int);

  void slotUpdateStatusColors(uint);
  void slotUpdateStatusColors(uint, uint*);
  void slotUpdateStatusPos(int, int);
  void slotUpdateStatusSize(int, int);
  void slotClearStatusMessage();
  void slotUpdateStatusMessage(const char *);
  void slotUpdateStatusName(const char *);
  void slotUpdateStatusScaling(int, bool);
  void slotUpdatePaste(bool);
  void slotUpdateCopy(bool);
  void slotOpenBlank(const QSize);
  void slotDropEvent( KDNDDropZone * );

protected:
  KMenuBar *setupMenuBar();
  KToolBar *setupToolBar();
  KToolBar *setupDrawToolBar();
  KStatusBar *setupStatusBar();
  virtual void closeEvent(QCloseEvent*);
  virtual void resizeEvent(QResizeEvent*);
  void writeConfig();
  void toggleTool(int id);
  void addRecent(const char *file);
  void updateMenuAccel();

#ifdef KWIZARD_VERSION
  KIconTemplateContainer *icontemplates;
#endif

private:
  KMenuBar *menubar;
  KToolBar *toolbar, *drawtoolbar;
  KStatusBar *statusbar;
  QPopupMenu *edit, *file, *view, *image, *help, *tools, *options, *zoom, *recent, *select;
  KIconEditGrid *grid;
  KGridView *gridview;
  KIconToolsView *toolsw;
  QWidget *mainview;
  QBoxLayout *l;
  QTimer *msgtimer;
  QScrollView *viewport;
  KIcon *icon;
  KDNDDropZone *dropzone;
  KIconEditProperties *kiprops;
};



#endif //__KICONEDIT_H__



