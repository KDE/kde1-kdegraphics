#define ID_NEXT 0
#define ID_PREV 1
#define ID_PAGE 2
#define ID_ZOOM 3
#define ID_PRINT 4
#define ID_MARK 5
#define ID_START 6
#define ID_END 7
#define ID_READ 8

#define ID_LOCATION 0
#define ID_MAGSTEP 1
#define ID_ORIENTATION 2
#define ID_FILENAME 3

#define ID_ANTIALIAS 0
#define ID_MESSAGES 1
#define ID_TOOLBAR 2
#define ID_STATUSBAR 3
#define ID_CONFIGURE 4
#define ID_GHOSTSCRIPT 5
#define ID_SAVE 6

#define ID_NEW_WINDOW 0
#define ID_CLOSE_WINDOW 1
#define ID_OPEN_FILE 2

#define ID_FILE_A 5
#define ID_FILE_B 6
#define ID_FILE_C 7
#define ID_FILE_D 8
#define ID_FILE_QUIT 9

#include <kmisc.h>
#include "kghostview.h"

#include <kkeyconf.h>
#include <kapp.h>

#include <kmsgbox.h>

extern "C" {
	#include "ps.h"
}

#include "kghostview.moc"

int toolbar1;

QList <KGhostview> KGhostview::windowList;

KGhostview::KGhostview( QWidget *, char *name )
	: KTopLevelWidget( name )
{   
    //printf("KGhostview::KGhostview\n");
    
    windowList.setAutoDelete( FALSE );
    
    // Initialise all the variables declared in this class.
    // I ususally forget to do a few resulting in nasty seg. faults !
    
    kfm = 0L;
    
    oldfilename.sprintf("");
	filename.sprintf("");
	psfile=NULL;
	doc=NULL;
	olddoc=NULL;
    
    for(int file_count=0;file_count<4;file_count++) {
    	lastOpened[file_count].sprintf("");
    }
    
    current_page=0;
    num_parts=0;
    for(int part_count=0;part_count<10;part_count++) {
    	pages_in_part[part_count]=0;
    }
    
    shrink_magsteps = 10;
	current_magstep = magstep = 10;
	expand_magsteps = 10;
	default_xdpi = 75.0;
	default_ydpi = 75.0;
	default_pagemedia = 1;
	base_papersize=0;
	orientation = 1;
	force_orientation = FALSE;
	force_pagemedia = FALSE;
	current_llx = 0;
	current_lly = 0;
	current_urx = 0;
	current_ury = 0;
	hide_toolbar = FALSE;
	hide_statusbar = FALSE;
    
    //
    // MENUBAR
    //
    
	createMenubar();
	
	//
	// TOOLBAR
	//
	
	createToolbar();
	
	//
    // MAIN WINDOW
    //
    
    //printf("Done toolbar - Create KPSWidget\n");
    
    page = new KPSWidget( this );
    CHECK_PTR( page );
    
    // Register page widget with KTopLevelWidget
    
    setView( page, FALSE );
    
    //printf("Done pswidget - Create statusbar\n");
    
    //
    // STATUSBAR
    //
    
    createStatusbar();
    
    //
  	//	INFO DIALOG
  	//
  
  	//printf("Do info dialog\n");
  
  	infoDialog = new InfoDialog( 0, "info dialog" );
  	infoDialog->setCaption("Information");
  	
  	//
  	//	PRINTING DIALOG
  	//
  
	pd = new PrintDialog( 0, "print dialog" );
	pd->setCaption("Print");
	
	//
	//	VIEWCONTROL DIALOG
	//
	
	vc = new ViewControl( 0 , "view control" );
	vc->setCaption("View control");
	connect(vc, SIGNAL( applyChanges() ), this, SLOT( applyViewChanges() ));
	
	// Build the combo box menu for magnification steps.
	
	int i;
	for (i = 1; i <= shrink_magsteps; i++) {
		char buf[16];
		
		sprintf(buf, "%d %%", (int)(100*i/(shrink_magsteps)));
		vc->magComboBox->insertItem(buf);
	}
	for (i = 1; i <= expand_magsteps; i++) {
		char buf[16];
		
		sprintf(buf, "%d %%", (int)(100+200*i/(expand_magsteps)));
		vc->magComboBox->insertItem(buf);
	}
	
	//
	//	READ SETTINGS AND SET OPTIONS MENU, SET LAST OPENED FILES
	//
	
	readSettings();
    	
    if ( hide_toolbar )	
    	m_options->setItemChecked( ID_TOOLBAR, FALSE );
    else
    	m_options->setItemChecked( ID_TOOLBAR, TRUE );
    	
    if ( hide_statusbar )
    	m_options->setItemChecked(  ID_STATUSBAR, FALSE );
    else
    	m_options->setItemChecked( ID_STATUSBAR, TRUE );
    	
    changeFileRecord();
	
	
	//
	// Make drop zone.
	//
	
	KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
	connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
		this, SLOT( slotDropEvent( KDNDDropZone *) ) );
	
	//
	//	RESIZE AND DISPLAY
	//

	resize( options_width, options_height );
	show();
}

KGhostview::~KGhostview()
{
	windowList.removeRef(this);
}

void KGhostview::bindKeys()
{    
    // create the KKeyCode object
    
    if(  this == KGhostview::windowList.first() ) {
    
	initKKeyConfig( kapp->getConfig() );

	// create functionName/keyCode association
	
	kKeys->addKey("Quit", "CTRL+Q");
	kKeys->addKey("Open", "CTRL+O");
	kKeys->addKey("New", "CTRL+N");
	kKeys->addKey("Close", "CTRL+W");
	kKeys->addKey("Print", "CTRL+P");
	kKeys->addKey("Help", "F1");
	kKeys->addKey("View Control", "CTRL+L");
	kKeys->addKey("Go To Page", "CTRL+G");
	kKeys->addKey("Zoom In", "Plus");     
	kKeys->addKey("Zoom Out", "Minus"); 
	kKeys->addKey("Next Page", "Next");
	kKeys->addKey("Prev Page", "Prior");
	kKeys->addKey("Scroll Up", "Up");
	kKeys->addKey("Scroll Down", "Down");
	kKeys->addKey("Scroll Left", "Left");
	kKeys->addKey("Scroll Right", "Right");
	kKeys->addKey("Redisplay", "CTRL+R");
	kKeys->addKey("Information", "CTRL+I");  
	
	}    

	// Register the main widget
	
	int window_count=0;
	QString widgetName;
	for ( KGhostview *kg_temp = KGhostview::windowList.first(); kg_temp!=NULL;
		kg_temp=KGhostview::windowList.next() )
	{
	
	window_count++;
	if( kg_temp == this )
		break;
	
	}

	widgetName.sprintf( "kg%d", window_count);
	
	kKeys->registerWidget(widgetName.data(), this);

	KGhostview *kg_temp = this;
	
	// define the connection for the main widget
	
	kKeys->connectFunction( widgetName.data(), "Quit",
							qApp, SLOT( quit() ) );
								
	kKeys->connectFunction( widgetName.data(), "Open",
								kg_temp, SLOT( openNewFile() ) );
	
	kKeys->connectFunction( widgetName.data(), "New",
								kg_temp, SLOT( newWindow() ) );
								
	kKeys->connectFunction( widgetName.data(), "Close",
								kg_temp, SLOT( closeWindow() ) );
	
	kKeys->connectFunction( widgetName.data(), "Print",
								kg_temp, SLOT( print() ) );
	
	kKeys->connectFunction( widgetName.data(), "Help",
								kg_temp, SLOT( help() ) );
	
	kKeys->connectFunction( widgetName.data(), "View Control",
								kg_temp, SLOT( viewControl() ) );
								
	kKeys->connectFunction( widgetName.data(), "Go To Page",
								kg_temp, SLOT( goToPage() ) );
								
	kKeys->connectFunction( widgetName.data(), "Zoom In",
								kg_temp, SLOT( zoomIn() ) );
								
	kKeys->connectFunction( widgetName.data(), "Zoom Out",
								kg_temp, SLOT( zoomOut() ) );
								
	kKeys->connectFunction( widgetName.data(), "Next Page",
								kg_temp, SLOT( nextPage() ) );
	
	kKeys->connectFunction( widgetName.data(), "Prev Page",
								kg_temp, SLOT( prevPage() ) );
								
	kKeys->connectFunction( widgetName.data(), "Scroll Up",
								kg_temp, SLOT( scrollUp() ) );
								
	kKeys->connectFunction( widgetName.data(), "Scroll Down",
								kg_temp, SLOT( scrollDown() ) );
								
	kKeys->connectFunction( widgetName.data(), "Scroll Left",
								kg_temp, SLOT( scrollLeft() ) );
								
	kKeys->connectFunction( widgetName.data(), "Scroll Right",
								kg_temp, SLOT( scrollRight() ) );
								
	kKeys->connectFunction( widgetName.data(), "Redisplay",
								kg_temp, SLOT( redisplay() ) );
								
	kKeys->connectFunction( widgetName.data(), "Information",
								kg_temp, SLOT( info() ) );
								
}

void KGhostview::copyright()
{
	CopyrightDialog *cd = new CopyrightDialog( 0, "copyright" );
	cd->exec();
}

void KGhostview::info()
{
	infoDialog->fileLabel->setText(filename);
	if( doc ) {
		infoDialog->titleLabel->setText(doc->title);
		infoDialog->dateLabel->setText(doc->date);
	} else {
		infoDialog->titleLabel->setText("");
		infoDialog->dateLabel->setText("");
	}
		
	if( !infoDialog->isVisible() ) {
		infoDialog->show();
		infoDialog->ok->setFocus();
	}
}

void KGhostview::configureGhostscript()
{
	page->intConfig->setCaption("Configure interpreter");
	page->intConfig->init();
	
	if( page->intConfig->exec() ) {
	}
}

void KGhostview::configureKeybindings()
{
	kKeys->configureKeys( this );
	updateMenuAccel();
}

void KGhostview::updateMenuAccel()
{
	changeMenuAccel( m_file, ID_PRINT, "Print" );
	changeMenuAccel( m_file, ID_OPEN_FILE, "Open" );
	changeMenuAccel( m_file, ID_NEW_WINDOW, "New" );
	changeMenuAccel( m_file, ID_CLOSE_WINDOW, "Close" );
	changeMenuAccel( m_file, ID_FILE_QUIT, "Quit" );
	changeMenuAccel( m_view, zoomInID, "Zoom In" );
	changeMenuAccel( m_view, zoomOutID, "Zoom Out" );
	changeMenuAccel( m_view, viewControlID, "View Control" );
	changeMenuAccel( m_view, redisplayID, "Redisplay" );
	changeMenuAccel( m_view, infoID, "Information" );
	changeMenuAccel( m_go, nextID, "Next Page" );
	changeMenuAccel( m_go, prevID, "Prev Page" );
	changeMenuAccel( m_go, goToPageID, "Go To Page" );
	changeMenuAccel( m_help, helpID, "Help" );
}

void KGhostview::changeMenuAccel ( QPopupMenu *menu, int id,
	char *functionName )
{
	QString s = menu->text( id );
	if ( !s ) return;
	
	int i = s.find('\t');
	
	QString k = keyToString( kKeys->readCurrentKey( functionName ) );
	if( !k ) return;
	
	if ( i >= 0 )
		s.replace( i+1, s.length()-i, k );
	else {
		s += '\t';
		s += k;
	}
	
	menu->changeItem( s, id );
}

void KGhostview::createMenubar()
{
 	m_file = new QPopupMenu;
    CHECK_PTR( m_file );
    m_file->insertItem( "&New", ID_NEW_WINDOW );
    m_file->insertItem( "&Open ...", ID_OPEN_FILE);
    m_file->insertItem( "&Close", ID_CLOSE_WINDOW);
    m_file->insertSeparator();
    m_file->insertItem( "&Print ...", ID_PRINT );
    m_file->insertSeparator();
    m_file->insertItem( "&1.", ID_FILE_A );
    m_file->insertItem( "&2.", ID_FILE_B );
    m_file->insertItem( "&3.", ID_FILE_C );
    m_file->insertItem( "&4.", ID_FILE_D );
    m_file->insertSeparator();
    m_file->insertItem( "E&xit", ID_FILE_QUIT );
    
    connect( m_file, SIGNAL( activated( int ) ), this, 
    	SLOT( fileMenuActivated( int ) ) );
    
	m_view = new QPopupMenu;
    CHECK_PTR( m_view );
    zoomInID =
	m_view->insertItem( "Zoom &in", this, SLOT( zoomIn()) );
	zoomOutID =
	m_view->insertItem( "Zoom &out", this, SLOT( zoomOut() ) );
	viewControlID =
	m_view->insertItem( "&View control ...", this, SLOT( viewControl() ) );
	m_view->insertSeparator();
	shrinkWrapID = 
	m_view->insertItem( "&Shrink wrap", this, SLOT( shrinkWrap() ) );
	redisplayID =
	m_view->insertItem( "&Redisplay", this, SLOT( redisplay() ) );
	infoID =
	m_view->insertItem( "&Info ...", this, SLOT( info() ) );

    m_go = new QPopupMenu;
    CHECK_PTR( m_go );
    nextID =
    m_go->insertItem( "&Next page", this, SLOT( nextPage() ) );
    prevID =
    m_go->insertItem( "&Previous Page", this, SLOT( prevPage() ) );
    goToPageID =
    m_go->insertItem( "&Go to page ...", this, SLOT( goToPage() ) );
    m_go->insertSeparator();
    goToStartID =
    m_go->insertItem( "Go to &start", this, SLOT( goToStart() ) );
    goToEndID =
    m_go->insertItem( "Go to &end", this, SLOT( goToEnd() ) );
    readDownID =
    m_go->insertItem( "&Read down", this, SLOT( dummy() ) );
    
  
    m_options = new QPopupMenu;
    CHECK_PTR( m_options );
    
    // I make the options menu here but don't set the entries until
    // I've read the config file.
    
	m_options->setCheckable( TRUE );
	m_options->insertItem("&Configure interpreter ...", ID_GHOSTSCRIPT);
	m_options->insertSeparator();
    m_options->insertItem("Show &tool bar", ID_TOOLBAR);
    m_options->insertItem("Show &status bar", ID_STATUSBAR);
    m_options->insertItem("Configure &key bindings ...", ID_CONFIGURE);
    m_options->insertSeparator();
    m_options->insertItem("&Save Options", ID_SAVE);
    
    connect( m_options, SIGNAL( activated(int) ),
    			SLOT( optionsMenuActivated(int) ) );
    
    m_help = new QPopupMenu;
    CHECK_PTR( m_help );
	m_help->insertItem( "&About this application", this, SLOT( about() ) );
	helpID = 
	m_help->insertItem( "&Help for this application", this, SLOT( help() ) );
	m_help->insertSeparator();
	m_help->insertItem( "&Copyright", this, SLOT( copyright() ) );
	
    menubar = new KMenuBar( this );
    CHECK_PTR( menubar );
    
    menubar->insertItem( "&File", m_file );
    menubar->insertItem( "&View", m_view );
    menubar->insertItem( "&Go", m_go );
    menubar->insertItem( "&Options", m_options);
    menubar->insertSeparator();
    menubar->insertItem( "&Help", m_help);
	
	m_file->setItemEnabled(ID_PRINT, FALSE);
	m_go->setItemEnabled(nextID, FALSE);
	m_go->setItemEnabled(prevID, FALSE);
	m_go->setItemEnabled(goToPageID, FALSE);
	m_go->setItemEnabled(goToStartID, FALSE);
	m_go->setItemEnabled(goToEndID, FALSE);
	m_go->setItemEnabled(readDownID, FALSE);
	m_view->setItemEnabled(viewControlID, FALSE);
	m_view->setItemEnabled(zoomInID, FALSE);
	m_view->setItemEnabled(zoomOutID, FALSE);
	m_view->setItemEnabled(shrinkWrapID, FALSE);
	m_view->setItemEnabled(redisplayID, FALSE);
	m_view->setItemEnabled(infoID, FALSE);
	
	// Register menubar with KTopLevelWidget layout manager
	
	setMenu( menubar );
}

void KGhostview::fileMenuActivated( int item )
{
	switch( item ) {
		case ID_NEW_WINDOW:
			newWindow();
			break;
			
		case ID_CLOSE_WINDOW:
			closeWindow();
			break;
			
		case ID_OPEN_FILE:
			openNewFile();
			break;
			
		case ID_PRINT:
			print();
			break;
			
		case ID_FILE_A:
			open_file( lastOpened[0] );
			break;
			
		case ID_FILE_B:
			open_file( lastOpened[1] );
			break;
			
		case ID_FILE_C:
			open_file( lastOpened[2] );
			break;
			
		case ID_FILE_D:
			open_file( lastOpened[3] );
			break;
			
		case ID_FILE_QUIT:
			qApp->quit();
	}
}

void KGhostview::createToolbar()
{
	//printf("KGhostview::createToolbar\n");

	// Read environment for location of KDE and set pixmap directory

	QString PIXDIR = "";
	char *kdedir = getenv("KDEDIR");
	if(kdedir)
		PIXDIR.append(kdedir);
	else
		PIXDIR.append("/usr/local/kde");
	PIXDIR.append("/lib/pics/toolbar/");

	//printf("Read environemnt\n");

	// Create the toolbar

	QPixmap pixmap;
	toolbar = new KToolBar( this );

	pixmap.load(PIXDIR + "back.xpm");
	toolbar->insertItem(pixmap, ID_PREV, FALSE, "Go back");

	pixmap.load(PIXDIR + "forward.xpm");
	toolbar->insertItem(pixmap, ID_NEXT, FALSE, "Go forward");

	pixmap.load(PIXDIR + "page.xpm");
	toolbar->insertItem(pixmap, ID_PAGE, FALSE, "Go to page ...");

	toolbar->insertSeparator();

	pixmap.load(PIXDIR + "viewzoom.xpm");
	toolbar->insertItem(pixmap, ID_ZOOM, FALSE, "Change view ...");

	toolbar->insertSeparator();

	pixmap.load(PIXDIR + "fileprint.xpm");
	toolbar->insertItem(pixmap, ID_PRINT, FALSE, "Print document");

	pixmap.load(PIXDIR + "flag.xpm");
	toolbar->insertItem(pixmap, ID_MARK, FALSE, "Mark this page");

	toolbar->insertSeparator();

	pixmap.load(PIXDIR + "start.xpm");
	toolbar->insertItem(pixmap, ID_START, FALSE, "Go to start");

	pixmap.load(PIXDIR + "finish.xpm");
	toolbar->insertItem(pixmap, ID_END, FALSE, "Go to end");

	pixmap.load(PIXDIR + "next.xpm");
	toolbar->insertItem(pixmap, ID_READ, FALSE, "Read down the page");

	// Register toolbar with KTopLevelWidget layout manager
	
	//printf("Inserted all buttons\n");

	toolbar1 = addToolBar( toolbar );
	
	//printf("Registered with ktopwidget\n");
	
	toolbar->setPos( KToolBar::Top );
	toolbar->show();
	
	//printf("Connect toolbar\n");
	
	connect(toolbar, SIGNAL( clicked( int ) ), this,
						SLOT( toolbarClicked( int ) ) );
						
	//printf("Done\n");
}


void KGhostview::createStatusbar()
{   
    //printf("KGhostview::createStatusbar\n");
    
    statusbar = new KStatusBar( this );
    statusbar->insertItem("Sc.100 of 100  P.100 of 100", ID_LOCATION);
    statusbar->insertItem("100 %", ID_MAGSTEP);
    statusbar->insertItem("Upside Down", ID_ORIENTATION);
    statusbar->insertItem("", ID_FILENAME);
    
    statusbar->changeItem("", ID_LOCATION);
    statusbar->changeItem("", ID_MAGSTEP);
    statusbar->changeItem("", ID_ORIENTATION);
    
    statusbar->show();
    
    // Register status bar with KTopLevelWidget layout manager
    
    setStatusBar( statusbar );
    
    //printf("done\n");
}

void KGhostview::openNetFile( const char *_url )
{

	QString string;
	netFile = _url;
	netFile.detach();
	KURL u( netFile.data() );

	if ( u.isMalformed() ) {
		KMsgBox::message (0, "Sorry", "Malformed URL.");
		return;
	}

	// Just a usual file ?
	if ( strcmp( u.protocol(), "file" ) == 0 ) {
		open_file( u.path() );
		return;
	}

	if ( kfm != 0L ) {
		KMsgBox::message (0,"Sorry", 
				  "Already waiting\n"\
				  "for an internet job to finish.\n"\
				  "Please wait until has finished.\n"\
				  "Alternatively stop the running one.");
		return;
	}

	kfm = new KFM;
	if ( !kfm->isOK() ) {
		KMsgBox::message (0,"Sorry", "Could not start or find KFM.");
		delete kfm;
		kfm = 0L;
		return;
	}

	tmpFile.sprintf( "file:/tmp/kghostview%i", time( 0L ) );
	connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
	kfm->copy( netFile.data(), tmpFile.data() );
	kfmAction = KGhostview::GET;
	openMode = 0;
}

void KGhostview::slotKFMFinished()
{
	if ( kfmAction == KGhostview::GET ) {
		KURL u( tmpFile.data() );
		open_file( u.path() );
		
		// Clean up
		unlink( tmpFile.data() );
		delete kfm;
		kfm = 0L;
	}
}

void KGhostview::slotDropEvent( KDNDDropZone * _dropZone )
{
    QStrList & list = _dropZone->getURLList();
    
    char *s;

    for ( s = list.first(); s != 0L; s = list.next() )
    {
		// Load the first file in this window
		if ( s == list.getFirst() ) {
	    	QString n = s;

			openNetFile( n.data() );

		} else {

	    	KGhostview *kg = new KGhostview();
	    	kg->show ();
	    	windowList.append( kg );

	    	QString n = s;

			kg->openNetFile( n.data() );

		}
    }
}


void KGhostview::readSettings()
{
	//printf("KGhostview::readSettings\n");
	
	QString str;

	KConfig *config = KApplication::getKApplication()->getConfig();
	
	config->setGroup( "General" );
	
	str = config->readEntry( "Antialiasing" );
	if ( !str.isNull() && str.find( "on" ) == 0 )
		page->intConfig->antialias = TRUE;
	else
		page->intConfig->antialias = FALSE;
		
	str = config->readEntry( "Palette" );
	if ( !str.isNull() && str.find( "mono" ) == 0 )
		page->intConfig->paletteOpt = MONO_PALETTE;
	else if ( !str.isNull() && str.find( "gray" ) == 0 )
		page->intConfig->paletteOpt = GRAY_PALETTE;
	else
		page->intConfig->paletteOpt = COLOR_PALETTE;
		
	str = config->readEntry( "Backing" );
	if ( !str.isNull() && str.find( "store" ) == 0 )
		page->intConfig->backingOpt = STORE_BACKING;
	else
		page->intConfig->backingOpt = PIX_BACKING;
		
	str = config->readEntry( "Messages" );
	if ( !str.isNull() && str.find( "on" ) == 0 )
		page->intConfig->show_messages = TRUE;
	else
		page->intConfig->show_messages = FALSE;
	
	str = config->readEntry( "Statusbar" );
	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_statusbar = TRUE;
		enableStatusBar( KStatusBar::Toggle );
	} else
		hide_statusbar = FALSE;
	
	str = config->readEntry( "Toolbar" );
	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_toolbar = TRUE;
		enableToolBar( KToolBar::Toggle, toolbar1 );
	} else
		hide_toolbar = FALSE;
	
	str = config->readEntry( "Width" );
	if ( !str.isNull() )
		options_width = atoi( str );
	else
		options_width = 600;
	
	str = config->readEntry( "Height" );
	if ( !str.isNull() )
		options_height = atoi( str );
	else
		options_height = 400;
		
	str = config->readEntry( "Magstep" );
	if ( !str.isNull() ) {
		int i = atoi( str );
		if( i<20 && i>0 ) { 
			//printf("Set new magstep in read Settings\n");
			magstep = i;
			set_new_magstep( );
		}
	}
	
	str = config->readEntry( "Orientation" );
	if ( !str.isNull() ) {
		int i = atoi( str );
		if( i<4 && i>0 ) { 
			set_new_orientation(i);
		}
	}
	
	str = config->readEntry( "Toolbar position" );
	if ( !str.isNull() ) {
		if( str == "Left" ) {
			toolbar->setPos( KToolBar::Left );
		} else if( str == "Right" ) {
			toolbar->setPos( KToolBar::Right );
		} else if( str == "Bottom" ) {
			toolbar->setPos( KToolBar::Bottom );
		} else
			toolbar->setPos( KToolBar::Top );
	}
		
	config->setGroup( "Last Opened" );
	str = config->readEntry( "File 1" );
	if ( !str.isNull() )
		lastOpened[0].sprintf( str );
	else
		lastOpened[0].sprintf("");
		
	str = config->readEntry( "File 2" );
	if ( !str.isNull() )
		lastOpened[1].sprintf( str );
	else
		lastOpened[1].sprintf("");
		
	str = config->readEntry( "File 3" );
	if ( !str.isNull() )
		lastOpened[2].sprintf( str );
	else
		lastOpened[2].sprintf("");
		
	str = config->readEntry( "File 4" );
	if ( !str.isNull() )
		lastOpened[3].sprintf( str );
	else
		lastOpened[3].sprintf("");
}

void KGhostview::shrinkWrap()
{
	int new_width=0;
	
	if ( !psfile ) return;
	
	new_width=page->fullView->width()+(x()+width()-view_right)+(view_left-x())+2;
	if( page->vertScrollBar->isVisible() )
		new_width+=page->vertScrollBar->width();
		
	QWidget *d = QApplication::desktop();
    int w=d->width()-10; 	
	if( new_width > w )
		new_width = w;
			
	resize( new_width, height() );
}

void KGhostview::paletteChange( const QPalette & )
{
	if( psfile )
		redisplay();
}

void KGhostview::newWindow()
{
	KGhostview *kg = new KGhostview ();
	kg->resize(width(), height());
	
	windowList.append( kg );

	kg->setMinimumSize( 250, 250 );
	kg->setCaption( "KGhostview: Version 0.5" );
	kg->bindKeys();
	kg->updateMenuAccel();
}

void KGhostview::closeEvent( QCloseEvent * )
{
	//printf("%d windows\n", windowList.count());
	
	if ( windowList.count() > 1 ) {
		windowList.remove( this );
    	delete this;
		for ( KGhostview *kg = windowList.first(); kg!=NULL;
				kg = windowList.next() )
			kg->page->disableInterpreter();
	} else
		qApp->quit();
}

void KGhostview::closeWindow()
{
	close();
}

void KGhostview::redisplay()
{
	page->disableInterpreter();
	show_page( current_page );
}

void KGhostview::writeFileSettings()
{
	//printf("KGhostview::writeFileSettings\n");

	KConfig *config = KApplication::getKApplication()->getConfig();
	
	config->setGroup( "Last Opened" );

	config->writeEntry( "File 1", lastOpened[0] );
	config->writeEntry( "File 2", lastOpened[1] );
	config->writeEntry( "File 3", lastOpened[2] );
	config->writeEntry( "File 4", lastOpened[3] );
}

void KGhostview::writeSettings()
{
	//printf("KGhostview::writeSettings\n");
	
	changed = TRUE;
	if ( !changed )
		return;

	KConfig *config = KApplication::getKApplication()->getConfig();
	
	config->setGroup( "General" );
	
	QString s;
	s.setNum( width() );
	config->writeEntry( "Width", s );
	s.setNum( height() );
	config->writeEntry( "Height", s );
	config->writeEntry( "Antialiasing", page->intConfig->antialias ? "on" : "off" );
	config->writeEntry( "Platform fonts", page->intConfig->platform_fonts ? "on" : "off" );
	config->writeEntry( "Statusbar", hide_statusbar ? "off" : "on" );
	config->writeEntry( "Toolbar", hide_toolbar ? "off" : "on" );
	config->writeEntry( "Messages", page->intConfig->show_messages ? "on" : "off" );
	
	if( page->intConfig->paletteOpt == COLOR_PALETTE )
		s.sprintf("color");
	else if( page->intConfig->paletteOpt == GRAY_PALETTE )
		s.sprintf("grayscale");
	else
		s.sprintf("monochrome");
	config->writeEntry( "Palette", s );
	
	if( page->intConfig->backingOpt == PIX_BACKING )
		s.sprintf("pixmap");
	else
		s.sprintf("store");
	config->writeEntry( "Backing", s );
	
	s.setNum( magstep );
	config->writeEntry( "Magstep", s );
	s.setNum( orientation );
	config->writeEntry( "Orientation", s );
	
	if ( toolbar->Pos() == KToolBar::Left )
		s.sprintf("Left");
	else if ( toolbar->Pos() == KToolBar::Right )
		s.sprintf("Right");
	else if ( toolbar->Pos() == KToolBar::Bottom )
		s.sprintf("Bottom");
	else
		s.sprintf("Top");

	config->writeEntry( "Toolbar position", s );

	changed = FALSE;
}


//
//	SLOTS UP NEXT
//

void KGhostview::toolbarClicked( int item )
{
	
	//printf("toolbar callback\n");
	switch ( item ) {
  		case ID_PREV:
  			prevPage();
  			break;
  		case ID_NEXT:
  			nextPage();
  			break;
  		case ID_PAGE:
  			goToPage();
  			break;
  		case ID_ZOOM:
  			viewControl();
  			break;
  		case ID_PRINT:
  			print();
  			break;
  		case ID_MARK:
  			break;
  		case ID_START:
  			goToStart();
  			break;
  		case ID_END:
  			goToEnd();
  			break;
  		case ID_READ:
  			readDown();
  			break;
  	}
}

void KGhostview::readDown()
{
	if( !page->readDown() )
		nextPage();
}

void KGhostview::scrollUp()
{
	page->scrollUp();
}

void KGhostview::scrollDown()
{
	page->scrollDown();
}

void KGhostview::scrollLeft()
{
	page->scrollLeft();
}

void KGhostview::scrollRight()
{
	page->scrollRight();
}

void KGhostview::help()
{
	if ( fork() == 0 )
	{
		QString path = "";
		char* kdedir = getenv("KDEDIR");
      	if (kdedir)
			path.append(kdedir);
     	 else
		path.append("/usr/local/kde");
      	path.append("/doc/HTML/kghostview");
		path.append("/kghostview.html");
		
		execlp( "kdehelp", "kdehelp", path.data(), 0 );
		exit( 1 );
	}
}


void KGhostview::applyViewChanges()
{
    //printf("KGhostview::applyViewChanges\n");

	int orient=1;
	int selection;
	Bool layout_changed=False;
	
	selection = vc->orientComboBox->currentItem()+1;
	switch(selection) {
		case 1:	orient=1;
				statusbar->changeItem( "Portrait", ID_ORIENTATION );
				break;
		case 2:	orient=4;statusbar->changeItem( "Landscape", ID_ORIENTATION );
				break;
		case 3:	orient=3;statusbar->changeItem( "Seascape", ID_ORIENTATION );
				break;
		case 4:	orient=2;statusbar->changeItem( "Upside down", ID_ORIENTATION );
				break;
	}
	force_orientation = True;
	orientation = orient;
		
	selection = vc->magComboBox->currentItem()+1;
	magstep = selection;
	
	selection = vc->mediaComboBox->currentItem()+1;
	if(doc->epsf && selection ==1) {
		force_pagemedia = False;
		force_document_media = False;
	} else if (selection >= base_papersize) {
		default_pagemedia = selection;
		force_pagemedia = True;
    } else {
		document_media = selection;
		force_document_media = True;
    }
	
	if (set_new_magstep()) layout_changed = True;

	if (set_new_orientation(current_page)) layout_changed = True;

    if (set_new_pagemedia(current_page)) layout_changed = True;
    	
    if (layout_changed) {
		page->layout();
		show_page(current_page);
	}
}

void KGhostview::goToPage()
{
    //printf("KGhostview::goToPage\n");

	GoTo gt(0, "Go to");
	gt.setCaption("Go to");
	gt.current_page=current_page;
	gt.num_parts=num_parts;
	for( int i=0;i<10;i++) {
	
		gt.pages_in_part[i]=pages_in_part[i];
	}
	gt.init();
	if( gt.exec() ) {
		current_page=gt.current_page;
		show_page(current_page);
	}
}

void KGhostview::print()
{
    //printf("KGhostview::print\n");

	pd->setCaption("Print");
	
	if( pd->exec() ) {
		printStart();
	}
}


void KGhostview::viewControl()
{
    //printf("KGhostview::viewControl\n");

	
	vc->magComboBox->setCurrentItem(magstep-1);
	
	if( !vc->isVisible() ) {
		vc->show();
		vc->orientComboBox->setFocus();
	}
}

void KGhostview::zoomIn()
{
	int i;

    i = magstep + 1;
    if (i <= shrink_magsteps+expand_magsteps) {
		set_magstep(i);
	}
}

void KGhostview::zoomOut()
{
	int i;

    i = magstep - 1;
    if (i >= 1) {
		set_magstep(i);
	}
}

void KGhostview::nextPage()
{
    //printf("KGhostview::nextPage\n");

    int new_page = 0;

    if (toc_text) {
	    new_page = current_page + 1;
		if ((unsigned int) new_page >= doc->numpages)
			return;
    }
    show_page(new_page);
    page->scrollTop();
}

void KGhostview::goToStart()
{
    //printf("KGhostview::goToStart\n");

    int new_page = 0;
    
    show_page(new_page);
    page->scrollTop();
}

void KGhostview::goToEnd()
{
    //printf("KGhostview::goToEnd\n");

    int new_page = 0;

    if (toc_text) {
	    new_page = doc->numpages;
    }
    
    show_page(new_page);
    page->scrollTop();
}


void KGhostview::prevPage()
{
    //printf("KGhostview::prevPage\n");

    int new_page = 0;
	
    if (toc_text) {
	    new_page = current_page - 1;
		if (new_page < 0)
			return;
    }
    show_page(new_page);
    page->scrollTop();
}

void KGhostview::openNewFile()
{
    //printf("KGhostview::openNewFile\n");
	
	QString dir;
	if ( filename )
		dir = QFileInfo( filename ).dirPath();	
	
	QString s = QFileDialog::getOpenFileName( dir, "*.*ps", this);
	if ( s.isNull() )
		return;
    
    open_file(s);
}


void KGhostview::optionsMenuActivated( int item )
{
    //printf("KGhostview::optionsMenuActivated\n");
    //printf("item= %d\n", item);

	switch (item){
  		case ID_ANTIALIAS:
  			if(page->intConfig->antialias) {
  				page->intConfig->antialias = FALSE;
  				m_options->changeItem("Turn antialiasing on", item);
  			} else {
  				m_options->changeItem("Turn antialiasing off", item);
  				page->intConfig->antialias = TRUE;
  			}
			page->disableInterpreter();
			if( psfile )
				show_page(current_page);
			break;

		case ID_MESSAGES:
			if(page->intConfig->show_messages) {
  				page->intConfig->show_messages = FALSE;
  				m_options->changeItem("Show Ghostscript messages", item);
  			} else {
  				m_options->changeItem("Hide Ghostscript messages", item);
  				page->intConfig->show_messages = TRUE;
  			}
			break;
			
		case ID_TOOLBAR:
			if(hide_toolbar) {
				hide_toolbar=False;
				enableToolBar( KToolBar::Toggle, toolbar1 );
				//resize( width(), height() );
				m_options->setItemChecked( item, TRUE );
			} else {
				hide_toolbar=True;
				enableToolBar( KToolBar::Toggle, toolbar1 );
				//resize( width(), height() );
				m_options->setItemChecked( item, FALSE );
			}
			break;
			
		case ID_STATUSBAR:
			if(hide_statusbar) {
				hide_statusbar=False;
				enableStatusBar( KStatusBar::Toggle );
				//resize( width(), height() );
				m_options->setItemChecked( item, TRUE );
			} else {
				hide_statusbar=True;
				enableStatusBar( KStatusBar::Toggle );
				//resize( width(), height() );
				m_options->setItemChecked( item, FALSE );
			}
			break;
			
		case ID_CONFIGURE:
			configureKeybindings();
			break;
			
		case ID_GHOSTSCRIPT:
			configureGhostscript();
			break;
			
		case ID_SAVE:
			writeSettings();
	}
}		


void KGhostview::dummy()
{
	 KMsgBox::message(0, "Version 0.5", 
                             "Sorry, not ready yet" );


}

void KGhostview::about()
{
	 KMsgBox::message(0, "About kghostview", 
                             "Version :	0.5 alpha\nAuthor : Mark Donohoe\nMail : donohoe@kde.org" );


}

void KGhostview::printStart()
{
    //printf("KGhostview::printStart\n");

	QString name, error;
	int selection, i;
	int copy_marked_list[1000];
	int mode=PRINT_WHOLE;
	
	selection = pd->modeComboBox->currentItem()+1;
	
	for(i=0;i<1000;i++) {
		copy_marked_list[i]=mark_page_list[i];
		mark_page_list[i]=0;
	}
	
	switch(selection) {
		case 1:	mode=PRINT_WHOLE;
				break;
				
		case 2:	mode=PRINT_MARKED;
				for(i=1;i<1000;i+=2) {
					mark_page_list[i]=1;
				}
				break;
				
		case 3:	mode=PRINT_MARKED;
				for(i=0;i<1000;i+=2)
					mark_page_list[i]=1;
				break;
				
		case 4:	mode=PRINT_MARKED;
				for(i=0;i<1000;i++)
					mark_page_list[i]=copy_marked_list[i];
				break;
	}
	
	name = pd->nameLineEdit->text();

	if (error = print_file(name, (mode == PRINT_WHOLE))) {
	    char *buf = (char *)malloc(strlen(error) +
				 strlen("Printer Name : ") + 2);
	    sprintf(buf, "%s\n%s", error.data(), "Printer Name : ");
	    fprintf(stderr, "%s\n%s\n", error.data(), "Printer Name : "); //prompt
	    free(error);
	    free(buf);
	}
	
	for(i=0;i<1000;i++) {
		mark_page_list[i]=copy_marked_list[i];
	}
}

//*********************************************************************************
//
//	POSTSCRIPT PARSING AND HANDLING FOR GHOSTSCRIPT
//
//	Look out !! This stuff is pretty messy.
//
//*********************************************************************************


QString KGhostview::print_file(QString name, Bool whole_mode)
{
    //printf("KGhostview::print_file\n");

    FILE *printer;
    SIGVAL (*oldsig)(int);
    int bytes=0;
    char buf[BUFSIZ];
    Bool failed;
    QString ret_val;

	page->disableInterpreter();

	// For SYSV, SVR4, USG printer variable=LPDEST, print command=lp

    if (name.data() != '\0') {
		setenv("PRINTER", name, True); // print variable
    }
    
    oldsig = signal(SIGPIPE, SIG_IGN);
    printer = popen("lpr", "w");	// print command
   
    if (toc_text && !whole_mode) {
		pscopydoc(printer);
		
		//printf("Done pscopydoc\n");
		
    } else {
		FILE *tmpfile = ::fopen(filename, "r");
		
		//printf("Opening file\n");
	
		while ( (bytes = ::read( ::fileno(tmpfile), buf, BUFSIZ)) ) {
			//printf("bytes written= %d\n", bytes);
	   		bytes = ::write( ::fileno(printer), buf, bytes);
	   		//printf("bytes written= %d\n", bytes);
	   	}
		::fclose(tmpfile);
		
		//printf("closed tmpfile\n");
    }
	
    failed = (pclose(printer) != 0);
	
    if (failed) {
		sprintf(buf, "Print failure : lpr"); // print error, command
		ret_val = QString(buf);
    } else {
		ret_val = NULL;
    }
	
	signal(SIGPIPE, oldsig);
	
    return(ret_val);
}

/* length calculates string length at compile time */
/* can only be used with character constants */
#define length(a) (sizeof(a)-1)

/* Copy the headers, marked pages, and trailer to fp */

void KGhostview::pscopydoc(FILE *fp)
{
    //printf("KGhostview::pscopydoc\n");

    FILE *psfile;
    char text[PSLINELENGTH];
    char *comment;
    Bool pages_written = False;
    Bool pages_atend = False;
    Bool marked_pages = False;
    int pages = 0;
    int page = 1;
    unsigned int i, j;
    long here;

    psfile = fopen(filename, "r");

    for (i = 0; i < doc->numpages; i++) {
	if (mark_page_list[i]==1) pages++;
    }

    if (pages == 0) {	/* User forgot to mark the pages */
	/* mark_page(form, NULL, NULL);
	marked_pages = True;
	for (i = 0; i < doc->numpages; i++) {
	    if (toc_text[toc_entry_length * i] == '*') pages++;
	} */
    }

    here = doc->beginheader;
    while ( (comment = pscopyuntil(psfile, fp, here,
				 doc->endheader, "%%Pages:")) ) {
	here = ftell(psfile);
	if (pages_written || pages_atend) {
	    free(comment);
	    continue;
	}
	sscanf(comment+length("%%Pages:"), "%s", text);
	if (strcmp(text, "(atend)") == 0) {
	    fputs(comment, fp);
	    pages_atend = True;
	} else {
	    switch (sscanf(comment+length("%%Pages:"), "%*d %d", &i)) {
		case 1:
		    fprintf(fp, "%%%%Pages: %d %d\n", pages, i);
		    break;
		default:
		    fprintf(fp, "%%%%Pages: %d\n", pages);
		    break;
	    }
	    pages_written = True;
	}
	free(comment);
    }
    pscopy(psfile, fp, doc->beginpreview, doc->endpreview);
    pscopy(psfile, fp, doc->begindefaults, doc->enddefaults);
    pscopy(psfile, fp, doc->beginprolog, doc->endprolog);
    pscopy(psfile, fp, doc->beginsetup, doc->endsetup);

    for (i = 0; i < doc->numpages; i++) {
	if (doc->pageorder == DESCEND) 
	    j = (doc->numpages - 1) - i;
	else
	    j = i;
	if (mark_page_list[j]==1) {
	    comment = pscopyuntil(psfile, fp, doc->pages[i].begin,
				  doc->pages[i].end, "%%Page:");
	    fprintf(fp, "%%%%Page: %s %d\n",
		    doc->pages[i].label, page++);
	    free(comment);
	    pscopy(psfile, fp, -1, doc->pages[i].end);
	}
    }

    here = doc->begintrailer;
    while ( (comment = pscopyuntil(psfile, fp, here,
				 doc->endtrailer, "%%Pages:")) ) {
	here = ftell(psfile);
	if (pages_written) {
	    free(comment);
	    continue;
	}
	switch (sscanf(comment+length("%%Pages:"), "%*d %d", &i)) {
	    case 1:
		fprintf(fp, "%%%%Pages: %d %d\n", pages, i);
		break;
	    default:
		fprintf(fp, "%%%%Pages: %d\n", pages);
		break;
	}
	pages_written = True;
	free(comment);
    }
    fclose(psfile);

    if (marked_pages) fprintf(stderr, "Should unmark?\n");
}
#undef length



Bool KGhostview::same_document_media()
{
    //printf("KGhostview::same_document_media\n");

    unsigned int i;

    if (olddoc == NULL && doc == NULL) {
    	//printf("	Null olddoc and doc\n");
    	return True;
    }
    if (olddoc == NULL || doc == NULL) return False;
    if (olddoc->nummedia != doc->nummedia) return False;
    /* Exclusive OR old document was EPS and new document is EPS */
    if ((olddoc->epsf || doc->epsf) && !(olddoc->epsf && doc->epsf)) {
    	//fprintf(stderr, "Old document was EPS and this isn't or vice versa\n");
    	return False;
    }
    for (i = 0; i < doc->nummedia; i++)
		if (strcmp(olddoc->media[i].name, doc->media[i].name)) return False;
	
    return False;
}

QString KGhostview::open_file( QString name )
{
    //printf("KGhostview::open_file\n");

    FILE *fp;
    struct stat sbuf;

	if ( name.data() == '\0' )	
		return( NULL );
	
    if (strcmp(name, "-")) {
    
		if ((fp = fopen(name, "r")) == NULL) {
		
	    	/*
	    	
	    	char *buf = (char *)malloc(strlen(open_fail) +
					  strlen(sys_errlist[errno]) + 1);
	    	sprintf(buf, open_fail);
	    	if (errno <= sys_nerr) strcat(buf, sys_errlist[errno]);
	    	
	    	*/
	    	
	    	// Error report on failure to open file not handled.
	    	
	    	//printf("Open file failed\n");
	    	
	    	QString s;
			s.sprintf( "The file\n\"%s\"\ncould not be found.", name.data());
 			KMsgBox::message(0, "Error", s, 2 );
	    	
	    	return( NULL );
	    	
		} else {

	    	oldfilename.sprintf( filename.data() );
	    	//printf("oldfilename = %s\n", oldfilename.data() );
	    	filename.sprintf( name.data() );
	    	//printf("filename = %s\n", filename.data() );
	    	if ( psfile ) {
	    		//printf("existing file -closing\n");
	    		fclose( psfile );
	    	}
	    	psfile = fp;
	    	stat( filename, &sbuf );
	    	mtime = sbuf.st_mtime;
	    	
	    	new_file(0);
	    	
	    	QString s( "KGhostview : " );
	    	s += QFileInfo( filename ).fileName();
	    	setCaption( s );
      		
	    	show_page(0);
	    	return( NULL );
		}
		
    } else {
    
		oldfilename.sprintf( filename.data() );
		filename.sprintf( name.data() );
		if ( psfile ) fclose( psfile );
		psfile = NULL;
		
		new_file(0);
		
		QString s( "KGhostview : " );
	    s += QFileInfo( filename ).fileName();
	    setCaption( s );
      	
		return(NULL);
    }
}


Bool KGhostview::set_new_orientation(int number)
{
    //printf("KGhostview::set_new_orientation\n");

    Bool changed = False;
    Bool from_doc = False;
    int new_orientation;

	//printf("******	Set new orientation\n");

    if (force_orientation) {
	new_orientation = orientation;
    } else {
	if (doc) {
	    if (toc_text && doc->pages[number].orientation != NONE) {
		new_orientation = doc->pages[number].orientation;
		from_doc = True;
	    } else if (doc->default_page_orientation != NONE) {
		new_orientation = doc->default_page_orientation;
		from_doc = True;
	    } else if (doc->orientation != NONE) {
		new_orientation = doc->orientation;
		from_doc = True;
	    } else {
		new_orientation = orientation;
	    }
	} else {
	    new_orientation = orientation;
	}
    }

    /* If orientation changed,
     * stop interpreter and setup for new orientation. */
    if (new_orientation != current_orientation) {
    	//printf("New orientation\n");
		page->disableInterpreter();
		page->orientation =  new_orientation;
		//printf("		NEW ORIENT %d\n", new_orientation);
		/* if(new_orientation == XtPageOrientationPortrait) {
			text=XmStringCreateLtoR("Portrait", XmFONTLIST_DEFAULT_TAG);
		} else if (new_orientation == XtPageOrientationLandscape) {
			text=XmStringCreateLtoR("Landscape", XmFONTLIST_DEFAULT_TAG);
		} else if (new_orientation == XtPageOrientationSeascape) {
			text=XmStringCreateLtoR("Seascape", XmFONTLIST_DEFAULT_TAG);
		} else if (new_orientation == XtPageOrientationUpsideDown) {
			text=XmStringCreateLtoR("Upside down", XmFONTLIST_DEFAULT_TAG);
		} */

		current_orientation = new_orientation;
		changed = True;
    } else {
    	//fprintf(stderr, "No new orientation\n");
    }
    
    return changed;
}

/* Set new pagemedia */
Bool KGhostview::set_new_pagemedia( int number )
{
    //printf("KGhostview::set_new_pagemedia\n");

    int new_pagemedia=0;
    int new_llx=0;
    int new_lly=0;
    int new_urx=0;
    int new_ury=0;
    Bool changed = False;
    Bool from_doc = False;

	//printf("******	Set new media\n");

    if (0 /*force_document_media*/) {
		new_pagemedia = document_media;
    } else if (0 /*app_res.force_pagemedia*/) {
		new_pagemedia = default_pagemedia;
    } else {
		if (doc) {
	    	if (toc_text && doc->pages[number].media != NULL) {
				new_pagemedia = doc->pages[number].media - doc->media;
				from_doc = True;
	    	} else if (doc->default_page_media != NULL) {
				new_pagemedia = doc->default_page_media - doc->media;
				from_doc = True;
	    	} else {
				new_pagemedia = default_pagemedia;
	    	}
		} else {
	    	new_pagemedia = default_pagemedia;
		}
    }

    /* If pagemedia changed, remove the old marker. */
    if (new_pagemedia != current_pagemedia) {
    	//fprintf(stderr, "New media\n");

		current_pagemedia = new_pagemedia;
    } else {
    	//fprintf(stderr, "No new media\n");
    }

    
    /* Compute bounding box */
    if (	1 /*!force_document_media*/ &&
    		1 /*!app_res.force_pagemedia*/ &&
			doc && doc->epsf &&
			/* Ignore malformed bounding boxes */
			(doc->boundingbox[URX] > doc->boundingbox[LLX]) &&
			(doc->boundingbox[URY] > doc->boundingbox[LLY])
	) {
		new_llx = doc->boundingbox[LLX];
		new_lly = doc->boundingbox[LLY];
		new_urx = doc->boundingbox[URX];
		new_ury = doc->boundingbox[URY];
		//text=XmStringCreateLtoR("EPS box", XmFONTLIST_DEFAULT_TAG);
		//printf("EPS box\n");
    } else {
    	//printf("Trying to compute size ! base papersize = %d\n",
    	//base_papersize);
		new_llx = new_lly = 0;
		if (new_pagemedia < base_papersize) {
	    	new_urx = doc->media[new_pagemedia].width;
	    	new_ury = doc->media[new_pagemedia].height;
	    	//printf("%s\n", doc->media[new_pagemedia].name);
		   // text=XmStringCreateLtoR(doc->media[new_pagemedia].name
	    	//	, XmFONTLIST_DEFAULT_TAG);
		} else {
	    	new_urx = papersizes[new_pagemedia-base_papersize].width;
	    	new_ury = papersizes[new_pagemedia-base_papersize].height;
	    	//printf("%s\n", papersizes[new_pagemedia-base_papersize].name);
		   // text=XmStringCreateLtoR(
		   // 	papersizes[new_pagemedia-base_papersize].name
		   // 	, XmFONTLIST_DEFAULT_TAG);
		}
    }
	
	//printf("new width = %d, current width = %d\n", new_urx, current_urx);
	//printf("new h = %d, current h = %d\n", new_ury, current_ury);
	
	//printf("new x offset %d : new y offset %d\n", new_llx, new_lly);
	
    /* If bounding box changed, setup for new size. */
    if ((new_llx != current_llx) || (new_lly != current_lly) ||
	(new_urx != current_urx) || (new_ury != current_ury)) {
		//printf("Change media\n");
		page->disableInterpreter();
		changed = True;
		current_llx = new_llx;
		current_lly = new_lly;
		current_urx = new_urx;
		current_ury = new_ury;
		page->llx = current_llx;
		page->lly = current_lly;
		page->urx = current_urx;
		page->ury = current_ury;
    }
	
	/* XtSetArg(args[0], XmNselectedItem, text);
	//XtSetValues(viewControlMediaCombo, args, ONE); */
    return changed;
}


void KGhostview::build_pagemedia_menu()
{
    //printf("KGhostview::build_pagemedia_menu\n");

    unsigned int i;
    int offset;

    if (same_document_media()) {
    	//printf("Same document media\n");
    	return;
    }
    force_document_media = False;

    /* Build the Page Media menu */
    /* the Page media menu has two parts.
     *  - the document defined page medias
     *  - the standard page media defined from Adobe's PPD
     */

    base_papersize = 0;
    if (doc) base_papersize = doc->nummedia;
    for (i = 0; papersizes[i].name; i++) {}	/* Count the standard entries */
    i += base_papersize;


	/*	if you've got an eps file need bounding box in menu too */
	
	if(doc && doc->epsf) {
		vc->mediaComboBox->insertItem("EPS Box");
		 offset=2;
	} else {
		offset=1;
	}
	

    if (doc && doc->nummedia) {
		for (i = 0; i < doc->nummedia; i++) {
			vc->mediaComboBox->insertItem(doc->media[i].name);
			//printf("%s  ", doc->media[i].name);
		}
    }
    for (i = 0; papersizes[i].name; i++) {
		if (i > 0) {
	    	/* Skip over same paper size with small imageable area */
	    	if ((papersizes[i].width == papersizes[i-1].width) &&
			(papersizes[i].height == papersizes[i-1].height)) {
			continue;
	    	}
		}
		vc->mediaComboBox->insertItem(papersizes[i].name);
		//printf("%s  ", papersizes[i].name);
    }
    //printf("\n");
}


Bool useful_page_labels;
Bool	KGhostview::setup()
{
    //printf("KGhostview::setup\n");

    int oldtoc_entry_length;
	int k;
    int this_page, last_page=0;
	char temp_text[20];
    
    
    if ( ! filename ) return False;
    
    //printf("Gone into setup\n");
    // Reset to a known state.
    psfree( olddoc );
    //printf("Freed olddoc\n");
    olddoc = doc;
    doc = NULL;
    current_page = -1;
    toc_text = 0;
    oldtoc_entry_length = toc_entry_length;
    //printf("Next - pages in part\n");
    for(k=0;k<10;k++) pages_in_part[k]=0;
    num_parts=0;
	//printf("Reset state\n");
	
    // Scan document and start setting things up
    if (psfile) {
    	//printf ("Scan file -");
    	doc = psscan(psfile);
    	//if (doc == NULL) //printf(" NULL FILE - ");
    	//printf ("scanned\n");
    }
    
    //
    // Build table of contents
    // Well, that's what it used to be called !!
    //
    if (doc && (!doc->epsf && doc->numpages > 0 ||
		 doc->epsf && doc->numpages > 1)) {
		int maxlen = 0;
		unsigned int i, j;
		useful_page_labels = False;

		if (doc->numpages == 1) useful_page_labels = True;
		for (i = 1; i < doc->numpages; i++)
			if ( (useful_page_labels = (useful_page_labels ||
				strcmp(doc->pages[i-1].label, doc->pages[i].label)))) break;
		if (useful_page_labels) {
			for (i = 0; i < doc->numpages; i++) 
			if((unsigned int)maxlen<strlen(doc->pages[i].label))
				maxlen=strlen(doc->pages[i].label);
		} else {
			double x;
			x = doc->numpages;
			maxlen = (int)( log10(x) + 1 );
		}
		toc_entry_length = maxlen + 3;
		toc_length = doc->numpages * toc_entry_length - 1;
		toc_text = 1;

		for (i = 0; i < doc->numpages;
			 i++) {
			if (useful_page_labels) {
			if (doc->pageorder == DESCEND) {
				j = (doc->numpages - 1) - i;
			} else {
				j = i;
			}
				this_page=atoi(doc->pages[j].label);
				if(last_page>this_page) {
					num_parts++;
				}
				if (num_parts<10) pages_in_part[num_parts]++;
				last_page=this_page;
			} else {
			}
		}
		page->filename = NULL;
		//printf("Set null filename for gs -- use pipe\n");
    } else {
		toc_length = 0;
		toc_entry_length = 3;
		page->filename.sprintf( filename );
		//printf("Set filename -- gs will open this file\n");
    }
	//printf("Parsed document structure\n");
	
	//printf("number of parts %d\n", num_parts);
	if (doc) {
		 if (num_parts>10 || (unsigned int)num_parts==doc->numpages) {
			num_parts=0;
			pages_in_part[0]=doc->numpages;
		}
		if(num_parts==0) {
			if(doc->numpages==0) {
				sprintf(page_total_label, "of 1    ");
			} else if(doc->numpages>0 && doc->numpages<10) {
				sprintf(page_total_label, "of %d    ", doc->numpages);
			} else if (doc->numpages<100) {
				sprintf(page_total_label, "of %d  ", doc->numpages);
			} else if (doc) {
				sprintf(page_total_label, "of %d", doc->numpages);
			} else {
				sprintf(page_total_label, "         ");
			}
		} else {
			if(pages_in_part[0]==0) {
				sprintf(page_total_label, "of 1    ");
			} else if(pages_in_part[0]>0 && pages_in_part[0]<10) {
				sprintf(page_total_label, "of %d    ", pages_in_part[0]);
			} else if (pages_in_part[0]<100) {
				sprintf(page_total_label, "of %d  ", pages_in_part[0]);
			} else if (doc) {
				sprintf(page_total_label, "of %d", pages_in_part[0]);
			} else {
				sprintf(page_total_label, "         ");
			}

			sprintf(part_total_label, "of %d", num_parts+1);
		}
	}
	//printf("Made table of contents\n");
	
	build_pagemedia_menu();
	//printf("Built pagemedia menu\n");
	
	// Reset ghostscript and output messages popup
    if (
    	!doc || !olddoc ||
		strcmp(oldfilename, filename) ||
		olddoc->beginprolog != doc->beginprolog ||
		olddoc->endprolog != doc->endprolog ||
		olddoc->beginsetup != doc->beginsetup ||
		olddoc->endsetup != doc->endsetup
	) {
		//printf("reset messages\n");

		page->disableInterpreter();
		//printf("Disabled Interpreter\n");
		
			/**************************************************************
			 *	XtUnmanageChild(infopopup);
			 *	XmTextReplace(infotext, 0, output_position, null_string);
			 *	info_up = False;
			 *	XtSetArg(args[0], XmNcursorPosition, 0);
			 *	XtSetValues(infotext, args, ONE);
			 *	output_position=0;
			 **************************************************************/
    }
	
	if(current_page==-1) current_page=0;
	
    toolbar->setItemEnabled(ID_ZOOM, TRUE);
    toolbar->setItemEnabled(ID_PRINT, TRUE);
    toolbar->setItemEnabled(ID_MARK, TRUE);
    m_file->setItemEnabled(ID_PRINT, TRUE);
    
	if(toc_text) {
		if( (unsigned int)current_page+1<doc->numpages ) {
			m_go->setItemEnabled(nextID, TRUE);
			toolbar->setItemEnabled(ID_NEXT, TRUE);
		}
		if( current_page-1>=0 ) {
			m_go->setItemEnabled(prevID, TRUE);
			toolbar->setItemEnabled(ID_PREV, TRUE);
		}
		m_go->setItemEnabled(goToPageID, TRUE);
		m_go->setItemEnabled(goToStartID, TRUE);
		m_go->setItemEnabled(goToEndID, TRUE);
		toolbar->setItemEnabled(ID_PAGE, TRUE);
		toolbar->setItemEnabled(ID_START, TRUE);
		toolbar->setItemEnabled(ID_END, TRUE);
		toolbar->setItemEnabled(ID_READ, TRUE);
	}
	m_view->setItemEnabled(viewControlID, TRUE);
	m_view->setItemEnabled(zoomInID, TRUE);
	m_view->setItemEnabled(zoomOutID, TRUE);
	m_view->setItemEnabled(shrinkWrapID, TRUE);
	m_view->setItemEnabled(redisplayID, TRUE);
	m_view->setItemEnabled(infoID, TRUE);
	
	statusbar->changeItem( filename.data(), ID_FILENAME );
	switch(orientation) {
		case 1:	statusbar->changeItem( "Portrait", ID_ORIENTATION );
				break;
		case 2:	statusbar->changeItem( "Upside down", ID_ORIENTATION );
				break;
		case 3:	statusbar->changeItem( "Seascape", ID_ORIENTATION );
				break;
		case 4:	statusbar->changeItem( "Landscape", ID_ORIENTATION );
				break;
	}
	sprintf(temp_text, "%d%%", (int)(100*page->xdpi/default_xdpi));
	statusbar->changeItem( temp_text, ID_MAGSTEP );

    //printf("Setup finished\n");
    return oldtoc_entry_length != toc_entry_length;
}

void KGhostview::changeFileRecord()
{
	QString s;
	
	s.sprintf("&1. ");
	s+=lastOpened[0];
	m_file->changeItem(s, ID_FILE_A);
	
	s.sprintf("&2. ");
	s+=lastOpened[1];
	m_file->changeItem(s, ID_FILE_B);
	
	s.sprintf("&3. ");
	s+=lastOpened[2];
	m_file->changeItem(s, ID_FILE_C);
	
	s.sprintf("&4. ");
	s+=lastOpened[3];
	m_file->changeItem(s, ID_FILE_D);
}
	

void KGhostview::new_file(int number)
{
    //printf("KGhostview::new_file\n");
	
	Bool ADD_THIS = TRUE;
	int file_count;
	
	for(file_count=0;file_count<4;file_count++) {
		if( filename == lastOpened[file_count] ) {
			ADD_THIS = FALSE;
		}
	}
	
	if( ADD_THIS) {
	
		for(file_count=3;file_count>0;file_count--) {
			lastOpened[file_count].sprintf(lastOpened[file_count-1]);
		}
		lastOpened[0].sprintf(filename);
		changeFileRecord();
		writeFileSettings();
	}
	
	Bool layout_changed = False;
	
	//page->disableInterpreter();
    if (setup()) layout_changed = True;

    /* Coerce page number to fall in range */
    if (toc_text) {
	if ((unsigned int)number >= doc->numpages) number = doc->numpages - 1;
	if (number < 0) number = 0;
    }

    if (set_new_orientation(number)) layout_changed = True;
    if (set_new_pagemedia(number)) layout_changed = True;
    if (layout_changed) {
    	//printf("Layout should change -- call KPS method\n");
    	page->layout();
    }

}

void KGhostview::show_page(int number)
{
    //printf("KGhostview::show_page\n");

    struct stat sbuf;
    Bool new_orient = FALSE, new_media = FALSE;

    if ( ! filename ) {
    	//printf("No file !\n");
    	return;
    }

    if (psfile) {
    	//printf("Oh does it go into psfile ?\n");
    	if (!stat(filename, &sbuf) && mtime != sbuf.st_mtime) {
			fclose(psfile);
			psfile = fopen(filename, "r");
			mtime = sbuf.st_mtime;
			oldfilename.sprintf( filename.data() );
			new_file(number);
		}
    }
    
    // Coerce page number to fall in range - Duh, what does that mean ?
    if (toc_text) {
		if ((unsigned int)number >= doc->numpages) number = doc->numpages - 1;
		if (number < 0) number = 0;
    }
    
    //printf("Hmm shouldn't this whole thing be inside the if(psfile) ?\n");
    
    new_orient=set_new_orientation(number);
    new_media=set_new_pagemedia(number);
    
    //printf("new_orient %d, new_media %d\n", new_orient, new_media);
    
    if (new_orient || new_media)
	{
		page->layout();
	};
    
    if (toc_text) {
		current_page = number;
		if ( page->isInterpreterReady() ) {
			//printf("Interpreter ready - Fire off next page\n");
			page->nextPage();
			
		} else {
			//printf("Start interpreter and send preamble\n");
			page->enableInterpreter();
			page->sendPS(psfile, doc->beginprolog,
				doc->lenprolog, False);
			page->sendPS(psfile, doc->beginsetup,
				doc->lensetup, False);
				
		}
		//printf("Send page %d\n", current_page);
		page->sendPS(psfile, doc->pages[current_page].begin,
			doc->pages[current_page].len, False);

  } else {
	if ( !page->isInterpreterRunning() ) {
		//printf("This is not a structured document -- start int.\n");
	    page->enableInterpreter();
	    
	} else if ( page->isInterpreterReady() ) {
	    page->nextPage();
	    
	} else XBell(page->x11Display(), 0);
	    
  }
  
  //
  // Well that takes care of sending the postscript.
  // Now update the page label on the status line
  //
  
  if(toc_text) {
		if(num_parts==0) {
			part_string.sprintf( "Sc.1 " );
			sprintf(part_total_label, "of 1    ");
			if(number==-1)
				page_string.sprintf("P.1 ");
			else
				page_string.sprintf("P.%d ", number+1);
		} else {
			int cumulative_pages=0;
			int k, part;


			for(k=0;k<10;k++) {
  				cumulative_pages+=pages_in_part[k];
  				if(cumulative_pages>number) break;
			}
			cumulative_pages-=pages_in_part[k];
			part=k-1;

			page_string.sprintf( "P.%d ", number+1-cumulative_pages);

			part_string.sprintf( "Sc.%d ", part+2);

			part=k;
			if(pages_in_part[part]==0) {
				sprintf(page_total_label, "of 1    ");
			} else if(pages_in_part[part]>0 && pages_in_part[part]<10) {
				sprintf(page_total_label, "of %d    ", pages_in_part[part]);
			} else if (pages_in_part[part]<100) {
				sprintf(page_total_label, "of %d  ", pages_in_part[part]);
			} else if (doc) {
				sprintf(page_total_label, "of %d", pages_in_part[part]);
			} else {
				sprintf(page_total_label, "         ");
			}

			sprintf(part_total_label, "of %d  ", num_parts+1);
		}

		part_label_text=operator+(part_string, part_total_label);
		page_label_text=operator+(page_string, page_total_label);
		position_label=operator+(part_label_text, page_label_text);
		statusbar->changeItem( position_label.data(), ID_LOCATION );

		if (current_page-1< 0) {
			toolbar->setItemEnabled(ID_PREV, FALSE);
			m_go->setItemEnabled(prevID, FALSE);
		} else {
			toolbar->setItemEnabled(ID_PREV, TRUE);
			m_go->setItemEnabled(prevID, TRUE);
		}
		if ((unsigned int)current_page+1 >= doc->numpages) {
			toolbar->setItemEnabled(ID_NEXT, FALSE);
			m_go->setItemEnabled(nextID, FALSE);
		} else {
			toolbar->setItemEnabled(ID_NEXT, TRUE);
			m_go->setItemEnabled(nextID, TRUE);
		}
	}
	else {
		statusbar->changeItem( "", ID_LOCATION );
	}
}

void KGhostview::set_magstep(int i)
{
    //printf("KGhostview::set_magstep\n");

	char temp_text[20];
   
    magstep = i;
    if (set_new_magstep()) {
		page->layout();
		page->resize(page->width(), page->height());
		page->repaint();
		sprintf(temp_text, "%d%%", (int)(100*page->xdpi/default_xdpi));
		statusbar->changeItem( temp_text, ID_MAGSTEP );
		show_page(current_page);
    }
}

Bool KGhostview::set_new_magstep()
{
    //printf("KGhostview::set_new_magstep\n");

    int new_magstep;
    Bool changed = False;
    float xdpi, ydpi;
    
    new_magstep = magstep;
    
    if (new_magstep != current_magstep) {
    	//printf("	new_magstep != current_magstep\n");
		page->disableInterpreter();
		changed = True;
		xdpi = default_xdpi;
		ydpi = default_ydpi;
		magnify(&xdpi, new_magstep);
		magnify(&ydpi, new_magstep);
		page->xdpi=xdpi;
		page->ydpi=ydpi;
		current_magstep = new_magstep;
	}
	return changed;
}

void KGhostview::magnify(float *dpi, int magstep)
{
    //printf("KGhostview::magnify\n");

    if (magstep < shrink_magsteps) {
		*dpi = (int)((* dpi)*magstep/(shrink_magsteps));
    } else {
		*dpi = (int)((* dpi)+2*(* dpi)*(magstep-shrink_magsteps)/(expand_magsteps));
    }
}





