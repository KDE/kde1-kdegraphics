// $Id$

#include <kdebug.h>
#include <string.h>
#include <qwidget.h>

#include <kfiledialog.h>

#include <qmsgbox.h>
#include <qlayout.h>
#include "QwViewport.h"
/* obsolet (jha)
   #include <kaccel.h>
   #include <kkeydialog.h> */
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

#define max(x,y) (x>y?x:y)


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

/* obsolet (jha)
   keys = new KAccel(this); */

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
  //  commandsToolbar= toolBar(ID_COMMANDS_TOOLBAR);

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
  commandsToolbar->insertButton(pixmap, ID_ZOOM_IN, true, i18n("Zoom In"));

  pixmap= Icon( "viewmag-.xpm" );
  commandsToolbar->insertButton(pixmap, ID_ZOOM_OUT, true, i18n("Zoom Out"));

  commandsToolbar->show();
  addToolBar(commandsToolbar, ID_COMMANDS_TOOLBAR);
  commandsToolbar->setFullWidth(false);

  connect (commandsToolbar, SIGNAL (clicked (int)),
	   SLOT (handleCommand (int)));

  // Tools toolbar
  //  toolsToolbar= man->toolbar();
  //  toolsToolbar= toolBar(ID_TOOLSTOOLBAR);
  toolsToolbar= new KToolBar(this);
  man->populateToolbar(toolsToolbar);
  toolsToolbar->show();
  addToolBar(toolsToolbar, ID_TOOLS_TOOLBAR);
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
    enableToolBar(KToolBar::Show, ID_TOOLS_TOOLBAR);
    menu->setItemChecked(ID_SHOW_TOOLS_TOOLBAR, true);
  }
  else {
    enableToolBar(KToolBar::Hide, ID_TOOLS_TOOLBAR);
    menu->setItemChecked(ID_SHOW_TOOLS_TOOLBAR, false);
  }

  if (showCommandsToolBar) {
    enableToolBar(KToolBar::Show, ID_COMMANDS_TOOLBAR);
    menu->setItemChecked(ID_SHOW_COMMANDS_TOOLBAR, true);
  }
  else {
    enableToolBar(KToolBar::Hide, ID_COMMANDS_TOOLBAR);
    menu->setItemChecked(ID_SHOW_COMMANDS_TOOLBAR, false);
  }

  if (showStatusBar) {
    enableStatusBar(KStatusBar::Show);
    menu->setItemChecked(ID_SHOW_STATUSBAR, true);
  }
  else {
    enableStatusBar(KStatusBar::Hide);
    menu->setItemChecked(ID_SHOW_STATUSBAR, false);
  }
}

void KPaint::canvasSizeChanged()
{
  QString size;

  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->changeItem(size, ID_FILE_SIZE);
}

void KPaint::initStatus()
{
  QString size;
  //  QFontMetrics fm; (below)
  ToolList tl;
  Tool *t, *maxt;
  uint maxtlen= 0, tmp;

  statusbar= new KStatusBar(this);
  setStatusBar(statusbar);

  /* write the image size */
  size.sprintf("%d x %d", c->width(), c->height());
  statusbar->insertItem(size, ID_FILE_SIZE);

  /* write the color depth */
  size.sprintf(" %d bpp", c->getDepth());
  statusbar->insertItem(size, ID_COLOR_DEPTH);

  /* write the zoomfactor */
  statusbar->insertItem("100%", ID_ZOOM_FACTOR);

  /* write the toolname */
  /* get the max. font length of the toolnames */
  tl = man->getToolList();
  QFontMetrics fm = statusbar->fontMetrics();
  for (t= tl.first(); t != NULL; t= tl.next()) {
    tmp = fm.width(t->getName());
    /* check the tool with the max. name len */
    if ( maxtlen < tmp ) {
      maxtlen = tmp;
      maxt = t;
    }
  }
  /* write the maxlen toolname */
  size = "";
  size += maxt->getName();
  statusbar->insertItem(size, ID_TOOL_NAME);

  /* write the filename */
  if (url.isEmpty())
    statusbar->insertItem(filename, ID_FILE_NAME);
  else
    statusbar->insertItem(url, ID_FILE_NAME);

  /* update to the current toolname */
  statusbar->changeItem(man->getCurrentTool().getName(), ID_TOOL_NAME);
  man->setStatusBar(statusbar);

  statusbar->show();
}

void KPaint::initMenus()
{
  ToolList tl;
  Tool *t;
  QPopupMenu *file;
  
  file = new QPopupMenu;

  file->insertItem(i18n("Open Image..."), ID_OPEN);
  file->insertItem(i18n("New Image..."), ID_NEW);
  file->insertItem(i18n("Save Image"), ID_SAVE);
  file->insertItem(i18n("Save Image As..."), ID_SAVE_AS);
  file->insertItem(i18n("Image Format..."), ID_FORMAT);
  file->insertSeparator();
  file->insertItem(i18n("Open from URL..."), ID_OPEN_URL);
  file->insertItem(i18n("Save to URL..."), ID_SAVE_URL);
  file->insertSeparator();
  file->insertItem(i18n("New Window"), ID_NEW_WINDOW);
  file->insertItem(i18n("Close Window"), ID_CLOSE_WINDOW);
  file->insertSeparator();
  file->insertItem(i18n("E&xit"), ID_EXIT);

  QPopupMenu *edit= new QPopupMenu;
  edit->insertItem(i18n("Copy Region"), ID_COPY);
  edit->insertItem(i18n("Cut Region"), ID_CUT);
  edit->insertItem(i18n("Paste Region"), ID_PASTE);
  edit->insertItem(i18n("Paste As Image"), ID_PASTE_IMAGE);
  edit->insertSeparator();
  edit->insertItem(i18n("Zoom In"), ID_ZOOM_IN);
  edit->insertItem(i18n("Zoom Out"), ID_ZOOM_OUT);
  edit->insertItem(i18n("Mask..."), ID_MASK);

  QPopupMenu *image= new QPopupMenu;
  image->insertItem(i18n("Information..."), ID_INFO);
  image->insertItem(i18n("Resize..."), ID_RESIZE_IMAGE);
  image->insertItem(i18n("Edit Palette..."), ID_PALETTE);
  image->insertItem(i18n("Change Colour Depth..."), ID_DEPTH);

  QPopupMenu *tool= new QPopupMenu;
  tool->insertItem( i18n("Tool Properties..."), -2);
  tool->insertSeparator();
  /* insert Tools */
  tl = man->getToolList();
  for (t= tl.first(); t != NULL; t= tl.next()) {
    tool->insertItem(t->getName(), t->id);
  }
  /*  
  tool->insertItem( , 2);
  tool->insertItem(, 4);
  tool->insertItem(, 3);
  tool->insertItem(, 0 );
  tool->insertItem(, 1 );
  tool->insertItem(, 5);
  tool->insertItem( , 6);
  tool->insertItem( i18n("Round Angle"), 7);
  */
  connect(tool, SIGNAL(activated(int)), SLOT(setTool(int)));

  QPopupMenu *options = new QPopupMenu;
  options->setCheckable(true);
  options->insertItem( i18n("Show Tools Toolbar"), ID_SHOW_TOOLS_TOOLBAR);
  options->insertItem( i18n("Show Commands Toolbar"), ID_SHOW_COMMANDS_TOOLBAR);
  options->insertItem( i18n("Show Status Bar"), ID_SHOW_STATUSBAR);
  options->insertItem( i18n("Save Options"), ID_SAVE_OPTIONS);

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
  case ID_SAVE_AS:
    fileSaveAs();
    break;
  case ID_FORMAT:
    fileFormat();
    break;
  case ID_OPEN_URL:
    fileOpenURL();
    break;
  case ID_SAVE_URL:
    fileSaveAsURL();
    break;
  case ID_NEW_WINDOW:
    newWindow();
    break;
  case ID_CLOSE_WINDOW:
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
  case ID_PASTE_IMAGE:
    editPasteImage();
    break;
    // Image
  case ID_ZOOM_IN:
    editZoomIn();
    break;
  case ID_ZOOM_OUT:
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
  case ID_RESIZE_IMAGE:
    imageResize();
    break;
  case ID_PALETTE:
    imageEditPalette();
    break;
  case ID_DEPTH:
    imageChangeDepth();
    break;

    // Options
  case ID_SHOW_TOOLS_TOOLBAR:
    showToolsToolBar= !showToolsToolBar;
    updateControls();
    break;
  case ID_SHOW_COMMANDS_TOOLBAR:
    showCommandsToolBar= !showCommandsToolBar;
    updateControls();
    break;
  case ID_SHOW_MENUBAR:
    break;
  case ID_SHOW_STATUSBAR:
    showStatusBar= !showStatusBar;
    updateControls();
    break;
  case ID_SAVE_OPTIONS:
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
	statusbar->changeItem(filename.data(), ID_FILE_NAME);
      }
      else {
	url= url_;
	statusbar->changeItem(url.data(), ID_FILE_NAME);
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
bool KPaint::saveRemote(const char * /*url_ (temporarily unused) */ )
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

  kdebug(KDEBUG_INFO, 3000, "File New");

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

    statusbar->changeItem(filename.data(), ID_FILE_NAME);

    canvasSizeChanged();

    repaint(0);
  }
}


void KPaint::fileOpen()
{
  QString name;

  kdebug(KDEBUG_INFO, 3000, "fileOpen()");

  name=KFileDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
  //name=KFilePreviewDialog::getOpenFileName(0, formatMngr->allImagesGlob(), this);
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

  kdebug(KDEBUG_INFO, 3000, "fileSaveAsCommand");

  newfilename= KFileDialog::getSaveFileURL(0,
					   formatMngr->glob(format),
					   this);
  if (!newfilename.isNull()) {
    kdebug(KDEBUG_INFO, 3000, ": %s", newfilename.data());
    if (!url.isEmpty()) {
      KURL u(url);

      if (!u.isMalformed()) {
	proto= u.protocol();

	if (proto != "file") {
          kdebug(KDEBUG_INFO, 3000, "KPaint: Deleting temp file \'%s\'", filename.data());
	  unlink(filename);
	}
      }
    }


      c->save(newfilename, format);
      statusbar->changeItem(newfilename.data(), ID_FILE_NAME);

      filename= newfilename;
      url= "";
      modified= false;
    }
}


void KPaint::fileFormat()
{
  formatDialog dlg(format);
     kdebug(KDEBUG_INFO, 3000, "fileFormat() %s", (const char *) format);
    if (dlg.exec()) {
      kdebug(KDEBUG_INFO, 3000, "Set format to %s", dlg.fileformat->text(dlg.fileformat->currentItem()));

      format= dlg.fileformat->text(dlg.fileformat->currentItem());
      int i = filename.findRev('.');
      if (i > 0)
	filename = filename.left(i+1);
      filename= filename+formatMngr->suffix(format);
      statusbar->changeItem(filename, ID_FILE_NAME);
    }

}

void KPaint::fileExit()
{
  QString proto;

  kdebug(KDEBUG_INFO, 3000, "fileExit()");

  if (exit()) {
    myapp->exit(0);
  }
}

void KPaint::newWindow()
{
   kdebug(KDEBUG_INFO, 3000, "newWindow()");
   KPaint *kp;
   
   kp= new KPaint();
   kp->show();
}

void KPaint::closeWindow()
{
  kdebug(KDEBUG_INFO, 3000, "closeWindow()");
  if (exit())
    close();
}

void KPaint::fileOpenURL()
{
  QString proto;

  kdebug(KDEBUG_INFO, 3000, "fileOpenURL()");

  QString n= KFileDialog::getOpenFileURL(0,
					 formatMngr->allImagesGlob(),
					 this);
  if (!n.isNull()) {
    // If the request was sent ok
    if (loadRemote(n)) {
      // Lock this window
      kdebug(KDEBUG_INFO, 3000, "Lock the window!");
    }
  }
}

void KPaint::fileSaveAsURL()
{
  kdebug(KDEBUG_INFO, 3000, "fileSaveAsURL(): %s", (const char *) url);

  QString n= KFileDialog::getSaveFileURL(0,
					 formatMngr->glob(format),
					 this);
  if (!n.isNull()) {
    saveRemote(n);
  }
}

// Edit
void KPaint::editCopy()
{
  kdebug(KDEBUG_INFO, 3000, "editCopy()");
  myapp->clipboard_= c->selectionData();
}

void KPaint::editCut()
{
  kdebug(KDEBUG_INFO, 3000, "editCut()");
  myapp->clipboard_= c->selectionData();
}

void KPaint::editPaste()
{
  kdebug(KDEBUG_INFO, 3000, "editPaste()");
}

void KPaint::editPasteImage()
{
  kdebug(KDEBUG_INFO, 3000, "editPasteImage()");
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
  kdebug(KDEBUG_INFO, 3000, "editZoomIn()");
  if (zoom >= 100) {
    zoom += 100;
    if (zoom > 1000)
      zoom= 1000;
  }
  else { // if (zoom < 100)
    zoom += 10;
  }
  c->setZoom(zoom);

  QString zoomstr;
  //  char *s;

  zoomstr.setNum(zoom);
  zoomstr += '%';
  //  zoomstr.append("%");
  //  s= strdup(zoomstr);

  statusbar->changeItem((const char *)zoomstr, ID_ZOOM_FACTOR);

  //  free(s);
}

void KPaint::editZoomOut()
{
  kdebug(KDEBUG_INFO, 3000, "editZoomOut()");
  if (zoom > 100) {
    zoom -= 100;
  }
  else { // if (zoom <= 100)
    zoom -= 10;
    if (zoom == 0)
      zoom= 10;
  }
  c->setZoom(zoom);

  QString zoomstr;
  //  char *s;

  zoomstr.setNum(zoom);
  zoomstr += '%';
  //  zoomstr.append("%");
  //  s= strdup(zoomstr);

  statusbar->changeItem((const char *)zoomstr, ID_ZOOM_FACTOR);

  //  free(s);
}

void KPaint::editMask()
{
    kdebug(KDEBUG_INFO, 3000, "editMask()");
}

void KPaint::editOptions()
{
    kdebug(KDEBUG_INFO, 3000, "editOptions()");
/* obsolet (jha)
    KKeyDialog::configureKeys(keys); */
}
  
// Image
void KPaint::imageInfo()
{
  imageInfoDialog info(c, 0, "Image Information");
  kdebug(KDEBUG_INFO, 3000, "imageInfo()");
  info.exec();
}

void KPaint::imageResize()
{
  kdebug(KDEBUG_INFO, 3000, "imageResize()");
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
    kdebug(KDEBUG_INFO, 3000, "imageEditPalette()");
    paletteDialog pal(c->pixmap());

    if (pal.exec()) {
      c->setPixmap(pal.pixmap());
      c->repaint(0);
    }
}

void KPaint::imageChangeDepth()
{
  QString depthstr;
  KStatusBar *sb = statusBar();
  depthDialog d(c);

  kdebug(KDEBUG_INFO, 3000, "imageChangeDepth()");
  if (d.exec()) {
    switch (d.depthBox->currentItem()) {
    case ID_COLOR_1:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 1");
      c->setDepth(1);
      depthstr.sprintf(" %d bpp", 1);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;
    case ID_COLOR_4:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 4");
      c->setDepth(4);
      depthstr.sprintf(" %d bpp", 4);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;
    case ID_COLOR_8:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 8");
      c->setDepth(8);
      depthstr.sprintf(" %d bpp", 8);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= true;
      break;
    case ID_COLOR_15:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 15");
      c->setDepth(15);
      depthstr.sprintf(" %d bpp", 15);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;
    case ID_COLOR_16:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 16");
      c->setDepth(16);
      depthstr.sprintf(" %d bpp", 16);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;
    case ID_COLOR_24:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 24");
      c->setDepth(24);
      depthstr.sprintf(" %d bpp", 24);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;
    case ID_COLOR_32:
      kdebug(KDEBUG_INFO, 3000, "setDepth to 32");
      c->setDepth(32);
      depthstr.sprintf("%d bpp", 32);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
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
  kdebug(KDEBUG_INFO, 3000, "setTool(%d)", t);
  if (t >= 0)
    man->setCurrentTool(t);
  else 
    man->showPropertiesDialog();
}

void KPaint::toolProperties()
{
  kdebug(KDEBUG_INFO, 3000, "toolProperties()");
  man->showPropertiesDialog();
}


// Help
void KPaint::helpAbout()
{
  kdebug(KDEBUG_INFO, 3000, "helpAbout()");
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
   kdebug(KDEBUG_INFO, 3000, "helpContents()");
   myapp->invokeHTMLHelp(filename, topic);
}

void KPaint::helpIndex()
{
   QString filename(APPNAME "/" APPNAME ".html" );
   QString topic;
   kdebug(KDEBUG_INFO, 3000, "helpIndex()");

   myapp->invokeHTMLHelp(filename, topic);
}
*/

#include "kpaint.moc"
