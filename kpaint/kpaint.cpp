// $Id$

#define KPDEBUG

#include <string.h>
#include <qwidget.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include "QwViewport.h"
#include <kmenubar.h>
#include <kkeyconf.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmsgbox.h>
#include <config.h>
#include <kurl.h>
#include "canvas.h"
#include "version.h"
#include "kpaint.h"
#include "manager.h"
#include "canvassize.h"
#include "palettedialog.h"
#include "app.h"
#include "infodialog.h"
#include "formatdialog.h"
#include "formats.h"

// Tags for statusbar items
static const int FILESIZE_ID= 1;
static const int ZOOMFACTOR_ID= 2;
static const int FILENAME_ID= 3;

extern MyApp *kpaintApp;
extern FormatManager *formatMngr;
extern int openwins;

KPaint::KPaint(char *url_= NULL) : KTopLevelWidget()
{
  int w, h; 

  modified= false;
  tempURL= new QString;
  url= new QString;
  filename= new QString("untitled.gif");
  format= new QString("GIF");
  kfm= NULL;

  w= 300;
  h= 200;

  v= new QwViewport(this);
  c= new Canvas(w, h, v->portHole());

  v->resize(c->size());
  setView(v);

  connect(c, SIGNAL(sizeChanged()), v, SLOT(resizeScrollBars()));
  man= new Manager(c, this);

  zoom= 100;
  openwins++;

  initToolbar();
  initStatus();
  initMenus();

  // Look at config file for these
  toolbar->show();
  statusbar->show();
  addToolBar(toolbar);
  setStatusBar(statusbar);
  KToolBar *t= man->toolbar();
  t->show();
  addToolBar(t);

  // Was a URL specified?
  if (url_ != NULL) {
    QString u(url_);

    // Is there a ':'?
    int colon= u.find(':');

    if (colon != -1) {
#ifdef KPDEBUG
    fprintf(stderr, "KPaint:: initialising from URL %s\n", url_);
#endif
      loadRemote(url_);
    }
    else {
#ifdef KPDEBUG
    fprintf(stderr, "KPaint:: initialising from file %s\n", url_);
#endif
      loadLocal(url_);
    }
  }
}

KPaint::~KPaint()
{
  delete filename;
  delete format;
  delete url;
  delete man;
  delete tempURL;
}

void KPaint::closeEvent(QCloseEvent *e)
{
  QString proto;

  // Not used yet (0.3.1)
  if (modified) {
    fprintf(stderr, "KPaint: Should warn about deleting modified images\n");
  }

  openwins--;

  if (!url->isEmpty()) {
    KURL u(*url);

    if (!u.isMalformed()) {
      proto= u.protocol();

      if (proto != "file") {
#ifdef KPDEBUG
	fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename->data());
#endif
	unlink(*filename);
      }
    }
  }

  if (openwins == 0) {
    exit(0);
  }

  e->accept();
}

void KPaint::initToolbar()
{
  QString pixdir;
  QPixmap pixmap;

  toolbar= new KToolBar(this);
  pixdir= kpaintApp->kdedir()+"/lib/pics/toolbar/";
  pixmap.load(pixdir + "filenew.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileNew()), TRUE, "New Canvas");
  pixmap.load(pixdir + "fileopen.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileOpen()), TRUE, "Open File");
  pixmap.load(pixdir + "filefloppy.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(fileSave()), TRUE, "Save File");
  toolbar->insertSeparator();
  pixmap.load(pixdir + "editcut.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(editCut()), FALSE, "Cut");
  pixmap.load(pixdir + "editcopy.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(editCopy()), FALSE, "Copy");
  pixmap.load(pixdir + "editpaste.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(editPaste()), FALSE, "Paste");
  toolbar->insertSeparator();
  pixmap.load(pixdir + "viewmag+.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(editZoomIn()), TRUE, "Zoom In");
  pixmap.load(pixdir + "viewmag-.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(editZoomOut()), TRUE, "Zoom Out");
}

void KPaint::initStatus()
{
  QString size;
  statusbar= new KStatusBar(this);

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->insertItem(size.data(), FILESIZE_ID);
  statusbar->insertItem("100%", ZOOMFACTOR_ID);

  if (url->isEmpty())
    statusbar->insertItem(filename->data(), FILENAME_ID);
  else
    statusbar->insertItem(url->data(), FILENAME_ID);
}

void KPaint::initMenus()
{
  QPopupMenu *file;
  
  file = new QPopupMenu;

  file->insertItem("New Image...", this, SLOT(fileNew()));
  file->insertItem("Open Image...", this, SLOT(fileOpen()));
  file->insertItem("Save Image...", this, SLOT(fileSave()));
  file->insertItem("Save Image As...", this, SLOT(fileSaveAs()));
  file->insertItem("Image Format...", this, SLOT(fileFormat()));
  file->insertSeparator();
  file->insertItem("Open from URL...", this, SLOT(fileOpenURL()));
  file->insertItem("Save to URL...", this, SLOT(fileSaveAsURL()));
  file->insertSeparator();
  file->insertItem("New Window", this, SLOT(newWindow()));
  file->insertItem("Close Window", this, SLOT(closeWindow()));
  file->insertSeparator();
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
  help->insertItem( "Contents", this, SLOT(helpContents()));
  help->insertItem( "Index", this, SLOT(helpIndex()));
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

bool KPaint::loadLocal(const char *filename_, const char *url_= NULL)
{
  QString size;
  const char *fmt;
  bool result;

  // Check the file exists

  fmt= QPixmap::imageFormat(filename_);

  if (fmt != NULL) {
    if (c->load(filename_)) {
      result= true;
      *format= fmt;
      *filename= filename_;

      if (url_ == NULL) {
	*url= "";
	statusbar->changeItem(filename->data(), FILENAME_ID);
      }
      else {
	*url= url_;
	statusbar->changeItem(url->data(), FILENAME_ID);
      }


      size.sprintf("%d x %d", c->width(), c->height());
      statusbar->changeItem( size.data(), FILESIZE_ID);
    }
    else {
      KMsgBox::message(0, "KPaint: Could not open file",
		       "Error: Could not open file\n"
		       "KPaint could not open the specified file.",
		       KMsgBox::EXCLAMATION,
		       "Continue");
      result= false;
    }
  }
  else {
    KMsgBox::message(0, "KPaint: Unknown Format",
		     "Error: Unknown Format\n"
		     "KPaint does not understand the format of the specified file"
		     "or the file is corrupt.",
		     KMsgBox::EXCLAMATION,
		     "Continue");
    result= false;
  }

  return result;
}

// Initiates fetch of remote file
bool KPaint::loadRemote(const char *url_= NULL)
{
  KURL u(url_);

#ifdef KPDEBUG
  fprintf(stderr, "loadRemote: %s\n", url_);
  fprintf(stderr, "loadRemote: KURL=\"%s\"\n", (const char *) u.url());
  fprintf(stderr, "loadRemote: protocol=\"%s\"\n", u.protocol());
  fprintf(stderr, "loadRemote: host=\"%s\"\n", u.host());
  fprintf(stderr, "loadRemote: path=\"%s\"\n", u.path());
#endif

  if (u.isMalformed()) {
    KMsgBox::message(0, "KPaint: Malformed URL",
		     "Error: Malformed URL\n"
		     "KPaint does not understand the specifed URL, check it\'s correct.",
		     KMsgBox::EXCLAMATION,
		     "Continue");
    return false;
  }

  // Is it a local file?
  if (strcmp(u.protocol(), "file") == 0) {
#ifdef KPDEBUG
    fprintf(stderr, "Local file:%s\n",u.path());
#endif

    return loadLocal(u.path());
  }

  if (kfm != NULL) {
    QMessageBox::message ("Sorry", 
			  "KPaint is already accessing a remote file, you must\n"
			  "cancel the current remote job or wait for it to complete.",
			  "Continue");
    return false;
  }

  // Create a connection to kfm
  kfm = new KFM;

  // Check connection was made
  if ( !kfm->isOK() ) {
    QMessageBox::message ("KPaint: Could not find KFM",
			  "Error: KPaint could not find a running KFM or start one itself.",
			  "Continue");
    delete kfm;
    kfm = NULL;
    return false;
  }
  
  // Make up a name for the temporary file 
  filename->sprintf("/tmp/kpaint%i", time( 0L ));

  tempURL->sprintf("file:%s", (const char *) *filename);
  *url=  url_;

  // Call kfm finished when the job is complete
  connect( kfm, SIGNAL( finished() ), this, SLOT(KFMfinished()) );
  
  // Ask kfm to copy the remote file to the temp file
  kfmOp= KfmGet;
  kfm->copy(*url, *tempURL);

  return true;
}



void KPaint::KFMfinished()
{
  assert(kfm != NULL);

  if (kfmOp == KfmPut)
    KFMputFinished();
  else if (kfmOp == KfmGet)
    KFMgetFinished();
}

void KPaint::KFMgetFinished()
{
  assert(kfm != NULL);
  const char *filename_;
  const char *url_;

  // Free the file manager
  delete kfm;
  kfm= NULL;

#ifdef KPDEBUG
  fprintf(stderr, "Fetch completed, loading %s as %s...\n", (const char *) *filename,
	  (const char *) *url);
#endif

  filename_= *filename;
  url_= *url;

  // Now load the image locally
  if (!loadLocal(filename_, url_)) {
    KMsgBox::message(0, "KPaint: Network Load Failed",
		     "Error: Netword Load Failed\n"
		     "Something went wrong :-(",
		     KMsgBox::EXCLAMATION,
		     "Continue");
  }
}

void KPaint::KFMputFinished()
{
  assert(kfm != NULL);

  delete kfm;
  kfm= NULL;
}

bool KPaint::saveLocal(const char *filename_, const char *url_= NULL)
{
  *filename= filename_;
  *url= url_;

  return c->save(filename_, *format);
}

// Save As Network file
bool KPaint::saveRemote(const char *url_)
{
  KURL u(url_);
  QString newFilename;

  if (u.isMalformed()) {
    KMsgBox::message(0, "KPaint: Malformed URL",
		     "Error: Malformed URL\n"
		     "KPaint does not understand the specifed URL, check it\'s correct.",
		     KMsgBox::EXCLAMATION,
		     "Continue");
    return false;
  }

  // Is it a local file?
  if (strcmp(u.protocol(), "file") == 0) {
#ifdef KPDEBUG
    fprintf(stderr, "Save file:%s\n", u.path());
#endif

    return saveLocal(u.path(), url_);
  }

  if (kfm != NULL) {
    QMessageBox::message ("Sorry", 
			  "KPaint is already accessing a remote file, you must\n"
			  "cancel the current remote job or wait for it to complete.",
			  "Continue");
    return false;
  }

  // Create a connection to kfm
  kfm = new KFM;

  // Check connection was made
  if ( !kfm->isOK() ) {
    QMessageBox::message ("KPaint: Could not start KFM",
			  "Error: KPaint could not find a running KFM or start one itself.",
			  "Continue");
    delete kfm;
    kfm = NULL;
    return false;
  }
  
  // Save to a local file here so we don't overwrite the original
  newFilename.sprintf("/tmp/kpaint%i", time( 0L ));
  saveLocal(newFilename, url_);

  // Call kfm finished when the job is complete
  connect( kfm, SIGNAL( finished() ), this, SLOT(KFMfinished()) );
  
  // Ask kfm to copy the remote file to the temp file
  tempURL->sprintf("file:%s", (const char *) newFilename);
  kfmOp= KfmPut;
  kfm->copy(*tempURL, *url);

  return true;
}

void KPaint::fileNew()
{
  int w, h;
  QPixmap p;
  QString size;
  QString proto;
  canvasSizeDialog sz(0, "Canvas Size");

#ifdef KPDEBUG
  fprintf(stderr, "File New\n");
#endif

  if (sz.exec()) {
    if (!url->isEmpty()) {
      KURL u(*url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename->data());
#endif
	  unlink(*filename);
	}
      }
    }

    w= sz.getWidth();
    h= sz.getHeight();
    p.resize(w, h);
    p.fill(QColor("white"));
    c->setPixmap(&p);
    man->setCurrentTool(0);
    *filename= "untitled.gif";
    *url= "";
    *format= "GIF";

    statusbar->changeItem(filename->data(), FILENAME_ID);

    size.sprintf("%d x %d", w, h);
    statusbar->changeItem( size.data(), FILESIZE_ID);

    repaint(0);
  }
}


void KPaint::fileOpen()
{
  QString name;

#ifdef KPDEBUG
    fprintf(stderr, "fileOpen()\n");
#endif	  
    name=QFileDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
    if (!name.isNull()) {
      loadLocal(name);
    }
}

void KPaint::fileSave()
{
  c->save(*filename, *format);

  // If it was remote we need to send it back
  if (*url != "") {
    // PUT file
  }
  else {
    modified= false;
  }
}

void KPaint::fileSaveAs()
{
  QString newfilename;
  QString proto;

#ifdef KPDEBUG
  fprintf(stderr, "fileSaveAsCommand");
#endif
  // get the glob for the current format

  newfilename= QFileDialog::getSaveFileName(0,
					    formatMngr->glob(*format),
					    this);

#ifdef KPDEBUG
  fprintf(stderr, ": %s\n",  newfilename.data());
#endif	  

  if (!newfilename.isNull()) {
    if (!url->isEmpty()) {
      KURL u(*url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename->data());
#endif
	  unlink(*filename);
	}
      }
    }


      c->save(newfilename, *format);
      statusbar->changeItem(newfilename.data(), FILENAME_ID);

      *filename= newfilename;
      *url= "";
      modified= false;
    }
}


void KPaint::fileFormat()
{
  formatDialog dlg(*format);
#ifdef KPDEBUG
    fprintf(stderr, "fileFormat() %s\n", (const char *) *format);
#endif	  
    if (dlg.exec()) {
      fprintf(stderr, "Set format to %s\n",
	      dlg.fileformat->text(dlg.fileformat->currentItem()));

      *format= dlg.fileformat->text(dlg.fileformat->currentItem());
    }

}

void KPaint::fileQuit()
{
  QString proto;

#ifdef KPDEBUG
    fprintf(stderr, "fileQuit()\n");
#endif

    if (modified) {
      // Warn about unsaved changes
    }

    // Delete any temp files from the image
    if (!url->isEmpty()) {
      KURL u(*url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename->data());
#endif
	  unlink(*filename);
	}
      }
    }

    exit(0);
}

void KPaint::newWindow()
{
#ifdef KPDEBUG
    fprintf(stderr, "newWindow()\n");
#endif	  
   KPaint *kp;
   
   kp= new KPaint();
   kp->show();
}

void KPaint::closeWindow()
{
#ifdef KPDEBUG
    fprintf(stderr, "closeWindow()\n");
#endif	  

    close();
}

void KPaint::fileOpenURL()
{
  QString proto;

#ifdef KPDEBUG
  fprintf(stderr, "fileOpenURL()\n");
#endif

  // Get the URL to open
  DlgLocation l( "Open Location:", *url, this );

  if ( l.exec() ) {
    QString n = l.getText();

    // Delete any temp files from the previous image
    if (!url->isEmpty()) {
      KURL u(*url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename->data());
#endif
	  unlink(*filename);
	}
      }
    }

    // If the request was sent ok
    if (loadRemote(n)) {
      // Lock this window
#ifdef KPDEBUG
      fprintf(stderr, "Lock the window!\n");
#endif
    }
  }
}

void KPaint::fileSaveAsURL()
{
  fprintf(stderr, "fileSaveAsURL(): %s\n", (const char *) *url);

  // Get the URL to save to
  DlgLocation l( "Save to Location:", *url, this );

  if ( l.exec() ) {
    QString n = l.getText();

    saveRemote(n);
  }
}

// Edit
void KPaint::editCopy()
{
#ifdef KPDEBUG
    fprintf(stderr, "editCopy()\n");
#endif	  
}

void KPaint::editCut()
{
#ifdef KPDEBUG
    fprintf(stderr, "editCut()\n");
#endif	  
}

void KPaint::editPaste()
{
#ifdef KPDEBUG
    fprintf(stderr, "editPaste()\n");
#endif
}

void KPaint::editPasteImage()
{
#ifdef KPDEBUG
    fprintf(stderr, "editPasteImage()\n");
#endif
}

void KPaint::editZoomIn()
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

  QString zoomstr;
  char *s;

  zoomstr.setNum(zoom);
  zoomstr.append("%");
  s= strdup(zoomstr);

  statusbar->changeItem(s, ZOOMFACTOR_ID);

  free(s);
}

void KPaint::editZoomOut()
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

  QString zoomstr;
  char *s;

  zoomstr.setNum(zoom);
  zoomstr.append("%");
  s= strdup(zoomstr);

  statusbar->changeItem(s, ZOOMFACTOR_ID);

  free(s);
}

void KPaint::editMask()
{
#ifdef KPDEBUG
    fprintf(stderr, "editMask()\n");
#endif
}

void KPaint::editOptions()
{
#ifdef KPDEBUG
    fprintf(stderr, "editOptions()\n");
#endif
    kKeys->configureKeys(this);
}
  
// Image
void KPaint::imageInfo()
{
  imageInfoDialog info(c, 0, "Image Information");
#ifdef KPDEBUG
  fprintf(stderr, "imageInfo()\n");
#endif
  info.exec();
}

void KPaint::imageEditPalette()
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

void KPaint::toolProperties()
{
#ifdef KPDEBUG
  fprintf(stderr, "toolProperties()\n");
#endif
  man->showPropertiesDialog();
}


// Help
void KPaint::helpAbout()
{
#ifdef KPDEBUG
  fprintf(stderr, "helpAbout()\n");
#endif
  QMessageBox::message("About " APPNAME,
		       APPVERSTR "\n" APPAUTHOR "\n" APPAUTHOREMAIL);
}

void KPaint::helpContents()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
#ifdef KPDEBUG
    fprintf(stderr, "helpContents()\n");
#endif
   kpaintApp->invokeHTMLHelp(filename, topic);
}

void KPaint::helpIndex()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
#ifdef KPDEBUG
    fprintf(stderr, "helpIndex()\n");
#endif

   kpaintApp->invokeHTMLHelp(filename, topic);
}


#include "kpaint.moc"
