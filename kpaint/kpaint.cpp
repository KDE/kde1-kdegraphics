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
#include <klocale.h>
#include <config.h>
#include <kurl.h>
#include <qregexp.h>
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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>

extern MyApp *kpaintApp;
extern FormatManager *formatMngr;
extern int openwins;

KPaint::KPaint(const char *url_) : KTopLevelWidget()
{
  int w, h; 

  modified= false;
  filename= klocale->translate("untitled.gif");
  format= "GIF";
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
  delete man;
}

void KPaint::setPixmap(QPixmap *p)
{
  format= "GIF";
  filename= "Untitled.gif";
  modified= false;
  url= "";
  c->setPixmap(p);
}

int KPaint::exit()
{
  int die= 0;

  if (!modified)
    die= 1;
  else
    if (QMessageBox::warning(this, klocale->translate("Unsaved Changes"),
			      "You have unsaved changes, you will loose them "
			      "if you exit now.",
			      "Exit", "Cancel",
			      0, 1, 1))
      die= 0;
    else
      die= 1;

  return die;
}

void KPaint::closeEvent(QCloseEvent *e)
{
  QString proto;

  // Catch if modified
  if (exit())
    e->accept();

  openwins--;

  if (!url.isEmpty()) {
    KURL u(url);

    if (!u.isMalformed()) {
      proto= u.protocol();

      if (proto != "file") {
#ifdef KPDEBUG
	fprintf(stderr, "KPaint: Deleting temp \'%s\'\n", filename.data());
#endif
	unlink(filename);
      }
    }
  }

  if (openwins == 0)
    ::exit(0);
}

void KPaint::initToolbar()
{
  QPixmap pixmap;
  QString pixdir= kapp->kdedir();

  toolbar= new KToolBar(this);

  pixdir.append("/share/apps/kpaint/toolbar/");
  pixmap.load(pixdir+"filenew.xpm");
  toolbar->insertButton(pixmap, ID_NEW, TRUE, klocale->translate("New Canvas"));

  pixmap.load(pixdir+"fileopen.xpm");
  toolbar->insertButton(pixmap, ID_OPEN, TRUE, klocale->translate("Open File"));

  pixmap.load(pixdir+"filefloppy.xpm");
  toolbar->insertButton(pixmap, ID_SAVE, TRUE, klocale->translate("Save File"));
  toolbar->insertSeparator();

  pixmap.load(pixdir+"editcopy.xpm");
  toolbar->insertButton(pixmap, ID_COPY, FALSE, klocale->translate("Copy"));

  pixmap.load(pixdir+"editcut.xpm");
  toolbar->insertButton(pixmap, ID_CUT, FALSE, klocale->translate("Cut"));

  pixmap.load(pixdir+"editpaste.xpm");
  toolbar->insertButton(pixmap, ID_PASTE, FALSE, klocale->translate("Paste"));
  toolbar->insertSeparator();

  pixmap.load(pixdir+"viewmag+.xpm");
  toolbar->insertButton(pixmap, ID_ZOOMIN, TRUE, klocale->translate("Zoom In"));

  pixmap.load(pixdir+"viewmag-.xpm");
  toolbar->insertButton(pixmap, ID_ZOOMOUT, TRUE, klocale->translate("Zoom Out"));
}

void KPaint::initStatus()
{
  QString size;
  statusbar= new KStatusBar(this);

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->insertItem(size.data(), ID_FILESIZE);
  statusbar->insertItem("100%", ID_ZOOMFACTOR);

  if (url.isEmpty())
    statusbar->insertItem(filename, ID_FILENAME);
  else
    statusbar->insertItem(url, ID_FILENAME);
}

void KPaint::initMenus()
{
  QPopupMenu *file;
  
  file = new QPopupMenu;

  file->insertItem(klocale->translate("Open Image..."), ID_OPEN);
  file->insertItem(klocale->translate("New Image..."), ID_NEW);
  file->insertItem(klocale->translate("Save Image..."), ID_SAVE);
  file->insertItem(klocale->translate("Save Image As..."), ID_SAVEAS);
  file->insertItem(klocale->translate("Image Format..."), ID_FORMAT);
  file->insertSeparator();
  file->insertItem(klocale->translate("Open from URL..."), ID_OPENURL);
  file->insertItem(klocale->translate("Save to URL..."), ID_SAVEURL);
  file->insertSeparator();
  file->insertItem(klocale->translate("New Window"), ID_NEWWINDOW);
  file->insertItem(klocale->translate("Close Window"), ID_CLOSEWINDOW);
  file->insertSeparator();
  file->insertItem(klocale->translate("Exit"), ID_EXIT);

  QPopupMenu *edit= new QPopupMenu;
  edit->insertItem(klocale->translate("Copy Region"), ID_COPY);
  edit->insertItem(klocale->translate("Cut Region"), ID_CUT);
  edit->insertItem(klocale->translate("Paste Region"), ID_PASTE);
  edit->insertItem(klocale->translate("Paste As Image"), ID_PASTEIMAGE);
  edit->insertSeparator();
  edit->insertItem(klocale->translate("Zoom In"), ID_ZOOMIN);
  edit->insertItem(klocale->translate("Zoom Out"), ID_ZOOMOUT);
  edit->insertItem(klocale->translate("Mask..."), ID_MASK);

  QPopupMenu *image= new QPopupMenu;
  image->insertItem(klocale->translate("Information..."), ID_INFO);
  image->insertItem(klocale->translate("Edit Palette..."), ID_PALETTE);
  image->insertItem(klocale->translate("Change Colour Depth..."), ID_DEPTH);

  QPopupMenu *tool= new QPopupMenu;
  tool->insertItem( klocale->translate("Tool Properties..."), -1);
  tool->insertSeparator();
  tool->insertItem( klocale->translate("Pen"), 2);
  tool->insertItem( klocale->translate("Line"), 4);
  tool->insertItem( klocale->translate("Rectangle"), 3);
  tool->insertItem( klocale->translate("Ellipse"), 0 );
  tool->insertItem( klocale->translate("Circle"), 1 );
  tool->insertItem( klocale->translate("Spray Can"), 5);
  tool->insertItem( klocale->translate("Area Select"), 6);
  connect(tool, SIGNAL(activated(int)), SLOT(setTool(int)));

  QPopupMenu *options = new QPopupMenu;
  options->setCheckable(true);
  options->insertItem( klocale->translate("Show Toolbar"), ID_SHOWTOOLBAR);
  options->insertItem( klocale->translate("Show Status Bar"), ID_SHOWSTATUSBAR);
  options->insertItem( klocale->translate("Save Options"), ID_SAVEOPTIONS);

  QPopupMenu *help = new QPopupMenu;
  help->insertItem( klocale->translate("Contents"), ID_HELPCONTENTS);
  help->insertItem( klocale->translate("Release Notes"), ID_RELEASENOTES);
  help->insertItem( klocale->translate("About..."), ID_ABOUT);

  KMenuBar *menu = new KMenuBar( this );
  menu->insertItem( klocale->translate("&File"), file );
  menu->insertItem( klocale->translate("&Edit"), edit );
  menu->insertItem( klocale->translate("&Image"), image );
  menu->insertItem( klocale->translate("&Tool"), tool );
  menu->insertItem( klocale->translate("&Options"), options );
  menu->insertSeparator();
  menu->insertItem( klocale->translate("&Help"), help );

  connect (file, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (edit, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (image, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (options, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (help, SIGNAL (activated (int)), SLOT (handleCommand (int)));

  menu->show();
  setMenu(menu);
}

void KPaint::handleCommand(int command)
{
  switch (command) {
  case ID_NEW:
    fileNew();
    break;
  case ID_OPEN:
    fileOpen();
    break;
  case ID_SAVE:
    fileSave();
    break;
  case ID_SAVEAS:
    fileSaveAs();
    break;
  case ID_FORMAT:
    fileFormat();
    break;
  case ID_OPENURL:
    fileOpenURL();
    break;
  case ID_SAVEURL:
    fileSaveAsURL();
    break;
  case ID_NEWWINDOW:
    newWindow();
    break;
  case ID_CLOSEWINDOW:
    closeWindow();
    break;
  case ID_EXIT:
    fileExit();
    break;
    
  // Edit
  case ID_COPY:
    editCopy();
    break;
  case ID_CUT:
    editCut();
    break;
  case ID_PASTE:
    editPaste();
    break;
  case ID_PASTEIMAGE:
    editPasteImage();
    break;
  case ID_ZOOMIN:
    editZoomIn();
    break;
  case ID_ZOOMOUT:
    editZoomOut();
    break;
  case ID_MASK:
    editMask();
    break;
  case ID_OPTIONS:
    editOptions();
    break;
  
  // Image
  case ID_INFO:
    imageInfo();
    break;
  case ID_PALETTE:
    imageEditPalette();
    break;
  case ID_DEPTH:
    imageChangeDepth();
    break;

    // Options
  case ID_SHOWTOOLBAR:
    enableToolBar(KToolBar::Toggle, ID_TOOLTOOLBAR);
    enableToolBar(KToolBar::Toggle, ID_FILETOOLBAR);
    break;
  case ID_SHOWMENUBAR:
    break;
  case ID_SHOWSTATUSBAR:
    enableStatusBar();
    break;
  case ID_SAVEOPTIONS:
    break;

  // Help
  case ID_ABOUT:
    helpAbout();
    break;
  case ID_HELPCONTENTS:
    helpContents();
    break;
  case ID_RELEASENOTES:
    helpIndex();
    break;
  }
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
      format= fmt;
      filename= filename_;
      modified= false;

      if (url_ == NULL) {
	url= "";
	statusbar->changeItem(filename.data(), ID_FILENAME);
      }
      else {
	url= url_;
	statusbar->changeItem(url.data(), ID_FILENAME);
      }


      size.sprintf("%d x %d", c->width(), c->height());
      statusbar->changeItem( size.data(), ID_FILESIZE);
    }
    else {
      KMsgBox::message(0, klocale->translate("KPaint: Could not open file"),
		       klocale->translate("Error: Could not open file\n"
		       "KPaint could not open the specified file."),
		       KMsgBox::EXCLAMATION,
		       klocale->translate("Continue"));
      result= false;
    }
  }
  else {
    KMsgBox::message(0, klocale->translate("KPaint: Unknown Format"),
		     klocale->translate("Error: Unknown Format\n"
		     "KPaint does not understand the format of the specified file"
		     "or the file is corrupt."),
		     KMsgBox::EXCLAMATION,
		     klocale->translate("Continue"));
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
    KMsgBox::message(0, klocale->translate("KPaint: Malformed URL"),
		     klocale->translate("Error: Malformed URL\n"
		     "KPaint does not understand the specifed URL, check it\'s correct."),
		     KMsgBox::EXCLAMATION,
		     klocale->translate("Continue"));
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
    QMessageBox::message (klocale->translate("Sorry"), 
			  klocale->translate("KPaint is already accessing a remote file, you must\n"
			  "cancel the current remote job or wait for it to complete."),
			  klocale->translate("Continue"));
    return false;
  }

  // Create a connection to kfm
  kfm = new KFM;

  // Check connection was made
  if ( !kfm->isOK() ) {
    QMessageBox::message (klocale->translate("KPaint: Could not find KFM"),
			  klocale->translate("Error: KPaint could not find a running KFM or start one itself."),
			  klocale->translate("Continue"));
    delete kfm;
    kfm = NULL;
    return false;
  }
  
  // Make up a name for the temporary file 
  filename.sprintf("/tmp/kpaint%i", time( 0L ));

  tempURL.sprintf("file:%s", (const char *) filename);
  url=  url_;

  // Call kfm finished when the job is complete
  connect( kfm, SIGNAL( finished() ), this, SLOT(KFMfinished()) );
  
  // Ask kfm to copy the remote file to the temp file
  kfmOp= KfmGet;
  kfm->copy(url, tempURL);

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
  fprintf(stderr, "Fetch completed, loading %s as %s...\n", (const char *) filename,
	  (const char *) url);
#endif

  filename_= filename;
  url_= url;

  // Now load the image locally
  if (!loadLocal(filename_, url_)) {
    KMsgBox::message(0, klocale->translate("KPaint: Network Load Failed"),
		     klocale->translate("Error: Netword Load Failed\n"
		     "Something went wrong :-("),
		     KMsgBox::EXCLAMATION,
		     klocale->translate("Continue"));
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
  filename= filename_;
  url= url_;

  return c->save(filename_, format);
}

// Save As Network file
bool KPaint::saveRemote(const char *url_)
{
  KURL u(url_);
  QString newFilename;

  if (u.isMalformed()) {
    KMsgBox::message(0, klocale->translate("KPaint: Malformed URL"),
		     klocale->translate("Error: Malformed URL\n"
		     "KPaint does not understand the specifed URL, check it\'s correct."),
		     KMsgBox::EXCLAMATION,
		     klocale->translate("Continue"));
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
    QMessageBox::message (klocale->translate("Sorry"), 
			  klocale->translate("KPaint is already accessing a remote file, you must\n"
			  "cancel the current remote job or wait for it to complete."),
			  klocale->translate("Continue"));
    return false;
  }

  // Create a connection to kfm
  kfm = new KFM;

  // Check connection was made
  if ( !kfm->isOK() ) {
    QMessageBox::message (klocale->translate("KPaint: Could not start KFM"),
			  klocale->translate("Error: KPaint could not find a running KFM or start one itself."),
			  klocale->translate("Continue"));
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
  tempURL.sprintf("file:%s", (const char *) newFilename);
  kfmOp= KfmPut;
  kfm->copy(tempURL, url);

  return true;
}

void KPaint::fileNew()
{
  int w, h;
  QPixmap p;
  QString size;
  QString proto;
  canvasSizeDialog sz(0, klocale->translate("Canvas Size"));

#ifdef KPDEBUG
  fprintf(stderr, "File New\n");
#endif

  if (sz.exec()) {
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename.data());
#endif
	  unlink(filename);
	}
      }
    }

    w= sz.getWidth();
    h= sz.getHeight();
    p.resize(w, h);
    p.fill(QColor("white"));
    c->setPixmap(&p);
    man->setCurrentTool(0);
    filename= klocale->translate("untitled.gif");
    url= "";
    format= "GIF";

    statusbar->changeItem(filename.data(), ID_FILENAME);

    size.sprintf("%d x %d", w, h);
    statusbar->changeItem( size.data(), ID_FILESIZE);

    repaint(0);
  }
}


void KPaint::fileOpen()
{
  QString name;

#ifdef KPDEBUG
    fprintf(stderr, "fileOpen()\n");
#endif	  
    //    name=QFileDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
    name=QFileDialog::getOpenFileName(0, "*", this);
    if (!name.isNull()) {
      loadLocal(name);
    }
}

void KPaint::fileSave()
{
  c->save(filename, format);

  // If it was remote we need to send it back
  if (url != "") {
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
  //  newfilename= QFileDialog::getSaveFileName(0,
  //					    formatMngr->glob(format),
  //					    this);
  newfilename= QFileDialog::getSaveFileName(0,
  					    "*",
  					    this);

#ifdef KPDEBUG
  fprintf(stderr, ": %s\n",  newfilename.data());
#endif	  

  if (!newfilename.isNull()) {
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename.data());
#endif
	  unlink(filename);
	}
      }
    }


      c->save(newfilename, format);
      statusbar->changeItem(newfilename.data(), ID_FILENAME);

      filename= newfilename;
      url= "";
      modified= false;
    }
}


void KPaint::fileFormat()
{
  formatDialog dlg(format);
#ifdef KPDEBUG
    fprintf(stderr, "fileFormat() %s\n", (const char *) format);
#endif	  
    if (dlg.exec()) {
      fprintf(stderr, "Set format to %s\n",
	      dlg.fileformat->text(dlg.fileformat->currentItem()));

      
      format= dlg.fileformat->text(dlg.fileformat->currentItem());
      filename.replace(QRegExp("\\..+$"), "");
      filename= filename+formatMngr->suffix(format);
      statusbar->changeItem(filename, ID_FILENAME);
    }

}

void KPaint::fileExit()
{
  QString proto;

#ifdef KPDEBUG
    fprintf(stderr, "fileExit()\n");
#endif

    if (exit()) {
      // Delete any temp files from the image
      if (!url.isEmpty()) {
	KURL u(url);
	
	if (!u.isMalformed()) {
	  proto= u.protocol();
	  
	  if (proto != "file") {
#ifdef KPDEBUG
	    fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename.data());
#endif
	    unlink(filename);
	  }
	}
      }
    ::exit(0);
    }
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
  DlgLocation l( klocale->translate("Open Location:"), url, this );

  if ( l.exec() ) {
    QString n = l.getText();

    // Delete any temp files from the previous image
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
#ifdef KPDEBUG
	  fprintf(stderr, "KPaint: Deleting temp file \'%s\'\n", filename.data());
#endif
	  unlink(filename);
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
#ifdef KPDEBUG
  fprintf(stderr, "fileSaveAsURL(): %s\n", (const char *) url);
#endif

  // Get the URL to save to
  DlgLocation l( klocale->translate("Save to Location:"), url, this );

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
    kpaintApp->clipboard_= c->selectionData();
}

void KPaint::editCut()
{
#ifdef KPDEBUG
    fprintf(stderr, "editCut()\n");
#endif	  
    kpaintApp->clipboard_= c->selectionData();
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
  KPaint *kp;

  kp= new KPaint();
  kp->setPixmap(kpaintApp->clipboard_);
  kp->show();
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

  statusbar->changeItem(s, ID_ZOOMFACTOR);

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

  statusbar->changeItem(s, ID_ZOOMFACTOR);

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
  QString aMessageHeader( klocale->translate( "About" ) );
  aMessageHeader + APPNAME;

  QMessageBox::message(aMessageHeader,
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
