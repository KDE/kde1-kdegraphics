#ifndef _MANAGER_H
#define _MANAGER_H

#include <kfm.h>
#include <qslider.h>

#include <kapp.h>


#include <qwidget.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpopmenu.h>
#include <qcombo.h>
#include <qstring.h>

#include <ktopwidget.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kmenubar.h>

#include "wview.h"

//IDs for Statusbar entries
#define ID_S_IMAGE  1
#define ID_S_FILES  2

//IDs for toolbar entries
#define ID_T_NEWWINDOW  10
#define ID_T_OPENFILE   11
#define ID_T_FIRSTPIC   12
#define ID_T_PREVPIC    13
#define ID_T_NEXTPIC    14
#define ID_T_LASTPIC    15
#define ID_T_STARTSHOW  16
#define ID_T_RANDOMSHOW 17
#define ID_T_STOPSHOW   18
#define ID_T_MAGNIFY    19
#define ID_T_MINIFY     20
#define ID_T_RELOAD     21
#define ID_T_RESIZE     22
#define ID_T_HELP       23

//class WView;

class Fileman : public KTopLevelWidget
{
    Q_OBJECT;
public:
  Fileman( const char *name=0, WView * myparent=0 );
 Fileman();
  static QString shortenFilename(QString);
  static void appendFileList(QString);

  //static QStrList fileList;
  
public slots:

  // menubar action
  void aboutKview();
  void saveOptions();

  // toolbar1 action
  void slotNew();
  void slotOpen();
  void slotOpenUrl();
  void slotReload();
  void slotMax();
  void slotMin();
  void slotResize();
  void invokeHelp();

  // toolbar2 action
  void firstClicked();
  void prevClicked();
  void nextClicked();
  void lastClicked();
  void startShow();
  void randomShow();
  void stopShow();


  
  void slotLineEdit();
  void closeWindow();

  void listboxClicked(int);
  void comboboxSelected(int); 
  void sliderChanged(int);
  void updateListbox(int);
  void updateCombobox();

  /// Drag and Drop - called when drop event occurs
  void slotDropEvent( KDNDDropZone * _dropZone );


protected:
  void closeEvent( QCloseEvent *);
  void resizeEvent(QResizeEvent *);
  void timerEvent( QTimerEvent *);
  void initMenuBar();
  void initToolBar();
  void initStatusBar();
  void initMainWidget();
  void checkValidButtons();
  
  
private:
  //Window elements
  QWidget     *mainwidget;
  QListBox    *listbox;
  QComboBox   *combobox;
  QSlider     *slider;
  QLineEdit   *lineEdit;
  QLabel      *textSlow, *textFast, *textOutput;
  KStatusBar  *statusbar;
  KToolBar    *ktoolbar1,*ktoolbar2;
  int         iktoolbar1,iktoolbar2;
  KMenuBar    *menubar;
  QPopupMenu  *file,*options, *help;
  
  
  bool    showrunning;
  bool    showrandom;
  int     timerDelay;
  int     timerID;
  WView   *imageWindow;

  QString lastPath;

  //keep track of all child display windows
  //no function, for future use already
  //QList<WView> windowList;
  //int winCount;

  static QList<Fileman> manList;
  static int            manCount;

  void createWView();
  
private slots:
  void doPositioning();
  void WViewCloseClicked(int);

};

#endif
