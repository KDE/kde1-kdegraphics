#ifndef _KWidget_H
#define _KWidget_H

#include <qapp.h>
#include "KToolbar.h"
#include <qmenubar.h> 
#include <qlist.h>
#include "KStatusbar.h"

class KTopLevelWidget : public QWidget {
  Q_OBJECT
  
  friend class KToolbar;
  
public:
  KTopLevelWidget(char *name=NULL);
  ~KTopLevelWidget();
  int addToolbar(KToolbar *toolbar, int index=-1);
  void setView(QWidget *view);
  void setMenu(QMenuBar *menu);
  void setStatusBar(KStatusBar *statusbar);
  void enableStatusBar(KToolbar::BarStatus stat = KToolbar::Toggle);
  void enableToolBar(KToolbar::BarStatus stat = KToolbar::Toggle, int ID=0); 
  KToolbar *toolBar(int ID=0);
  KStatusBar *statusBar();
  
protected:
  void resizeEvent( QResizeEvent *e);
  void focusInEvent ( QFocusEvent *);
  void focusOutEvent ( QFocusEvent *);
  void updateRects();

private:
  QList<KToolbar> toolbars;
  QWidget *kmainwidget;
  QMenuBar *kmenubar;
  KStatusBar *kstatusbar;
};

#endif
