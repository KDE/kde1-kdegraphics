// $Id$

#include <string.h>
#include <qwidget.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include "QwViewport.h"
#include <kmenubar.h>
#include <kkeyconf.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include "canvas.h"
#include "version.h"
#include "kpaint.h"
#include "manager.h"
#include "canvassize.h"
#include "palettedialog.h"
#include "app.h"
#include "infodialog.h"
#include "formats.h"

extern MyApp *kpaintApp;
extern FormatManager *formatMngr;

KPaint::KPaint(char *name= 0) :
  KTopLevelWidget(name)
{
  int w, h; 
  
  w= 300;
  h= 200;

  v= new QwViewport(this);
  c= new Canvas(w, h, v->portHole());

  v->resize(c->size());
  setView(v);

  connect(c, SIGNAL(sizeChanged()), v, SLOT(resizeScrollBars()));
  man= new Manager(c, this);

  zoom= 100;

  initToolbar();
  initMenus();

  toolbar->show();
  addToolBar(toolbar);
  KToolBar *t= man->toolbar();
  t->show();
  addToolBar(t);
}

void KPaint::initToolbar(void)
{
  QString pixdir;
  QPixmap pixmap;

  toolbar= new KToolBar(this);
  pixdir= kpaintApp->kdedir()+"/lib/pics/toolbar/";
  pixmap.load(pixdir + "filenew.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileNew()), TRUE, "New Canvas");
  pixmap.load(pixdir + "fileopen.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileOpen()), TRUE, "Open File");
  pixmap.load(pixdir + "filefloppy.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileSave()), TRUE, "Save File");
  toolbar->insertSeparator();
  pixmap.load(pixdir + "editcut.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(editCut()), TRUE, "Cut");
  pixmap.load(pixdir + "editcopy.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(editCopy()), TRUE, "Copy");
  pixmap.load(pixdir + "editpaste.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(editPaste()), TRUE, "Paste");
  toolbar->insertSeparator();
  pixmap.load(pixdir + "viewmag+.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(editZoomIn()), TRUE, "Zoom In");
  pixmap.load(pixdir + "viewmag-.xpm");
  toolbar->insertItem(pixmap, 0, SIGNAL(clicked()), this, SLOT(editZoomOut()), TRUE, "Zoom Out");
}

void KPaint::initMenus(void)
{
  QPopupMenu *file = new QPopupMenu;

  file->insertItem("New Image...", this, SLOT(fileNew()));
  file->insertItem("Open Image...", this, SLOT(fileOpen()));
  file->insertItem("Save Image...", this, SLOT(fileSave()));
  file->insertItem("Save Image As...", this, SLOT(fileSaveAs()));
  file->insertItem("Image Info...", this, SLOT(fileInfo()));
  file->insertSeparator();
  file->insertItem("New Window", this, SLOT(newWindow()));
  file->insertItem("Close", this, SLOT(closeWindow()));
  file->insertItem("Quit", this, SLOT(fileQuit()));

  QPopupMenu *edit= new QPopupMenu;
  edit->insertItem("Copy Region", this, SLOT(editCopy()));
  edit->insertItem("Cut Region", this, SLOT(editCut()));
  edit->insertItem("Paste Region", this, SLOT(editPaste()));
  edit->insertItem("Paste As Image", this, SLOT(editPasteImage()));
  edit->insertSeparator();
  edit->insertItem("Zoom In", this, SLOT(editZoomIn()));
  edit->insertItem("Zoom Out", this, SLOT(editZoomOut()));
  edit->insertItem("Mask...", this, SLOT(editMask()));
  edit->insertItem("Options...", this, SLOT(editOptions()));

  QPopupMenu *image= new QPopupMenu;
  image->insertItem("Information...", this, SLOT(imageInfo()));
  image->insertItem("Edit Palette...", this, SLOT(imageEditPalette()));
  image->insertItem("Change Colour Depth...", this, SLOT(imageChangeDepth()));

  QPopupMenu *tool= new QPopupMenu;
  tool->insertItem( "Tool Properties...", -1);
  tool->insertSeparator();
  tool->insertItem( "Pen", 2);
  tool->insertItem( "Line", 4);
  tool->insertItem( "Rectangle", 3);
  tool->insertItem( "Ellipse", 0 );
  tool->insertItem( "Circle", 1 );
  tool->insertItem( "Spray Can", 5);
  connect(tool, SIGNAL(activated(int)), SLOT(setTool(int)));

  QPopupMenu *help = new QPopupMenu;
  help->insertItem( "KPaint Help Contents", this, SLOT(helpContents()));
  help->insertItem( "KPaint Help Index", this, SLOT(helpIndex()));
  help->insertItem( "About KPaint", this, SLOT(helpAbout()));

  KMenuBar *menu = new KMenuBar( this );
  menu->insertItem( "&File", file );
  menu->insertItem( "&Edit", edit );
  menu->insertItem( "&Image", image );
  menu->insertItem( "&Tool", tool );
  menu->insertSeparator();
  menu->insertItem( "&Help", help );
  menu->show();
  setMenu(menu);
}

void KPaint::fileNew(void)
{
  int w, h;
  QPixmap p;
  canvasSizeDialog sz(0, "Canvas Size");

#ifdef KPDEBUG
  fprintf(stderr, "File New\n");
#endif
  if (sz.exec()) {
    w= sz.getWidth();
    h= sz.getHeight();
    p.resize(w, h);
    p.fill(QColor("white"));
    c->setPixmap(&p);
    man->setCurrentTool(0);
    repaint(0);
  }
}


void KPaint::fileOpen(void)
{
  QString currfilename;
#ifdef KPDEBUG
    fprintf(stderr, "fileOpen()\n");
#endif	  
    currfilename=QFileDialog::getOpenFileName(0, formatMngr->allImagesGlob(),
					      this);
    if (!currfilename.isNull())
      c->load(currfilename);
}

void KPaint::fileSave(void)
{
  c->save();
}

void KPaint::fileSaveAs(void)
{
  QString newfilename;

#ifdef KPDEBUG
    fprintf(stderr, "fileSaveAsCommand");
#endif
    
    newfilename= QFileDialog::getSaveFileName(0, "*.gif", this);

#ifdef KPDEBUG
    fprintf(stderr, ": %s\n", (const char *) newfilename);
#endif	  

    if (!newfilename.isNull()) {
      c->save(newfilename, "GIF");
    }
}

void KPaint::fileInfo(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "fileInfo()\n");
#endif	  
}

void KPaint::fileQuit(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "fileQuit()\n");
#endif
    exit(0);
}

void KPaint::newWindow(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "newWindow()\n");
#endif	  
   KPaint *kp;
   
   kp= new KPaint();
   kp->show();
}

void KPaint::closeWindow(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "closeWindow()\n");
#endif	  
   delete this;
}

// Edit
void KPaint::editCopy(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editCopy()\n");
#endif	  
}

void KPaint::editCut(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editCut()\n");
#endif	  
}

void KPaint::editPaste(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editPaste()\n");
#endif
}

void KPaint::editPasteImage(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editPasteImage()\n");
#endif
}

void KPaint::editZoomIn(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editZoomIn()\n");
#endif
  if (zoom >= 100) {
    zoom= zoom+100;
    if (zoom > 1000)
      zoom= 1000;
    c->setZoom(zoom);
  }
  else if (zoom < 100) {
    zoom= zoom+10;
    c->setZoom(zoom);
  }
}

void KPaint::editZoomOut(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "editZoomOut()\n");
#endif
  if (zoom > 100) {
    zoom= zoom-100;
    c->setZoom(zoom);
  }
  else if (zoom <= 100) {
    zoom= zoom-10;
    if (zoom == 0)
      zoom= 10;
    c->setZoom(zoom);
  }
}

void KPaint::editMask(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editMask()\n");
#endif
}

void KPaint::editOptions(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "editOptions()\n");
#endif
    kKeys->configureKeys(this);
}
  
// Image
void KPaint::imageInfo(void)
{
  imageInfoDialog info(c, 0, "Image Information");
#ifdef KPDEBUG
  fprintf(stderr, "imageInfo()\n");
#endif
  info.exec();
}

void KPaint::imageEditPalette(void)
{
#ifdef KPDEBUG
    fprintf(stderr, "imageEditPalette()\n");
#endif
    paletteDialog pal(c->pixmap());

    if (pal.exec()) {
      c->setPixmap(pal.pixmap());
      c->repaint(0);
    }
}

void KPaint::imageChangeDepth()
{
#ifdef KPDEBUG
  fprintf(stderr, "imageChangeDepth()\n");
#endif
}

// Tool
void KPaint::setTool(int t)
{
#ifdef KPDEBUG
  fprintf(stderr, "setTool(%d)\n", t);
#endif
  if (t > 0)
    man->setCurrentTool(t);
  else 
    man->showPropertiesDialog();
}

void KPaint::toolProperties(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "toolProperties()\n");
#endif
  man->showPropertiesDialog();
}


// Help
void KPaint::helpAbout(void)
{
#ifdef KPDEBUG
  fprintf(stderr, "helpAbout()\n");
#endif
  QMessageBox::message("About " APPNAME,
		       APPVERSTR "\n" APPAUTHOR "\n" APPAUTHOREMAIL);
}

void KPaint::helpContents(void)
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
#ifdef KPDEBUG
    fprintf(stderr, "helpContents()\n");
#endif
   kpaintApp->invokeHTMLHelp(filename, topic);
}

void KPaint::helpIndex(void)
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
#ifdef KPDEBUG
    fprintf(stderr, "helpIndex()\n");
#endif

   kpaintApp->invokeHTMLHelp(filename, topic);
}


#include "kpaint.moc"
