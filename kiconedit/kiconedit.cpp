/*
    kdraw - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include "debug.h"
#include "kiconedit.h"
#include "main.h"
#include "pics/logo.xpm"

KIconEdit::KIconEdit(const char *name, const char *xpm)
 : KTopLevelWidget(name)
{
  initMetaObject();
  setCaption(kapp->getCaption());

  if(!windowList)
  {
    debug("KIconEdit::KIconEdit() Globalizing windowlist");
    windowList = (WindowList*)memberList;
  }

  kiprops = new KIconEditProperties(this);
  CHECK_PTR(kiprops);

  menubar = 0L;
  toolbar = 0L;
  drawtoolbar = 0L;
  statusbar = 0L;
  mainview = 0L;
  winwidth = winheight = 0;

  recentlist = new QStrList(true);
  CHECK_PTR(recentlist);
  recentlist->setAutoDelete(true);

  readConfig();

  msgtimer = new QTimer(this);
  CHECK_PTR(msgtimer);
  connect( msgtimer, SIGNAL(timeout()), SLOT(slotClearStatusMessage()));

#ifdef KWIZARD_VERSION
  icontemplates = new KIconTemplateContainer;
  CHECK_PTR(icontemplates);
  debug("Templates created");
#endif

  mainview = new QWidget(this); //KIconEditView(this);
  CHECK_PTR(mainview);
  debug("mainview created");
  l = new QHBoxLayout(mainview);
  toolsw = new KIconToolsView(mainview);
  CHECK_PTR(toolsw);
  debug("Toolsview created");
  //QWidget *gridview = new QWidget(mainview);

  //viewport = new QwViewport(mainview);
  viewport = new QScrollView(mainview);
  CHECK_PTR(viewport);

  QPixmap pmlogo((const char**)logo);
  viewport->viewport()->setBackgroundPixmap(pmlogo);
  viewport->setMouseTracking(true);
  grid = new KIconEditGrid; //viewport->viewport(), 0);
  CHECK_PTR(grid);
  viewport->addChild(grid);
  debug("Grid created");
  grid->setGrid(showgrid);
  debug("Grid->setGrid done");
  grid->setCellSize(gridscaling);
  debug("Grid->setCellSize done");
  //toolsw->setPreview(grid->pixmap());
  setMinimumHeight(toolsw->sizeHint().height());

  l->addWidget(viewport);
  l->addWidget(toolsw);
  l->activate();
  debug("Layout activated");

  icon = new KIcon(this, &grid->image());
  CHECK_PTR(icon);

  dropzone = new KDNDDropZone( this, DndURL);
  CHECK_PTR(dropzone);
  connect( dropzone, SIGNAL( dropAction( KDNDDropZone *) ), 
    this, SLOT( slotDropEvent( KDNDDropZone *) ) );

  setupMenuBar();
  setupToolBar();
  setupDrawToolBar();
  setupStatusBar();

  connect( icon, SIGNAL( saved()),
           SLOT(slotSaved()));
  connect( icon, SIGNAL( loaded(QImage *)),
     grid, SLOT(load(QImage *)));
  connect( icon, SIGNAL(opennewwin(const char *)),
           SLOT(slotNewWin(const char *)));
  connect(icon, SIGNAL(newname(const char *)),
           SLOT( slotUpdateStatusName(const char *)));
  connect(icon, SIGNAL(newmessage(const char *)),
           SLOT( slotUpdateStatusMessage(const char *)));

  connect( toolsw, SIGNAL( newcolor(uint)),
     grid, SLOT(setColorSelection(uint)));

  connect( grid, SIGNAL( changed(const QPixmap &)),
    toolsw, SLOT(setPreview(const QPixmap &)));
  connect( grid, SIGNAL( addingcolor(uint) ),
    toolsw, SLOT(addColor(uint)));
  connect( grid, SIGNAL( colorschanged(uint, uint*) ),
    toolsw, SLOT(addColors(uint, uint*)));

  connect(grid, SIGNAL(sizechanged(int, int)),
           SLOT( slotUpdateStatusSize(int, int)));
  connect(grid, SIGNAL(poschanged(int, int)),
           SLOT( slotUpdateStatusPos(int, int)));
  connect(grid, SIGNAL(scalingchanged(int, bool)),
           SLOT( slotUpdateStatusScaling(int, bool)));
  connect( grid, SIGNAL( addingcolor(uint) ),
           SLOT(slotUpdateStatusColors(uint)));
  connect(grid, SIGNAL(colorschanged(uint, uint*)),
           SLOT( slotUpdateStatusColors(uint, uint*)));
  connect(grid, SIGNAL(newmessage(const char *)),
           SLOT( slotUpdateStatusMessage(const char *)));
  connect(grid, SIGNAL(clipboarddata(bool)),
           SLOT( slotUpdatePaste(bool)));
  connect(grid, SIGNAL(selecteddata(bool)),
           SLOT( slotUpdateCopy(bool)));

  debug("Setting tool");
  slotTools(ID_DRAW_FIND);
  debug("Updating statusbar");
  slotUpdateStatusSize(grid->cols(), grid->rows());
  slotUpdateStatusScaling(grid->scaling(), true);
  slotUpdateStatusName(icon->url());
  slotUpdateCopy(false);
  uint *c = 0, n = 0;
  n = grid->getColors(c);
  slotUpdateStatusColors(n, c);
  debug("Setting mainview");
  setView(mainview);
  viewport->show();

  if((winwidth > 0) && (winheight > 0))
    resize( winwidth, winheight );

  debug("Showing");
  show();
  icon->open(&grid->image(), xpm);
  toolsw->setPreview(grid->pixmap());
}

KIconEdit::~KIconEdit()
{
  debug("KIconEdit - Destructor");
/*  the following should be destructed by the QObject/QWidget destructor
    if(l)
      delete l;
    l = 0L; 
    if(grid)
      delete grid;
    grid = 0L; 
    if(toolsw)
      delete toolsw;
    toolsw = 0L; 
    if(mainview)
      delete mainview;
    mainview = 0L; 
    if(newicon)
      delete newicon;
    newicon = 0L; 
*/
    if(recentlist)
      delete recentlist;
    if(toolbar)
      delete toolbar;
    toolbar = 0L; 
    if(drawtoolbar)
      delete drawtoolbar;
    drawtoolbar = 0L; 
    if(menubar)
      delete menubar;
    menubar = 0L; 
    if(statusbar)
      delete statusbar;
    statusbar = 0L; 
#ifdef KWIZARD_VERSION
    if(icontemplates)
      delete icontemplates;
    icontemplates = 0L;
#endif 
    delete file;
    delete edit;
    delete view;
    delete tools;
    delete options;
    delete help;

  debug("KIconEdit - Destructor: done");
}

void KIconEdit::closeEvent(QCloseEvent *e)
{
  bool cancel = false;
  if (grid->isModified()) 
  {
    KWM::activate(winId());
    int r = KMsgBox::yesNoCancel(this, i18n("Warning"), 
		i18n("The current file has been modified.\nDo you want to save it?"));
    switch(r)
    {
      case 1:
        icon->save(&grid->image());
        //grid->save(grid->drawPath());
        break;
      case 2:
        break;
      case 3:
        cancel = true;
        break;
      default:
        break;
    }
  }
  if(!cancel)
  {
    writeConfig();
    icon->cleanup();
    e->accept();
    delete this;
    if(memberList->count() == 0)
    {
      debug("KIconEdit: Last in list closes the door...");
      kapp->quit();
    }
  }
  else
    e->ignore();
}

void KIconEdit::resizeEvent( QResizeEvent * )
{
  // save size of the application window
  //debug("KIconEdit::resizeEvent()");
  winwidth = geometry().width();
  winheight = geometry().height();

  updateRects();
  viewport->updateScrollBars();
}

void KIconEdit::unsaved(bool flag)
{
    setUnsavedData(flag);
}

void KIconEdit::saveGoingDownStatus()
{
  debug("KIconEdit::saveGoingDownStatus");
  if( grid->isModified() )
    icon->saveBackup(&grid->image());
  debug("KIconEdit::saveGoingDownStatus - done");
}

// this is for exit by request of the session manager
void KIconEdit::saveProperties(KConfig *config )
{
  debug("KIconEdit::saveProperties");
  config->writeEntry("Name", icon->url());
  saveGoingDownStatus();
}

// this is for instances opened by the session manager
void KIconEdit::readProperties(KConfig *config)
{
  debug("KIconEdit::readProperties");
  QString entry = config->readEntry("Name", ""); // no default
  if (entry.isEmpty())
    return;
  //readGoingDownStatus(entry);
}

// this is always read
void KIconEdit::readConfig()
{
  KConfig *config = kapp->getConfig();
  config->setGroup( "Files" );
  int n = config->readListEntry("RecentOpen", *recentlist);
  debug("Read %i recent files", n);

  config->setGroup( "Appearance" );

  // restore geometry settings
  QString geom = config->readEntry( "Geometry" );
  if ( !geom.isEmpty() )
    sscanf( geom, "%dx%d", &winwidth, &winheight );

  maintoolbarstat = config->readBoolEntry( "ShowMainToolBar", true );
  drawtoolbarstat = config->readBoolEntry( "ShowDrawToolBar", true );
  statusbarstat = config->readBoolEntry( "ShowStatusBar", true );

  maintoolbarpos = (KToolBar::BarPosition)config->readNumEntry( "MainToolBarPos", KToolBar::Top);
  drawtoolbarpos = (KToolBar::BarPosition)config->readNumEntry( "DrawToolBarPos", KToolBar::Left);
  //statusbarpos = config->readNumEntry( "StatusBarPos", KStatusBar::Bottom);
  menubarpos = (KMenuBar::menuPosition)config->readNumEntry( "MenuBarPos", KMenuBar::Top);

  config->setGroup( "Grid" );
  showgrid = config->readBoolEntry( "ShowGrid", true );
  gridscaling = config->readNumEntry( "GridScaling", 10 );
  debug("readConfig done");
}

// this is for normal exits or request from "Options->Save options".
void KIconEdit::writeConfig()
{
  KConfig *config = kapp->getConfig();
  KIconEditProperties::getProperties(this)->keys->writeSettings(config);
  config->setGroup( "Files" );
  config->writeEntry("RecentOpen", *recentlist);

  config->setGroup( "Appearance" );

  QString geom;
  geom.sprintf( "%dx%d", winwidth, winheight );
  config->writeEntry( "Geometry", geom );

  config->writeEntry("ShowMainToolBar", toolbar->isVisible());
  config->writeEntry("ShowDrawToolBar", drawtoolbar->isVisible());
  config->writeEntry("ShowStatusBar", statusbar->isVisible());

  config->writeEntry("MainToolBarPos", (int)toolbar->barPos());
  config->writeEntry("DrawToolBarPos", (int)drawtoolbar->barPos());
  config->writeEntry("MenuBarPos", (int)menubar->menuBarPos());

  config->setGroup( "Grid" );
  config->writeEntry("ShowGrid", grid->hasGrid());
  config->writeEntry("GridScaling", grid->cellSize());
}

QSize KIconEdit::sizeHint()
{
  if(mainview)
    return mainview->sizeHint();
  else
    return QSize(-1, -1);
}

KMenuBar *KIconEdit::setupMenuBar()
{
  debug("setupMenuBar");
  if(!menubar)
  {
    menubar = new KMenuBar(this);
    CHECK_PTR(menubar);
    setMenu(menubar);
  }

  keys = KIconEditProperties::getProperties(this)->keys; // = new KAccel( this ); 
  CHECK_PTR(keys);
  debug("setupMenuBar - pprops ok");

  keys->connectItem( KAccel::New, this, SLOT(slotNew()) );
  keys->connectItem( KAccel::Save , this, SLOT(slotSave()) );
  keys->connectItem( KAccel::Quit, this, SLOT(slotQuit()) );
  keys->connectItem( KAccel::Cut , this, SLOT(slotCut()) );
  keys->connectItem( KAccel::Copy , this, SLOT(slotCopy()) );
  keys->connectItem( KAccel::Paste , this, SLOT(slotPaste()) );
  keys->connectItem( KAccel::Open , this, SLOT(slotOpen()) );
  keys->connectItem( KAccel::Close , this, SLOT(slotClose()) );
  keys->connectItem( KAccel::Print , this, SLOT(slotPrint()) );
  keys->insertItem(i18n("Select All"),   "Select All",   CTRL+Key_A);
  keys->connectItem( "Select All", this, SLOT(slotSelectAll()) );
  keys->readSettings();

  debug("setupMenuBar - read settings");


  recent = new QPopupMenu;
  CHECK_PTR(recent);
  connect( recent, SIGNAL(activated(int)), SLOT(slotOpenRecent(int)));
  for(uint i = 0; i < recentlist->count(); i++)
    recent->insertItem(recentlist->at(i));

  int id;

  file = new QPopupMenu;
  CHECK_PTR(file);
  file->insertItem(Icon("newwin.xpm"), i18n("New &window"), ID_FILE_NEWWIN);
  file->connectItem(ID_FILE_NEWWIN, this, SLOT(slotNewWin()));
  file->insertSeparator();
  id = file->insertItem(Icon("filenew.xpm"), i18n("&New..."), this, SLOT(slotNew()));
  keys->changeMenuAccel(file, id, KAccel::New); 
  id = file->insertItem(Icon("fileopen.xpm"), i18n("&Open..."), this, SLOT(slotOpen()));
  keys->changeMenuAccel(file, id, KAccel::Open); 
  file->insertItem(i18n("Open recent"), recent, ID_FILE_RECENT);
  //file->setItemEnabled(ID_FILE_RECENT, false);
  file->insertSeparator();
  id = file->insertItem(Icon("filefloppy.xpm"), i18n("&Save"), this, SLOT(slotSave()));
  keys->changeMenuAccel(file, id, KAccel::Save); 
  file->insertItem(i18n("Save &as..."), this, SLOT(slotSaveAs()));
  file->insertSeparator();
  id = file->insertItem(Icon("fileprint.xpm"), i18n("&Print..."), ID_FILE_PRINT);
  file->connectItem(ID_FILE_PRINT, this, SLOT(slotPrint()));
  keys->changeMenuAccel(file, id, KAccel::Print); 
  file->insertSeparator();
  id = file->insertItem(Icon("fileclose.xpm"), i18n("&Close"), this, SLOT(slotClose()));
  keys->changeMenuAccel(file, id, KAccel::Close); 
  id = file->insertItem(i18n("&Exit"), this, SLOT(slotQuit()));
  keys->changeMenuAccel(file, id, KAccel::Quit); 
  menubar->insertItem(i18n("&File"), file);

  edit = new QPopupMenu;
  CHECK_PTR(edit);
  id = edit->insertItem(Icon("editcut.xpm"), i18n("Cu&t"), ID_EDIT_CUT);
  edit->connectItem(ID_EDIT_CUT, this, SLOT(slotCut()));
  keys->changeMenuAccel(edit, id, KAccel::Cut); 
  id = edit->insertItem(Icon("editcopy.xpm"), i18n("&Copy"), ID_EDIT_COPY);
  edit->connectItem(ID_EDIT_COPY, this, SLOT(slotCopy()));
  keys->changeMenuAccel(edit, id, KAccel::Copy); 
  id = edit->insertItem(Icon("editpaste.xpm"), i18n("&Paste"), ID_EDIT_PASTE);
  edit->connectItem(ID_EDIT_PASTE, this, SLOT(slotPaste()));
  keys->changeMenuAccel(edit, id, KAccel::Paste); 
  id = edit->insertItem(i18n("Paste as &new"), ID_EDIT_PASTE_AS_NEW);
  edit->connectItem(ID_EDIT_PASTE_AS_NEW, grid, SLOT(editPasteAsNew()));
  id = edit->insertItem(i18n("Clea&r"),  this, SLOT(slotClear()));
  edit->insertSeparator();
  id = edit->insertItem(i18n("Select &all"),  this, SLOT(slotSelectAll()));
  keys->changeMenuAccel(edit, id, "Select All"); 
  menubar->insertItem(i18n("&Edit"), edit);

  zoom = new QPopupMenu;
  CHECK_PTR(zoom);
  id = zoom->insertItem(i18n("1:1"), ID_VIEW_ZOOM_1TO1);
  id = zoom->insertItem(i18n("1:5"), ID_VIEW_ZOOM_1TO5);
  id = zoom->insertItem(i18n("1:10"), ID_VIEW_ZOOM_1TO10);
  connect( zoom, SIGNAL(activated(int)), SLOT(slotView(int)));

  view = new QPopupMenu;
  CHECK_PTR(view);
  id = view->insertItem(Icon("viewmag+.xpm"), i18n("Zoom &in"), ID_VIEW_ZOOM_IN);
  id = view->insertItem(Icon("viewmag-.xpm"), i18n("Zoom &out"), ID_VIEW_ZOOM_OUT);
  id = view->insertItem(Icon("viewmag.xpm"), i18n("Zoom factor"), zoom);
  connect( view, SIGNAL(activated(int)), SLOT(slotView(int)));
  menubar->insertItem(i18n("&View"), view);

  image = new QPopupMenu;
  CHECK_PTR(image);
#if QT_VERSION >= 140
  id = image->insertItem(Icon("transform.xpm"), i18n("&Resize"), ID_IMAGE_RESIZE);
#endif
  id = image->insertItem(Icon("grayscale.xpm"), i18n("&GrayScale"), ID_IMAGE_GRAYSCALE);
  /*
  id = image->insertItem(Icon("kdepalette.xpm"), i18n("&Map to KDE palette"), ID_IMAGE_MAPTOKDE);
  */
  connect( image, SIGNAL(activated(int)), SLOT(slotImage(int)));
  menubar->insertItem(i18n("&Icon"), image);

  tools = new QPopupMenu;
  CHECK_PTR(tools);
  tools->insertItem(Icon("paintbrush.xpm"), i18n("Freehand"), ID_DRAW_FREEHAND);
  tools->insertItem(Icon("rectangle.xpm"), i18n("Rectangle"), ID_DRAW_RECT);
  tools->insertItem(Icon("filledrectangle.xpm"), i18n("Filled rectangle"), ID_DRAW_RECT_FILL);
  tools->insertItem(Icon("circle.xpm"), i18n("Circle"), ID_DRAW_CIRCLE);
  tools->insertItem(Icon("filledcircle.xpm"), i18n("Filled circle"), ID_DRAW_CIRCLE_FILL);
  tools->insertItem(Icon("ellipse.xpm"), i18n("Ellipse"), ID_DRAW_ELLIPSE);
  tools->insertItem(Icon("filledellipse.xpm"), i18n("Filled ellipse"), ID_DRAW_ELLIPSE_FILL);
  tools->insertItem(Icon("spraycan.xpm"), i18n("Spray"), ID_DRAW_SPRAY);
  tools->insertItem(Icon("flood.xpm"), i18n("Flood fill"), ID_DRAW_FILL);
  tools->insertItem(Icon("line.xpm"), i18n("Line"), ID_DRAW_LINE);
  tools->insertItem(Icon("eraser.xpm"), i18n("Eraser (Transparent)"), ID_DRAW_ERASE);
  //tools->insertSeparator();
  connect( tools, SIGNAL(activated(int)), SLOT(slotTools(int)));
  menubar->insertItem(i18n("&Tools"), tools);

  options = new QPopupMenu;
  CHECK_PTR(options);
  id = options->insertItem(i18n("&Configure"), ID_OPTIONS_CONFIGURE);
  options->insertSeparator();
  options->insertItem(i18n("Toggle &Grid"), ID_OPTIONS_TOGGLE_GRID);
  if(showgrid)
    options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, true);
  options->insertItem(i18n("Toggle &toolbar"), ID_OPTIONS_TOGGLE_TOOL1);
  if(maintoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, true);
  options->insertItem(i18n("Toggle &drawing tools"), ID_OPTIONS_TOGGLE_TOOL2);
  if(drawtoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, true);
  options->insertItem(i18n("Toggle &statusbar"), ID_OPTIONS_TOGGLE_STATS);
  if(statusbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, true);
  options->insertSeparator();
  options->insertItem(Icon("filefloppy.xpm"), i18n("&Save options"), ID_OPTIONS_SAVE);
  connect( options, SIGNAL(activated(int)), SLOT(slotConfigure(int)));
  menubar->insertItem(i18n("&Options"), options);

  menubar->insertSeparator();
  help = kapp->getHelpMenu(true,ABOUTSTR);
  menubar->insertItem(i18n("&Help"), help);

  menubar->setMenuBarPos(menubarpos);
  menubar->show();
  //connect( menubar, SIGNAL(activated(int)), SLOT(slotActions(int)));

  debug("setupMenuBar - done");
  return menubar;
}

KToolBar *KIconEdit::setupToolBar()
{
  debug("setupToolBar");
  toolbar = new KToolBar(this);
  CHECK_PTR(toolbar);
  addToolBar(toolbar);

  toolbar->insertButton(Icon("filenew.xpm"), ID_FILE_NEWFILE,
         SIGNAL(clicked()), this, SLOT(slotNew()), TRUE, i18n("New File"));
  toolbar->insertButton(Icon("fileopen.xpm"),ID_FILE_OPEN,
         SIGNAL(clicked()), this, SLOT(slotOpen()), TRUE, i18n("Open a file"));
  toolbar->insertButton(Icon("filefloppy.xpm"), ID_FILE_SAVE,
         SIGNAL(clicked()), this, SLOT(slotSave()), TRUE, i18n("Save the file"));
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("fileprint.xpm"),ID_FILE_PRINT,
         SIGNAL(clicked()), this, SLOT(slotPrint()), TRUE, i18n("Print icon"));
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("editcut.xpm"),ID_EDIT_CUT,
         SIGNAL(clicked()), this, SLOT(slotCut()), TRUE, i18n("Cut"));
  toolbar->insertButton(Icon("editcopy.xpm"),ID_EDIT_COPY,
         SIGNAL(clicked()), this, SLOT(slotCopy()), TRUE, i18n("Copy"));
  toolbar->insertButton(Icon("editpaste.xpm"),ID_EDIT_PASTE,
         SIGNAL(clicked()), this, SLOT(slotPaste()), TRUE, i18n("Paste"));
  toolbar->insertSeparator();
#if QT_VERSION >= 140
  toolbar->insertButton(Icon("transform.xpm"),ID_IMAGE_RESIZE, TRUE, 
			  i18n("Resize"));
#endif
  toolbar->insertButton(Icon("grayscale.xpm"),ID_IMAGE_GRAYSCALE, TRUE, 
			  i18n("GrayScale"));
/*
  toolbar->insertButton(Icon("kdepalette.xpm"),ID_IMAGE_MAPTOKDE, TRUE, 
			  i18n("To KDE palette"));
*/
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("viewmag-.xpm"),ID_VIEW_ZOOM_OUT, TRUE, 
			  i18n("Zoom out"));
  toolbar->insertButton(Icon("viewmag+.xpm"),ID_VIEW_ZOOM_IN, TRUE, 
			  i18n("Zoom in"));
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("grid.xpm"),ID_OPTIONS_TOGGLE_GRID, TRUE, 
			  i18n("Toggle grid"));
  toolbar->setToggle(ID_OPTIONS_TOGGLE_GRID, true);
  if(showgrid)
    ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(true);

  toolbar->insertButton(Icon("newwin.xpm"),ID_FILE_NEWWIN,
         SIGNAL(clicked()), this, SLOT(slotNewWin()), TRUE, i18n("New Window"));
  toolbar->alignItemRight( ID_FILE_NEWWIN, true);
    
  toolbar->setBarPos(maintoolbarpos);
  if(maintoolbarstat)
    toolbar->enable(KToolBar::Show);
  else
    toolbar->enable(KToolBar::Hide);
  //toolbar->show();
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotView(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotImage(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotConfigure(int)));

  debug("setupToolBar - done");
  return toolbar;
}

KToolBar *KIconEdit::setupDrawToolBar()
{
  debug("setupDrawToolBar");
  drawtoolbar = new KToolBar(this);
  CHECK_PTR(drawtoolbar);
  addToolBar(drawtoolbar);

  drawtoolbar->insertButton(Icon("pointer.xpm"), ID_DRAW_FIND, TRUE, i18n("Find pixel"));
  drawtoolbar->setToggle(ID_DRAW_FIND, true);
  drawtoolbar->insertButton(Icon("paintbrush.xpm"), ID_DRAW_FREEHAND, TRUE, i18n("Draw freehand"));
  drawtoolbar->setToggle(ID_DRAW_FREEHAND, true);
  drawtoolbar->insertButton(Icon("areaselect.xpm"), ID_DRAW_SELECT, TRUE, i18n("Select area"));
  drawtoolbar->setToggle(ID_DRAW_SELECT, true);
  drawtoolbar->insertButton(Icon("line.xpm"), ID_DRAW_LINE, TRUE, i18n("Draw line"));
  drawtoolbar->setToggle(ID_DRAW_LINE, true);
  drawtoolbar->insertButton(Icon("rectangle.xpm"),ID_DRAW_RECT, TRUE, i18n("Draw rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT, true);
  drawtoolbar->insertButton(Icon("filledrectangle.xpm"),ID_DRAW_RECT_FILL, TRUE, i18n("Draw filled rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT_FILL, true);
  drawtoolbar->insertButton(Icon("circle.xpm"),ID_DRAW_CIRCLE, TRUE, i18n("Draw circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE, true);
  drawtoolbar->insertButton(Icon("filledcircle.xpm"),ID_DRAW_CIRCLE_FILL, TRUE, i18n("Draw filled circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE_FILL, true);
  drawtoolbar->insertButton(Icon("ellipse.xpm"),ID_DRAW_ELLIPSE, TRUE, i18n("Draw ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE, true);
  drawtoolbar->insertButton(Icon("filledellipse.xpm"),ID_DRAW_ELLIPSE_FILL, TRUE, i18n("Draw filled ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE_FILL, true);
  drawtoolbar->insertButton(Icon("spraycan.xpm"),ID_DRAW_SPRAY, TRUE, i18n("Spray"));
  drawtoolbar->setToggle(ID_DRAW_SPRAY, true);
  drawtoolbar->insertButton(Icon("flood.xpm"),ID_DRAW_FILL, TRUE, i18n("Flood fill"));
  drawtoolbar->setToggle(ID_DRAW_FILL, true);
  drawtoolbar->setToggle(ID_DRAW_SPRAY, true);
  drawtoolbar->insertButton(Icon("eraser.xpm"),ID_DRAW_ERASE, TRUE, i18n("Erase"));
  drawtoolbar->setToggle(ID_DRAW_ERASE, true);
    
  drawtoolbar->setBarPos(drawtoolbarpos);
  if(drawtoolbarstat)
    drawtoolbar->enable(KToolBar::Show);
  else
    drawtoolbar->enable(KToolBar::Hide);
  //drawtoolbar->show();
  connect( drawtoolbar, SIGNAL(clicked(int)), SLOT(slotTools(int)));

  debug("setupDrawToolBar - done");
  return drawtoolbar;
}

KStatusBar *KIconEdit::setupStatusBar()
{
  statusbar = new KStatusBar(this);
  CHECK_PTR(statusbar);
  setStatusBar(statusbar);
  statusbar->insertItem("    -1, -1    ", 0);
  statusbar->insertItem("   32 x 32   ", 1);
  statusbar->insertItem(" 1:1000 ", 2);
  statusbar->insertItem("Colors:       ", 3);
  statusbar->insertItem("", 4);

  if(statusbarstat)
    statusbar->enable(KStatusBar::Show);
  else
    statusbar->enable(KStatusBar::Hide);
  return statusbar;
}


void KIconEdit::addRecent(const char *filename)
{
  //debug("addRecent - checking %s", filename);
  if(filename && strlen(filename) == 0 || recentlist->contains(filename))
    return;
  //debug("addRecent - adding %s", filename);

  if( recentlist->count() < 5)
    recentlist->insert(0,filename);
  else
  {
    recentlist->remove(4);
    recentlist->insert(0,filename);
  }

  recent->clear();

  for ( int i = 0 ;i < (int)recentlist->count(); i++)
    recent->insertItem(recentlist->at(i));

  file->setItemEnabled(ID_FILE_RECENT, true);
  //debug("addRecent - done");
}

void KIconEdit::toggleTool(int id)
{
  for(int i = ID_DRAW_FREEHAND; i <= ID_DRAW_ERASE; i++)
  {
    if(i != id)
    {
      if(tools->isItemChecked(i))
        tools->setItemChecked(i, false);
      if(drawtoolbar->isButtonOn(i));
        drawtoolbar->setButton(i, false);
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(false);
    }
    else
    {
      tools->setItemChecked(i, true);
      if(!drawtoolbar->isButtonOn(i));
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(true);
        drawtoolbar->setButton(i, true);
    }
  }
}


