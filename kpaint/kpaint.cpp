// $Id$


#include <kdebug.h>
#include <string.h>
#include <qwidget.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include "QwViewport.h"
#include <kkeyconf.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmsgbox.h>
#include <kiconloader.h>
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
#include "depthdialog.h"
#include "formatdialog.h"
#include "formats.h"

#include <unistd.h>
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
  kfm= 0;

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

  initMenus();
  initToolbars();
  initStatus();

  readOptions();

  // Was a URL specified?
  if (url_ != 0) {
    QString u(url_);

    // Is there a ':'?
    int colon= u.find(':');

    if (colon != -1) {
KDEBUG1(KDEBUG_INFO, 3000, "KPaint:: initialising from URL %s\n", url_);
      loadRemote(url_);
    }
    else {
KDEBUG1(KDEBUG_INFO, 3000, "KPaint:: initialising from file %s\n", url_);
      loadLocal(url_);
    }
  }

  if (c->pixmap()->depth() > 8) {
    allowEditPalette= false;
  }
  else {
    allowEditPalette= true;
  }

  updateCommands();
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
KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp \'%s\'\n", filename.data());
	unlink(filename);
      }
    }
  }

  if (openwins == 0)
    kpaintApp->exit(0);
}

void KPaint::initToolbars()
{
  QPixmap pixmap;

  // Create command toolbar
  commandsToolbar= new KToolBar(this);

  pixmap= kapp->getIconLoader()->loadIcon( "filenew.xpm" );
  commandsToolbar->insertButton(pixmap, ID_NEW, TRUE, klocale->translate("New Canvas"));

  pixmap= kapp->getIconLoader()->loadIcon( "fileopen.xpm" );
  commandsToolbar->insertButton(pixmap, ID_OPEN, TRUE, klocale->translate("Open File"));

  pixmap= kapp->getIconLoader()->loadIcon( "filefloppy.xpm" );
  commandsToolbar->insertButton(pixmap, ID_SAVE, TRUE, klocale->translate("Save File"));
  commandsToolbar->insertSeparator();

  pixmap= kapp->getIconLoader()->loadIcon( "editcopy.xpm" );
  commandsToolbar->insertButton(pixmap, ID_COPY, FALSE, klocale->translate("Copy"));

  pixmap= kapp->getIconLoader()->loadIcon( "editcut.xpm" );
  commandsToolbar->insertButton(pixmap, ID_CUT, FALSE, klocale->translate("Cut"));

  pixmap= kapp->getIconLoader()->loadIcon( "editpaste.xpm" );
  commandsToolbar->insertButton(pixmap, ID_PASTE, FALSE, klocale->translate("Paste"));
  commandsToolbar->insertSeparator();

  pixmap= kapp->getIconLoader()->loadIcon( "viewmag+.xpm" );
  commandsToolbar->insertButton(pixmap, ID_ZOOMIN, TRUE, klocale->translate("Zoom In"));

  pixmap= kapp->getIconLoader()->loadIcon( "viewmag-.xpm" );
  commandsToolbar->insertButton(pixmap, ID_ZOOMOUT, TRUE, klocale->translate("Zoom Out"));

  commandsToolbar->show();
  addToolBar(commandsToolbar);

  // Tools toolbar
  toolsToolbar= man->toolbar();
  toolsToolbar->show();
  addToolBar(toolsToolbar);
}

void KPaint::updateCommands()
{
  if (allowEditPalette)
    menu->setItemEnabled(ID_PALETTE, true);
  else
    menu->setItemEnabled(ID_PALETTE, false);
}

void KPaint::readOptions()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  
  config->setGroup( "Appearance" );
  
  // Read the entries
  if (config->readNumEntry("ShowToolsToolBar", 1))
    showToolsToolBar= true;
  else
    showToolsToolBar= false;

  if (config->readNumEntry("ShowCommandsToolBar", 1))
    showCommandsToolBar= true;
  else
    showCommandsToolBar= false;

  if (config->readNumEntry("ShowStatusBar", 1))
    showStatusBar= true;
  else
    showStatusBar= false;

  updateControls();
}

void KPaint::writeOptions()
{
  KConfig *config = KApplication::getKApplication()->getConfig();

  config->setGroup( "Appearance" );
  
  // Write the entries
  if (showToolsToolBar)
    config->writeEntry("ShowToolsToolBar", 1);
  else
    config->writeEntry("ShowToolsToolBar", 0);

  if (showCommandsToolBar)
    config->writeEntry("ShowCommandsToolBar", 1);
  else
    config->writeEntry("ShowCommandsToolBar", 0);

  if (showStatusBar)
    config->writeEntry("ShowStatusBar", 1);
  else
    config->writeEntry("ShowStatusBar", 0);

  config->sync();
}

void KPaint::updateControls()
{
  if (showToolsToolBar) {
    enableToolBar(KToolBar::Show, ID_TOOLSTOOLBAR);
    menu->setItemChecked(ID_SHOWTOOLSTOOLBAR, true);
  }
  else {
    enableToolBar(KToolBar::Hide, ID_TOOLSTOOLBAR);
    menu->setItemChecked(ID_SHOWTOOLSTOOLBAR, false);
  }

  if (showCommandsToolBar) {
    enableToolBar(KToolBar::Show, ID_COMMANDSTOOLBAR);
    menu->setItemChecked(ID_SHOWCOMMANDSTOOLBAR, true);
  }
  else {
    enableToolBar(KToolBar::Hide, ID_COMMANDSTOOLBAR);
    menu->setItemChecked(ID_SHOWCOMMANDSTOOLBAR, false);
  }

  if (showStatusBar) {
    enableStatusBar(KStatusBar::Show);
    menu->setItemChecked(ID_SHOWSTATUSBAR, true);
  }
  else {
    enableStatusBar(KStatusBar::Hide);
    menu->setItemChecked(ID_SHOWSTATUSBAR, false);
  }
}

void KPaint::canvasSizeChanged()
{
  QString size;

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->changeItem(size, ID_FILESIZE);
}

void KPaint::initStatus()
{
  QString size;
  statusbar= new KStatusBar(this);

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->insertItem(size, ID_FILESIZE);
  statusbar->insertItem("100%", ID_ZOOMFACTOR);

  if (url.isEmpty())
    statusbar->insertItem(filename, ID_FILENAME);
  else
    statusbar->insertItem(url, ID_FILENAME);

  statusbar->show();
  setStatusBar(statusbar);
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
  options->insertItem( klocale->translate("Show Tools Toolbar"), ID_SHOWTOOLSTOOLBAR);
  options->insertItem( klocale->translate("Show Commands Toolbar"), ID_SHOWCOMMANDSTOOLBAR);
  options->insertItem( klocale->translate("Show Status Bar"), ID_SHOWSTATUSBAR);
  options->insertItem( klocale->translate("Save Options"), ID_SAVEOPTIONS);

/*
  QPopupMenu *help = new QPopupMenu;
  help->insertItem( klocale->translate("Contents"), ID_HELPCONTENTS);
  help->insertItem( klocale->translate("Release Notes"), ID_RELEASENOTES);
  help->insertItem( klocale->translate("About..."), ID_ABOUT);
*/
  QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Paint"))
                                         + " " + APPVERSTR
                                         + i18n("\n\nby ") + APPAUTHOR 
                                         + "(" 
                                         + APPAUTHOREMAIL 
                                         + ")" );  

  menu = new KMenuBar( this );
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
  case ID_SHOWTOOLSTOOLBAR:
    showToolsToolBar= !showToolsToolBar;
    updateControls();
    break;
  case ID_SHOWCOMMANDSTOOLBAR:
    showCommandsToolBar= !showCommandsToolBar;
    updateControls();
    break;
  case ID_SHOWMENUBAR:
    break;
  case ID_SHOWSTATUSBAR:
    showStatusBar= !showStatusBar;
    updateControls();
    break;
  case ID_SAVEOPTIONS:
    writeOptions();
    break;

/*
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
*/ 
 }
}

bool KPaint::loadLocal(const char *filename_, const char *url_)
{
  QString size;
  const char *fmt;
  bool result;

  // Check the file exists

  fmt= QPixmap::imageFormat(filename_);

  if (fmt != 0) {
    if (c->load(filename_)) {
      result= true;
      format= fmt;
      filename= filename_;
      modified= false;

      if (url_ == 0) {
	url= "";
	statusbar->changeItem(filename.data(), ID_FILENAME);
      }
      else {
	url= url_;
	statusbar->changeItem(url.data(), ID_FILENAME);
      }
      canvasSizeChanged();
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
		     "KPaint does not understand the format of the "
		     "specified file\nor the file is corrupt."),
		     KMsgBox::EXCLAMATION,
		     klocale->translate("Continue"));
    result= false;
  }

  return result;
}

// Initiates fetch of remote file
bool KPaint::loadRemote(const char *url_)
{
  KURL u(url_);

KDEBUG1(KDEBUG_INFO, 3000, "loadRemote: %s\n", url_);
KDEBUG1(KDEBUG_INFO, 3000, "loadRemote: KURL=\"%s\"\n", (const char *) u.url());
KDEBUG1(KDEBUG_INFO, 3000, "loadRemote: protocol=\"%s\"\n", u.protocol());
KDEBUG1(KDEBUG_INFO, 3000, "loadRemote: host=\"%s\"\n", u.host());
KDEBUG1(KDEBUG_INFO, 3000, "loadRemote: path=\"%s\"\n", u.path());

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
KDEBUG1(KDEBUG_INFO, 3000, "Local file:%s\n",u.path());

    return loadLocal(u.path());
  }

  if (kfm != 0) {
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
    kfm = 0;
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
  assert(kfm != 0);

  if (kfmOp == KfmPut)
    KFMputFinished();
  else if (kfmOp == KfmGet)
    KFMgetFinished();
}

void KPaint::KFMgetFinished()
{
  assert(kfm != 0);
  const char *filename_;
  const char *url_;

  // Free the file manager
  delete kfm;
  kfm= 0;

KDEBUG2(KDEBUG_INFO, 3000, "Fetch completed, loading %s as %s...\n", (const char *) filename, (const char *) url);

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
  assert(kfm != 0);

  delete kfm;
  kfm= 0;
}

bool KPaint::saveLocal(const char *filename_, const char *url_)
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
KDEBUG1(KDEBUG_INFO, 3000, "Save file:%s\n", u.path());

    return saveLocal(u.path(), url_);
  }

  if (kfm != 0) {
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
    kfm = 0;
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
  QString proto;
  canvasSizeDialog sz(0, klocale->translate("Canvas Size"));

KDEBUG(KDEBUG_INFO, 3000, "File New\n");

  if (sz.exec()) {
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'\n", filename.data());
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

    canvasSizeChanged();

    repaint(0);
  }
}


void KPaint::fileOpen()
{
  QString name;

KDEBUG(KDEBUG_INFO, 3000, "fileOpen()\n");
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

KDEBUG(KDEBUG_INFO, 3000, "fileSaveAsCommand");

  // get the glob for the current format
  //  newfilename= QFileDialog::getSaveFileName(0,
  //					    formatMngr->glob(format),
  //					    this);
  newfilename= QFileDialog::getSaveFileName(0,
  					    "*",
  					    this);

KDEBUG1(KDEBUG_INFO, 3000, ": %s\n",  newfilename.data());

  if (!newfilename.isNull()) {
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'\n", filename.data());
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
KDEBUG1(KDEBUG_INFO, 3000, "fileFormat() %s\n", (const char *) format);
    if (dlg.exec()) {
KDEBUG1(KDEBUG_INFO, 3000, "Set format to %s\n", dlg.fileformat->text(dlg.fileformat->currentItem()));
      
      format= dlg.fileformat->text(dlg.fileformat->currentItem());
      filename.replace(QRegExp("\\..+$"), "");
      filename= filename+formatMngr->suffix(format);
      statusbar->changeItem(filename, ID_FILENAME);
    }

}

void KPaint::fileExit()
{
  QString proto;

KDEBUG(KDEBUG_INFO, 3000, "fileExit()\n");

  if (exit()) {
    // Delete any temp files from the image
    if (!url.isEmpty()) {
      KURL u(url);
      
      if (!u.isMalformed()) {
	proto= u.protocol();
	
	if (proto != "file") {
	  KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'\n", filename.data());
	  unlink(filename);
	}
      }
    }
    kpaintApp->exit(0);
  }
}

void KPaint::newWindow()
{
KDEBUG(KDEBUG_INFO, 3000, "newWindow()\n");
   KPaint *kp;
   
   kp= new KPaint();
   kp->show();
}

void KPaint::closeWindow()
{
KDEBUG(KDEBUG_INFO, 3000, "closeWindow()\n");

    close();
}

void KPaint::fileOpenURL()
{
  QString proto;

KDEBUG(KDEBUG_INFO, 3000, "fileOpenURL()\n");

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
KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'\n", filename.data());
	  unlink(filename);
	}
      }
    }

    // If the request was sent ok
    if (loadRemote(n)) {
      // Lock this window
KDEBUG(KDEBUG_INFO, 3000, "Lock the window!\n");
    }
  }
}

void KPaint::fileSaveAsURL()
{
KDEBUG1(KDEBUG_INFO, 3000, "fileSaveAsURL(): %s\n", (const char *) url);

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
  KDEBUG(KDEBUG_INFO, 3000, "editCopy()\n");
  kpaintApp->clipboard_= c->selectionData();
}

void KPaint::editCut()
{
  KDEBUG(KDEBUG_INFO, 3000, "editCut()\n");
  kpaintApp->clipboard_= c->selectionData();
}

void KPaint::editPaste()
{
KDEBUG(KDEBUG_INFO, 3000, "editPaste()\n");
}

void KPaint::editPasteImage()
{
  KDEBUG(KDEBUG_INFO, 3000, "editPasteImage()\n");
  KPaint *kp;
  QPixmap *p;

  if ((kpaintApp->clipboard_) != 0) {
    p= new QPixmap(*(kpaintApp->clipboard_));
    CHECK_PTR(p);
    kp= new KPaint();
    kp->setPixmap(p);
    kp->show();
  }
}

void KPaint::editZoomIn()
{
KDEBUG(KDEBUG_INFO, 3000, "editZoomIn()\n");
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
KDEBUG(KDEBUG_INFO, 3000, "editZoomOut()\n");
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
KDEBUG(KDEBUG_INFO, 3000, "editMask()\n");
}

void KPaint::editOptions()
{
KDEBUG(KDEBUG_INFO, 3000, "editOptions()\n");
    kKeys->configureKeys(this);
}
  
// Image
void KPaint::imageInfo()
{
  imageInfoDialog info(c, 0, "Image Information");
KDEBUG(KDEBUG_INFO, 3000, "imageInfo()\n");
  info.exec();
}

void KPaint::imageEditPalette()
{
KDEBUG(KDEBUG_INFO, 3000, "imageEditPalette()\n");
    paletteDialog pal(c->pixmap());

    if (pal.exec()) {
      c->setPixmap(pal.pixmap());
      c->repaint(0);
    }
}

void KPaint::imageChangeDepth()
{
  depthDialog d(c);
  KDEBUG(KDEBUG_INFO, 3000, "imageChangeDepth()\n");
  if (d.exec()) {
fprintf(stderr, "TODO: finish KPaint::imageChangeDepth()");
  }
}

// Tool
void KPaint::setTool(int t)
{
KDEBUG1(KDEBUG_INFO, 3000, "setTool(%d)\n", t);
  if (t > 0)
    man->setCurrentTool(t);
  else 
    man->showPropertiesDialog();
}

void KPaint::toolProperties()
{
KDEBUG(KDEBUG_INFO, 3000, "toolProperties()\n");
  man->showPropertiesDialog();
}


// Help
void KPaint::helpAbout()
{
KDEBUG(KDEBUG_INFO, 3000, "helpAbout()\n");
  QString aMessageHeader( klocale->translate( "About" ) );
  aMessageHeader + APPNAME;

  QMessageBox::about( NULL, kapp->getCaption(), "KPaint" ); 

//  QMessageBox::message(aMessageHeader,
//		       APPVERSTR "\n" APPAUTHOR "\n" APPAUTHOREMAIL);
}

/*
void KPaint::helpContents()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
KDEBUG(KDEBUG_INFO, 3000, "helpContents()\n");
   kpaintApp->invokeHTMLHelp(filename, topic);
}

void KPaint::helpIndex()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
KDEBUG(KDEBUG_INFO, 3000, "helpIndex()\n");

   kpaintApp->invokeHTMLHelp(filename, topic);
}
*/

#include "kpaint.moc"
