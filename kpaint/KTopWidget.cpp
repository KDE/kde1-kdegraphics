#include "KTopWidget.h"

#include "metas/KTopWidget.moc"

KTopLevelWidget::KTopLevelWidget(char *name=NULL) : QWidget(0L,name) {
  kmenubar = NULL;
  kmainwidget = NULL;
  kstatusbar = NULL;
};

KTopLevelWidget::~KTopLevelWidget() {
};

int KTopLevelWidget::addToolbar(KToolbar *toolbar, int index) {
  if (index==-1)
    toolbars.append(toolbar);
  else
    toolbars.insert(index, toolbar);
  index = toolbars.at();
  updateRects();
  return index;
};

void KTopLevelWidget::setView(QWidget *view) {
  kmainwidget = view;
};

void KTopLevelWidget::setMenu(QMenuBar *menu) {
  kmenubar = menu;
};

void KTopLevelWidget::setStatusBar(KStatusBar *statusbar) {
  kstatusbar = statusbar;
};

void KTopLevelWidget::focusInEvent ( QFocusEvent *) {
  repaint(FALSE);
};

void KTopLevelWidget::focusOutEvent ( QFocusEvent *) {
  repaint(FALSE);
};

void KTopLevelWidget::updateRects() {
  int t=0, b=0, l=0, r=0;
  int to=-1, bo=-1, lo=-1, ro=-1;
  int h = height();
  
  if (kmenubar && kmenubar->isVisible()) {
    t += kmenubar->height(); // the menu is always on top
  };

  if (kstatusbar && kstatusbar->isVisible()) {
    kstatusbar->setGeometry(0, height() - kstatusbar->height(),
			    width(), kstatusbar->height());
    b += kstatusbar->height();
  };
  for (KToolbar *toolbar=toolbars.first();
       toolbar != NULL; toolbar=toolbars.next()) 
    if (toolbar->Pos() == KToolbar::Top && toolbar->isVisible()) {
      if (to<0) {
	to=0;
	t+=toolbar->height();
      };
      if (to + toolbar->width() > width()) {
	to=0;
	t+=toolbar->height();
      };
      toolbar->move(to, t-toolbar->height());
      to += toolbar->width() + 3;
    };
  for (KToolbar *toolbar=toolbars.first();
       toolbar != NULL; toolbar=toolbars.next()) 
    if (toolbar->Pos() == KToolbar::Bottom && toolbar->isVisible()) {
      if (bo<0) {
	bo=0;
	b+=toolbar->height();
      };
      if (bo + toolbar->width() > width()) {
	bo=0;
	b+=toolbar->height();
      };
      toolbar->move(bo, height() - b );
      bo += toolbar->width();
    };
  h = height() - t - b;
  for (KToolbar *toolbar=toolbars.first();
       toolbar != NULL; toolbar=toolbars.next()) 
    if (toolbar->Pos() == KToolbar::Left && toolbar->isVisible()) {
      if (lo<0) {
	lo=0;
	l+=toolbar->width();
      };
      if (lo + toolbar->height() > h) {
	lo=0;
	l+=toolbar->width();
      };
      toolbar->move(l-toolbar->width(), t + lo);
      lo += toolbar->height();
    };
  for (KToolbar *toolbar=toolbars.first();
       toolbar != NULL; toolbar=toolbars.next()) 
    if (toolbar->Pos() == KToolbar::Right && toolbar->isVisible()) {
      if (ro<0) {
	ro=0;
	r+=toolbar->width();
      };
      if (ro + toolbar->height() > h) {
	ro=0;
	r+=toolbar->width();
      };
      toolbar->move(width() - r, t + ro);
      ro += toolbar->height();
    };
    
  if (kmainwidget)
    kmainwidget->setGeometry(l,t,width()-l-r,height()-t-b);
};

void KTopLevelWidget::resizeEvent( QResizeEvent *) {
  //menu resizes themself
  updateRects();
};

KStatusBar *KTopLevelWidget::statusBar() {
  return kstatusbar;
};

KToolbar *KTopLevelWidget::toolBar(int ID) {
  return toolbars.at(ID);
};

void KTopLevelWidget::enableToolBar(KToolbar::BarStatus stat, int ID) {
  KToolbar *t = toolbars.at(ID);
  if (t)
    t->enable(stat);
  updateRects();
};

void KTopLevelWidget::enableStatusBar(KToolbar::BarStatus stat) {
  CHECK_PTR(kstatusbar);
  if ((stat == KToolbar::Toggle && kstatusbar->isVisible()) || 
      stat == KToolbar::Hide)
    kstatusbar->hide();
  else
    kstatusbar->show();
  updateRects();
};


