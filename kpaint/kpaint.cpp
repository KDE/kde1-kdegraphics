// $Id$

#define USE_KFILEDLG

#include <kdebug.h>
#include <string.h>
#include <qwidget.h>

#ifdef USE_KFILEDLG
#include <kfiledialog.h>
#else
#include <qfiledlg.h>
#endif

#include <qmsgbox.h>
#include <qlayout.h>
#include "QwViewport.h"
#include <kaccel.h>
#include <kkeydialog.h>
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
#include "view.h"
#include "sidebar.h"

#include <unistd.h>
#include <time.h>

extern FormatManager *formatMngr;
extern int openwins;

KPaint::KPaint(const char *url) : KTopLevelWidget()
{
  int w, h; 

  modified= false;
  filename= i18n("untitled.gif");
  format= "GIF";

  w= 300;
  h= 200;

  keys = new KAccel(this);

  View *view= new View(this, "view container");
  v= new QwViewport(view);
  c= new Canvas(w, h, v->portHole());

  v->resize(c->size());

  setView(view);

  man= new Manager(c);
  connect(c, SIGNAL(sizeChanged()), v, SLOT(resizeScrollBars()));
  connect(c, SIGNAL(modified()), this, SLOT(updateCommands()));
  SideBar *side= new SideBar(c, view, "sidebar");
  connect(c, SIGNAL(modified()), side, SLOT(pixmapChanged()));

  view->c= v;
  view->s= side;

  connect(side, SIGNAL(lmbColourChanged(const QColor &)),
	  man, SLOT(setLMBcolour(const QColor &)));
  connect(side, SIGNAL(rmbColourChanged(const QColor &)),
	  man, SLOT(setRMBcolour(const QColor &)));

  zoom= 100;
  openwins++;

  initMenus();
  initToolbars();
  initStatus();

  readOptions();

  if (url != 0)
    loadRemote(url);

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
  // TODO: Fix setPixmap()
  format= "GIF";
  filename= "Untitled.gif";
  modified= false;
  url= "";
  c->setPixmap(p);
}

int KPaint::exit()
{
  int die= 0;

  if (!(c->isModified()))
    die= 1;
  else
    if (QMessageBox::warning(this, i18n("Unsaved Changes"),
			     i18n("You have unsaved changes, you "
				  "will loose them if you close "
				  "this window now."),
			      i18n("Close"), i18n("Cancel"),
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
  if (exit()) {
    e->accept();
    openwins--;
  }

  if (openwins == 0)
    myapp->exit(0);
}

void KPaint::initToolbars()
{
  QPixmap pixmap;

  // Create command toolbar
  commandsToolbar= new KToolBar(this);
  //  commandsToolbar= toolBar(ID_COMMANDSTOOLBAR);

  pixmap= Icon( "filenew.xpm" );
  commandsToolbar->insertButton(pixmap, ID_NEW, true, i18n("New Canvas"));

  pixmap= Icon( "fileopen.xpm" );
  commandsToolbar->insertButton(pixmap, ID_OPEN, true, i18n("Open File"));

  pixmap= Icon( "filefloppy.xpm" );
  commandsToolbar->insertButton(pixmap, ID_SAVE, true, i18n("Save File"));
  commandsToolbar->insertSeparator();

  pixmap= Icon( "editcopy.xpm" );
  commandsToolbar->insertButton(pixmap, ID_COPY, false, i18n("Copy"));

  pixmap= Icon( "editcut.xpm" );
  commandsToolbar->insertButton(pixmap, ID_CUT, false, i18n("Cut"));

  pixmap= Icon( "editpaste.xpm" );
  commandsToolbar->insertButton(pixmap, ID_PASTE, false, i18n("Paste"));
  commandsToolbar->insertSeparator();

  pixmap= Icon( "viewmag+.xpm" );
  commandsToolbar->insertButton(pixmap, ID_ZOOMIN, true, i18n("Zoom In"));

  pixmap= Icon( "viewmag-.xpm" );
  commandsToolbar->insertButton(pixmap, ID_ZOOMOUT, true, i18n("Zoom Out"));

  commandsToolbar->show();
  addToolBar(commandsToolbar, ID_COMMANDSTOOLBAR);
  commandsToolbar->setFullWidth(false);

  connect (commandsToolbar, SIGNAL (clicked (int)),
	   SLOT (handleCommand (int)));

  // Tools toolbar
  //  toolsToolbar= man->toolbar();
  //  toolsToolbar= toolBar(ID_TOOLSTOOLBAR);
  toolsToolbar= new KToolBar(this);
  man->populateToolbar(toolsToolbar);
  toolsToolbar->show();
  addToolBar(toolsToolbar, ID_TOOLSTOOLBAR);
  toolsToolbar->setFullWidth(false);

  connect (toolsToolbar, SIGNAL (clicked (int)),
	   SLOT (handleCommand (int)));
}

void KPaint::updateCommands()
{
  if (allowEditPalette)
    menu->setItemEnabled(ID_PALETTE, true);
  else
    menu->setItemEnabled(ID_PALETTE, false);

  if (c->isModified()) {
    menu->setItemEnabled(ID_SAVE, true);
  }
  else {
    menu->setItemEnabled(ID_SAVE, false);
  }

  menu->setItemEnabled(ID_MASK, false);
}

void KPaint::readOptions()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  
  config->setGroup( "Appearance" );

  // Read the entries
  showToolsToolBar= config->readBoolEntry("ShowToolsToolBar", true);
  showCommandsToolBar= config->readBoolEntry("ShowCommandsToolBar", true);
  showStatusBar= config->readBoolEntry("ShowStatusBar", true);

  updateControls();
}

void KPaint::writeOptions()
{
  KConfig *config = KApplication::getKApplication()->getConfig();

  config->setGroup( "Appearance" );

  // Write the entries
  config->writeEntry("ShowToolsToolBar", showToolsToolBar);
  config->writeEntry("ShowCommandsToolBar", showCommandsToolBar);
  config->writeEntry("ShowStatusBar", showStatusBar);
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
  setStatusBar(statusbar);

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->insertItem(size, ID_FILESIZE);
  statusbar->insertItem("100%", ID_ZOOMFACTOR);

  if (url.isEmpty())
    statusbar->insertItem(filename, ID_FILENAME);
  else
    statusbar->insertItem(url, ID_FILENAME);

  statusbar->show();
}

void KPaint::initMenus()
{
  QPopupMenu *file;
  
  file = new QPopupMenu;

  file->insertItem(i18n("Open Image..."), ID_OPEN);
  file->insertItem(i18n("New Image..."), ID_NEW);
  file->insertItem(i18n("Save Image"), ID_SAVE);
  file->insertItem(i18n("Save Image As..."), ID_SAVEAS);
  file->insertItem(i18n("Image Format..."), ID_FORMAT);
  file->insertSeparator();
  file->insertItem(i18n("Open from URL..."), ID_OPENURL);
  file->insertItem(i18n("Save to URL..."), ID_SAVEURL);
  file->insertSeparator();
  file->insertItem(i18n("New Window"), ID_NEWWINDOW);
  file->insertItem(i18n("Close Window"), ID_CLOSEWINDOW);
  file->insertSeparator();
  file->insertItem(i18n("Exit"), ID_EXIT);

  QPopupMenu *edit= new QPopupMenu;
  edit->insertItem(i18n("Copy Region"), ID_COPY);
  edit->insertItem(i18n("Cut Region"), ID_CUT);
  edit->insertItem(i18n("Paste Region"), ID_PASTE);
  edit->insertItem(i18n("Paste As Image"), ID_PASTEIMAGE);
  edit->insertSeparator();
  edit->insertItem(i18n("Zoom In"), ID_ZOOMIN);
  edit->insertItem(i18n("Zoom Out"), ID_ZOOMOUT);
  edit->insertItem(i18n("Mask..."), ID_MASK);

  QPopupMenu *image= new QPopupMenu;
  image->insertItem(i18n("Information..."), ID_INFO);
  image->insertItem(i18n("Resize..."), ID_RESIZEIMAGE);
  image->insertItem(i18n("Edit Palette..."), ID_PALETTE);
  image->insertItem(i18n("Change Colour Depth..."), ID_DEPTH);

  QPopupMenu *tool= new QPopupMenu;
  tool->insertItem( i18n("Tool Properties..."), -1);
  tool->insertSeparator();
  tool->insertItem( i18n("Pen"), 2);
  tool->insertItem( i18n("Line"), 4);
  tool->insertItem( i18n("Rectangle"), 3);
  tool->insertItem( i18n("Ellipse"), 0 );
  tool->insertItem( i18n("Circle"), 1 );
  tool->insertItem( i18n("Spray Can"), 5);
  tool->insertItem( i18n("Area Select"), 6);
  tool->insertItem( i18n("Round Angle"), 7);
  connect(tool, SIGNAL(activated(int)), SLOT(setTool(int)));

  QPopupMenu *options = new QPopupMenu;
  options->setCheckable(true);
  options->insertItem( i18n("Show Tools Toolbar"), ID_SHOWTOOLSTOOLBAR);
  options->insertItem( i18n("Show Commands Toolbar"), ID_SHOWCOMMANDSTOOLBAR);
  options->insertItem( i18n("Show Status Bar"), ID_SHOWSTATUSBAR);
  options->insertItem( i18n("Save Options"), ID_SAVEOPTIONS);

/*
  QPopupMenu *help = new QPopupMenu;
  help->insertItem( i18n("Contents"), ID_HELPCONTENTS);
  help->insertItem( i18n("Release Notes"), ID_RELEASENOTES);
  help->insertItem( i18n("About..."), ID_ABOUT);
*/
  QPopupMenu *help = kapp->getHelpMenu(false, QString("        ")
				       + i18n("KPaint") +
                                         + " " + APPVERSTR
                                         + i18n("\n\n(c) ") + APPAUTHOR 
                                         + "(" 
                                         + APPAUTHOREMAIL 
                                         + ")\n\n" + APPLICENSE );  

  menu = new KMenuBar( this );

  menu->insertItem( i18n("&File"), file );
  menu->insertItem( i18n("&Edit"), edit );
  menu->insertItem( i18n("&Image"), image );
  menu->insertItem( i18n("&Tool"), tool );
  menu->insertItem( i18n("&Options"), options );
  menu->insertSeparator();
  menu->insertItem( i18n("&Help"), help );
  setMenu(menu);
  menu->show();

  connect (file, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (edit, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (image, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (options, SIGNAL (activated (int)), SLOT (handleCommand (int)));
  connect (help, SIGNAL (activated (int)), SLOT (handleCommand (int)));
}

void KPaint::handleCommand(int command)
{
  switch (command) {
    // File
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
    // Image
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
  case ID_RESIZEIMAGE:
    imageResize();
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

  // Check the file exists and is in a known format
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
      KMsgBox::message(0, i18n("KPaint: Could not open file"),
		       i18n("Error: Could not open file\n"
		       "KPaint could not open the specified file."),
		       KMsgBox::EXCLAMATION,
		       i18n("Continue"));
      result= false;
    }
  }
  else {
    KMsgBox::message(0, i18n("KPaint: Unknown Format"),
		     i18n("Error: Unknown Format\n"
		     "KPaint does not understand the format of the "
		     "specified file\nor the file is corrupt."),
		     KMsgBox::EXCLAMATION,
		     i18n("Continue"));
    result= false;
  }

  return result;
}

// Initiates fetch of remote file
bool KPaint::loadRemote(const char *url_)
{
  QString filename;
  QString url;
  bool result;

  url= url_;
  result= KFM::download(url, filename);

  if (result) {
    loadLocal(filename, url);
    KFM::removeTempFile(filename);
  }

  return result;
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
  // Temporarily removed waiting for kfmlib changes
  return false;
}

void KPaint::fileNew()
{
  int w, h;
  QPixmap p;
  QString proto;
  canvasSizeDialog sz(0, "canvassizedlg");

  KDEBUG(KDEBUG_INFO, 3000, "File New\n");

  if (sz.exec()) {
    w= sz.getWidth();
    h= sz.getHeight();
    p.resize(w, h);
    p.fill(QColor("white"));
    c->setPixmap(&p);
    man->setCurrentTool(0);
    filename= i18n("untitled.gif");
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

#ifdef USE_KFILEDLG
  name=KFileDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
  //  name=KFilePreviewDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
#else
  name=QFileDialog::getOpenFileName(0, "*", this);
#endif
  if (!name.isNull()) {
    loadRemote(name);
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

#ifdef USE_KFILEDLG
  newfilename= KFileDialog::getSaveFileURL(0,
					   formatMngr->glob(format),
					   this);
#else
  newfilename= QFileDialog::getSaveFileName(0,
  					    formatMngr->glob(format),
  					    this);
#endif
  if (!newfilename.isNull()) {
    KDEBUG1(KDEBUG_INFO, 3000, ": %s\n",  newfilename.data());  
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
KDEBUG1(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'", filename.data());
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
KDEBUG1(KDEBUG_INFO, 3000, "fileFormat() %s", (const char *) format);
    if (dlg.exec()) {
KDEBUG1(KDEBUG_INFO, 3000, "Set format to %s", dlg.fileformat->text(dlg.fileformat->currentItem()));
      
      format= dlg.fileformat->text(dlg.fileformat->currentItem());
      int i = filename.findRev('.');
      if (i > 0)
	filename = filename.left(i+1);
      filename= filename+formatMngr->suffix(format);
      statusbar->changeItem(filename, ID_FILENAME);
    }

}

void KPaint::fileExit()
{
  QString proto;

KDEBUG(KDEBUG_INFO, 3000, "fileExit()\n");

  if (exit()) {
    myapp->exit(0);
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
  if (exit())
    close();
}

void KPaint::fileOpenURL()
{
  QString proto;

  KDEBUG(KDEBUG_INFO, 3000, "fileOpenURL()\n");

#ifdef USE_KFILEDLG
  QString n= KFileDialog::getOpenFileURL(0,
					 formatMngr->allImagesGlob(),
					 this);
  if (!n.isNull()) {
#else
  // Get the URL to open
  DlgLocation l( i18n("Open Location:"), url, this );

  if ( l.exec() ) {
    QString n = l.getText();
#endif
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

#ifdef USE_KFILEDLG
  QString n= KFileDialog::getSaveFileURL(0,
					 formatMngr->glob(format),
					 this);
  if (!n.isNull()) {
#else
  // Get the URL to save to
  DlgLocation l( i18n("Save to Location:"), url, this );

  if ( l.exec() ) {
    QString n = l.getText();
#endif

    saveRemote(n);
  }
}

// Edit
void KPaint::editCopy()
{
  KDEBUG(KDEBUG_INFO, 3000, "editCopy()\n");
  myapp->clipboard_= c->selectionData();
}

void KPaint::editCut()
{
  KDEBUG(KDEBUG_INFO, 3000, "editCut()\n");
  myapp->clipboard_= c->selectionData();
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

  if ((myapp->clipboard_) != 0) {
    p= new QPixmap(*(myapp->clipboard_));
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
    KKeyDialog::configureKeys(keys);
}
  
// Image
void KPaint::imageInfo()
{
  imageInfoDialog info(c, 0, "Image Information");
KDEBUG(KDEBUG_INFO, 3000, "imageInfo()\n");
  info.exec();
}

void KPaint::imageResize()
{
  KDEBUG(KDEBUG_INFO, 3000, "imageResize()\n");
  canvasSizeDialog sz(this);
  if (sz.exec()) {
    int w= sz.getWidth();
    int h= sz.getHeight();
    c->resizeImage(w, h);
    canvasSizeChanged();
  }
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
    switch (d.depthBox->currentItem()) {
    case 0:
      KDEBUG(KDEBUG_INFO, 3000, "setDepth to 1\n");
      c->setDepth(1);
      allowEditPalette= false;
      break;
    case 1:
      KDEBUG(KDEBUG_INFO, 3000, "setDepth to 8\n");
      c->setDepth(8);
      allowEditPalette= true;
      break;
    case 2:
      KDEBUG(KDEBUG_INFO, 3000, "setDepth to 32\n");
      c->setDepth(32);
      allowEditPalette= false;
      break;
    default:
      break;
    }
    // This is broken
    //    updateCommands();
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
  QString aMessageHeader( i18n( "About" ) );
  aMessageHeader + APPNAME;

  QMessageBox::about( 0, kapp->getCaption(), "KPaint" ); 

//  QMessageBox::message(aMessageHeader,
//		       APPVERSTR "\n" APPAUTHOR "\n" APPAUTHOREMAIL);
}

/*
void KPaint::helpContents()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
KDEBUG(KDEBUG_INFO, 3000, "helpContents()\n");
   myapp->invokeHTMLHelp(filename, topic);
}

void KPaint::helpIndex()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
KDEBUG(KDEBUG_INFO, 3000, "helpIndex()\n");

   myapp->invokeHTMLHelp(filename, topic);
}
*/

#include "kpaint.moc"
