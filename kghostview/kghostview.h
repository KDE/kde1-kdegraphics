#ifndef KGHOSTVIEW_H
#define KGHOSTVIEW_H

#include "kpswidget.h"
#include "viewcontrol.h"
#include "goto.h"
#include "print.h"

#include <stdlib.h>
#include <math.h>

#include <signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#include <qwidget.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qstring.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qgrpbox.h>
#include <qaccel.h>

#include <kpixmap.h>

#define TOOLBAR_HEIGHT 26
#define TOOLBAR_X_OFFSET 10
#define TOOLBAR_Y_OFFSET 5
#define TOOLBAR_BUTTON_HEIGHT 28
#define TOOLBAR_BUTTON_WIDTH 28

#define BUTTON_HEIGHT		25
#define BUTTON_WIDTH		25
#define BUTTON_SEPARATION	6

#define MENUBAR_HEIGHT 28

#define STATUSBAR_HEIGHT 20
#define STATUSBAR_PAGE_WIDTH 140
#define STATUSBAR_ZOOM_WIDTH 45
#define STATUSBAR_ORIENT_WIDTH 90

#define PAGE_X_OFFSET 2

enum {OPEN, PRINT_WHOLE, PRINT_MARKED, SAVE};

extern FILE		*psfile;
extern QString 	filename;	
extern QString	oldfilename;	
extern int		current_page;
extern int pages_in_part[10];
extern int num_parts;	
extern time_t	mtime;		
extern struct 	document *doc;	
extern struct 	document *olddoc;
extern Atom 	gs_colors, ghostview, next, gs_page, done;    
extern Display 	*gs_display;
extern Window 	gs_window;
extern Window 	mwin;
extern Bool busy;

class KGhostview : public QWidget
{
    Q_OBJECT

public:
	KGhostview();

	Bool setup();
	void show_page(int number);
	void build_pagemedia_menu();
	void set_magstep(int i);
	void magnify(float *dpi, int magstep);
	void new_file(int number);
	QString open_file(QString name);
	Bool same_document_media();
	Bool set_new_orientation(int number);
	Bool set_new_pagemedia( int number );
	Bool set_new_magstep();
	
	void writeSettings();
	void readSettings();
	
	void changeFonts(const QFont &newFont);
	
	void enableToolbarButton( int id, bool enable, QButtonGroup *toolbar );
	void initToolBar();
	
	KPSWidget *page;
	
	ViewControl *vc;
	PrintDialog *pd;
	
	Bool force_pagemedia;
	Bool force_orientation;
	int orientation;
	int mark_page_list[1000];
	
	QString print_file(QString name, Bool whole_mode);
	void pscopydoc(FILE *fp);
	void printStart();
	
public slots:
	void apply_view_changes();
	void scroll_down();
	void scroll_up();
	void scroll_left();
	void scroll_right();
	void about();
	void help();
   void dummy();
   void view_control();			
   void next_page();
   void prev_page();
   void go_to_page();
   void print();
   void open_new_file();
   void zoom_in();
   void zoom_out();
   void options_menu_activated(int item);
     
protected:
   void resizeEvent( QResizeEvent * );
	
private:
	QMenuBar *menubar;
	QPopupMenu *m_file;
	QPopupMenu *m_view;
	QPopupMenu *m_go;
	QPopupMenu *m_options;
	QPopupMenu *m_help;
	
	QButtonGroup *toolbar;
	KPixmap toolbarPixmaps[18];
	
	int topOffset, bottomOffset;
	
	int viewID;
	int printID;
	int nextID;
	int prevID;
	int goID;
	int messagesID;
	
	
	QFrame *frame;

	QLabel *statusPageLabel;
	QLabel *statusZoomLabel;
	QLabel *statusOrientLabel;
	QLabel *statusMiscLabel;

	Bool hide_toolbar;
	Bool hide_statusbar;
	Bool fitWindow;
	int options_width;
	int options_height;
	int shrink_magsteps, expand_magsteps, magstep, current_magstep;
	Bool	force_document_media;	
	int	base_papersize;
	Bool	toc_text;
	int	toc_length;
	int	toc_entry_length;
	char page_total_label[20];
	char part_total_label[20];
	QString part_string;
	QString page_string;
	QString page_label_text;
	QString part_label_text;
	QString position_label;
	float default_xdpi, default_ydpi;
	int current_llx;
	int current_lly;
	int current_urx;
	int current_ury;
	int current_pagemedia, default_pagemedia, document_media;
	int current_orientation;
	Bool changed;
};

#endif // KGHOSTVIEW_H


