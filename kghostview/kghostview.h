#ifndef KGHOSTVIEW_H
#define KGHOSTVIEW_H

#include "kpswidget.h"
#include "info.h"
#include "viewcontrol.h"
#include "goto.h"
#include "print.h"
#include "ktopwidget.h"

#include <stdlib.h>
#include <math.h>

#include <signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#include <qwidget.h>
#include <qlabel.h>
#include <qstring.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qgrpbox.h>
#include <qaccel.h>

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

class KGhostview : public KTopLevelWidget
{
    Q_OBJECT

public:
	KGhostview( QWidget *parent=0, char *name=0 );
	~KGhostview();

	static QList <KGhostview> windowList;

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
	void writeFileSettings();
	void readSettings();
	
	void createToolbar();
	void createMenubar();
	void createStatusbar();
	
	KPSWidget *page;
	
	ViewControl *vc;
	PrintDialog *pd;
	InfoDialog *infoDialog;
	
	Bool force_pagemedia;
	Bool force_orientation;
	int orientation;
	int mark_page_list[1000];
	
	QString print_file(QString name, Bool whole_mode);
	void pscopydoc(FILE *fp);
	void printStart();
	
	void bindKeys();
	void changeMenuAccel( QPopupMenu *menu, int id, char *functionName );
	void updateMenuAccel();
	void changeFileRecord();
	
	FILE		*psfile;
	QString 	filename;	
	QString		oldfilename;	
	int			current_page;
	int 		pages_in_part[10];
	int 		num_parts;
	time_t		mtime;	
	struct 		document *doc;	
	struct 		document *olddoc;
	
	QString lastOpened[4];
	
public slots:

	void applyViewChanges();
	void scrollDown();
	void scrollUp();
	void scrollLeft();
	void scrollRight();
	void about();
	void help();
	void dummy();
	void viewControl();			
	void nextPage();
	void prevPage();
	void goToPage();
	void goToStart();
	void goToEnd();
	void print();
	void openNewFile();
	void zoomIn();
	void zoomOut();
	void optionsMenuActivated( int item );
	void fileMenuActivated( int item );
	void toolbarClicked( int item );
	void configureKeybindings();
	void shrinkWrap();
	void redisplay();
	void newWindow();
	void closeWindow();
	void info();
	void configureGhostscript();
     
protected:
	void paletteChange( const QPalette & );
   
	
private:

	KMenuBar *menubar;
	QPopupMenu *m_file;
	QPopupMenu *m_view;
	QPopupMenu *m_go;
	QPopupMenu *m_options;
	QPopupMenu *m_help;
	
	KToolBar *toolbar;
	KStatusBar *statusbar;
	
	int topOffset, bottomOffset;
	
	int viewControlID;
	int printID;
	int openID;
	int newWindowID;
	int closeWindowID;
	int quitID;
	int zoomInID;
	int zoomOutID;
	int nextID;
	int prevID;
	int goToPageID;
	int goToStartID;
	int goToEndID;
	int readDownID;
	int messagesID;
	int helpID;
	int shrinkWrapID;
	int redisplayID;
	int infoID;
	int file1ID;
	int file2ID;
	int file3ID;
	int file4ID;

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


