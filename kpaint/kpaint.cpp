#include <unistd.h>
#include <string.h>
#include <qwidget.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include "QwViewport.h"
#include "KTopWidget.h"
#include "KToolbar.h"
#include "canvas.h"
#include "commands.h"
#include "version.h"
#include "kpaint.h"
#include "manager.h"
#include "canvassize.h"

KPaint::KPaint(char *name= 0) :
  KTopLevelWidget(name)
{
  int w, h; 

   w= 300;
   h= 200;

	   
   v= new QwViewport(this);
  c=  new Canvas("Untitled", w, h, v->portHole());

  v->resize(c->size());
  setView(v);

  man= new Manager(c);

  initToolbar();
  initMenus();

  toolbar->show();
  addToolbar(toolbar);
}

void KPaint::initToolbar(void)
{
  QString pixdir("/usr/local/kde/lib/pics/toolbar/");
  KPixmap pixmap;

  toolbar= new KToolbar(this);
 
  pixmap.load(pixdir + "filenew.xpm");
  toolbar->insertItem(pixmap, fileNewCommand, TRUE, "New Canvas");
  pixmap.load(pixdir + "fileopen.xpm");
  toolbar->insertItem(pixmap, fileOpenCommand, TRUE, "Open File");
  pixmap.load(pixdir + "filefloppy.xpm");
  toolbar->insertItem(pixmap, fileSaveCommand, TRUE, "Save File");

  connect(toolbar, SIGNAL(clicked(int)), SLOT(handleCommand(int)));
}

void KPaint::initMenus(void)
{
  QPopupMenu *file = new QPopupMenu;

  file->insertItem( "New Image...", fileNewCommand);
  file->insertItem( "Open Image...", fileOpenCommand);
  file->insertItem( "Save Image...", fileSaveCommand);
  file->insertItem( "Save Image As...", fileSaveAsCommand);
  file->insertItem( "Image Info...", fileInfoCommand);
  file->insertSeparator();
  file->insertItem( "New Window", newWindowCommand);
  file->insertItem( "Close", closeWindowCommand);
  file->insertItem( "Quit", exitCommand);

  QPopupMenu *tool= new QPopupMenu;
  tool->insertItem( "Tool Properties...", toolPropertiesCommand );
  file->insertSeparator();
  tool->insertItem( "Pen", setPenCommand );
  tool->insertItem( "Line", setLineCommand );
  tool->insertItem( "Rectangle", setRectangleCommand );
  tool->insertItem( "Ellipse", setEllipseCommand );
  tool->insertItem( "Circle", setCircleCommand );

  QPopupMenu *help = new QPopupMenu;
  help->insertItem( "Help on KPaint", helpContentsCommand);
  help->insertItem( "About KPaint", helpAboutCommand);
  connect (file, SIGNAL (activated (int)), SLOT (handleCommand(int)));
  connect (help, SIGNAL (activated (int)), SLOT (handleCommand(int)));
  connect (tool, SIGNAL (activated (int)), SLOT (handleCommand(int)));

  QMenuBar *menu = new QMenuBar( this );
  menu->insertItem( "&File", file );
  menu->insertItem( "&Tool", tool );
  menu->insertSeparator();
  menu->insertItem( "&Help", help );
  menu->show();
  setMenu(menu);
}

void KPaint::handleCommand(int i)
{
  QString filename;
	 int w, h;
	 canvasSizeDialog sz(0, "Canvas Size");

   //   Command command= (Command)i;
   
  switch(i) {
  case fileOpenCommand:
#ifdef KPDEBUG
    fprintf(stderr, "fileOpenCommand\n");
#endif	  
    filename= QFileDialog::getOpenFileName(0, "*.bmp", this);
    if (!filename.isNull())
      c->load(filename);
    break;

  case fileSaveAsCommand:
#ifdef KPDEBUG
    fprintf(stderr, "fileSaveAsCommand\n");
#endif	  
    filename= QFileDialog::getSaveFileName(0, "*.bmp", this);
#ifdef KPDEBUG
    fprintf(stderr, "fileSaveAsCommand: Filename is: %s\n", (const char *) filename);
#endif	  
    if (!filename.isNull())
      c->save(filename, "BMP");
    break;

  case fileSaveCommand:
    c->save(c->filename(), "BMP");
    break;

  case exitCommand:
    exit(0);
    break;

  case toolPropertiesCommand:
    man->showPropertiesDialog();
    break;

  case helpAboutCommand:
    QMessageBox::message("About " APPNAME,
			 APPVERSTR "\n" APPAUTHOR "\n" APPAUTHOREMAIL);
    break;
  case setEllipseCommand:
    man->setCurrentTool(0);
    break;
  case setCircleCommand:
    man->setCurrentTool(1);
    break;
  case setPenCommand:
	man->setCurrentTool(2);
	break;
  case setRectangleCommand:
	man->setCurrentTool(3);
	break;
  case setLineCommand:
	man->setCurrentTool(4);
	break;
  case helpIndexCommand:	  
  case helpContentsCommand:
	 if ( fork() == 0 )
	   {
		  execlp( "kdehelp", "kdehelp", APPHELPCONTENTSURL, 0 );
		  exit( 1 );
	   }
	 break;
  case fileNewCommand:
	 
		fprintf(stderr, "File New\n");
	 if (sz.exec()) {
		w= sz.getWidth();
		h= sz.getHeight();
		c->setFilename("Untitled");
		c->pixmap()->resize(w, h);
		c->resize(w, h);
		c->pixmap()->fill(QColor("white"));
		repaint(0);
	 }
    break;
  case fileInfoCommand:
  case closeWindowCommand:
  default:
#ifdef KPDEBUG
    fprintf(stderr, "Unknown command %d\n", i);
#endif	  
  }
}

#include "metas/kpaint.moc"
