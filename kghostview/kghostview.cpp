
#include "kghostview.h"
#include "kpushbt.h"
#include <kapp.h>
#include <kmisc.h>

#include <qmsgbox.h>

extern "C" {
	#include "ps.h"
}

#include "kghostview.moc"

KGhostview::KGhostview()
{   
    //printf("KGhostview::KGhostview\n");
    
    shrink_magsteps=5;
	current_magstep=magstep=5;
	expand_magsteps=5;
	default_xdpi=75.0; default_ydpi=75.0;
	default_pagemedia=0;
	orientation=1;
	force_orientation=False;
	force_pagemedia=False;
	current_llx=0;
	current_lly=0;
	current_urx=0;
	current_ury=0;
	
	hide_toolbar=False;
	hide_statusbar=False;
    
    //
    // MENUBAR
    //
    
    m_file = new QPopupMenu;
    CHECK_PTR( m_file );
    m_file->insertItem( "Open ...",  	this, SLOT(open_new_file()) );
    printID = m_file->insertItem( "Print ...",  	this, SLOT(print()) );
    m_file->insertItem( "Quit",  		qApp, SLOT(quit()) );
    
	m_view = new QPopupMenu;
    CHECK_PTR( m_view );
	m_view->insertItem( "Zoom in", 			this, SLOT(zoom_in()) );
	m_view->insertItem( "Zoom out", 		this, SLOT(zoom_out()) );
	viewID = m_view->insertItem( "View control ...", this, SLOT(view_control()) );
	m_view->insertItem( "Redisplay", 		this, SLOT(dummy()) );
	m_view->insertItem( "Info ...", 		this, SLOT(dummy()) );

    m_go = new QPopupMenu;
    CHECK_PTR( m_go );
    nextID = m_go->insertItem( "Next page", 		this, SLOT(next_page()) );
    prevID = m_go->insertItem( "Previous Page", 	this, SLOT(prev_page()) );
    goID = m_go->insertItem( "Go to page ...", this, SLOT(go_to_page()) );
    
    m_options = new QPopupMenu;
    CHECK_PTR( m_options );
    
    m_options->insertItem("");
    messagesID = m_options->insertItem( "Show Ghostscript messages" );
    m_options->insertItem("");
    m_options->insertItem("");
    m_options->insertItem("");
    
    m_options->insertSeparator();
    m_options->insertItem("Save Options");
    
    connect(m_options, SIGNAL(activated(int)), SLOT(options_menu_activated(int)));
    
    m_help = new QPopupMenu;
    CHECK_PTR( m_help );
	m_help->insertItem( "About this ...", this, SLOT(about()) );
	m_help->insertItem( "Kghostview help", this, SLOT(help()) );
	
    menubar = new QMenuBar( this );
    CHECK_PTR( menubar );
    
    /*  
    
    menubar->setFrameStyle( QFrame::Box | QFrame::Sunken );
    menubar->setMidLineWidth(0);
    menubar->setLineWidth(1);
    
    No more Office 97 style menubar
    
    */
    
    menubar->insertItem( "&File", m_file );
    menubar->insertItem( "&View", m_view );
    menubar->insertItem( "&Go", m_go );
    menubar->insertItem( "&Options", m_options);
    menubar->insertSeparator();
    menubar->insertItem( "&Help", m_help);
	
	m_file->setItemEnabled(printID, False);
	m_go->setItemEnabled(nextID, False);
	m_go->setItemEnabled(prevID, False);
	m_go->setItemEnabled(goID, False);
	m_view->setItemEnabled(viewID, False);
	
	
	//
	// TOOLBAR
	//
	
	initToolBar();
	
	//
    // MAIN WINDOW
    //
	
	frame = new QFrame( this );
	CHECK_PTR( frame );
    frame ->setFrameStyle( QFrame::Panel | QFrame::Sunken);
    frame ->setLineWidth(2);
    
    page = new KPSWidget(this);
    CHECK_PTR( page );
    		
    //
    // STATUSBAR
    //
    
    
    statusPageLabel = new QLabel( this, "statusPageLabel" );
    CHECK_PTR( statusPageLabel );
    statusPageLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusPageLabel->setAlignment( AlignLeft|AlignVCenter );
    
    statusZoomLabel = new QLabel( this, "statusZoomLabel" );
    CHECK_PTR( statusZoomLabel );
    statusZoomLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusZoomLabel->setAlignment( AlignLeft|AlignVCenter );
    
    statusOrientLabel = new QLabel( this, "statusOrientLabel" );
    CHECK_PTR( statusOrientLabel );
    statusOrientLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusOrientLabel->setAlignment( AlignLeft|AlignVCenter );
    
    statusMiscLabel = new QLabel( this, "statusMiscLabel" );
    CHECK_PTR( statusMiscLabel );
    statusMiscLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusMiscLabel->setAlignment( AlignLeft|AlignVCenter );
  
  	//
  	//	PRINTING DIALOG
  	//
  
	pd = new PrintDialog( 0, "print dialog" );
	
	//
	//	VIEWCONTROL DIALOG
	//
	
	vc = new ViewControl( 0, "view control" );
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
	//	READ SETTINGS AND SET OPTIONS MENU
	//
	
	readSettings();
	
	if ( page->antialias )
    	m_options->changeItem( "Turn antialiasing off", 0 );
    else
    	m_options->changeItem( "Turn antialiasing on", 0 );
    
    if ( fitWindow )
    	m_options->changeItem( "Allow window resizing", 2 );
    else
    	m_options->changeItem( "Fit window to page", 2 );
    	
    if ( hide_toolbar )	
    	m_options->changeItem( "Show tool bar", 3 );
    else
    	m_options->changeItem( "Hide tool bar", 3 );
    	
    if ( hide_statusbar )
    	m_options->changeItem( "Show status bar", 4 );
    else
    	m_options->changeItem( "Hide status bar", 4 );
	
	//
	//	HARDCODE KEYBOARD ACCELERATORS
	//
	
	 QAccel *a = new QAccel( this );
	     
     a->connectItem( a->insertItem(Key_Q+CTRL), qApp, SLOT( quit() ) );
     a->connectItem( a->insertItem(Key_G+CTRL), this, SLOT( go_to_page() ) );
     a->connectItem( a->insertItem(Key_Plus), this, SLOT( zoom_in() ) );
     a->connectItem( a->insertItem(Key_Minus), this, SLOT( zoom_out() ) );
     a->connectItem( a->insertItem(Key_Next), this, SLOT( next_page() ) );
     a->connectItem( a->insertItem(Key_Prior), this, SLOT( prev_page() ) );
     a->connectItem( a->insertItem(Key_Up), this, SLOT( scroll_up() ) );
     a->connectItem( a->insertItem(Key_Down), this, SLOT( scroll_down() ) );
     a->connectItem( a->insertItem(Key_Left), this, SLOT( scroll_left() ) );
     a->connectItem( a->insertItem(Key_Right), this, SLOT( scroll_right() ) );
     
     //
     //	RESIZE
     //
     
     resize( options_width, options_height );
}

void KGhostview::readSettings()
{
	//printf("KGhostview::readSettings\n");
	
	QString str;

	KConfig *config = KApplication::getKApplication()->getConfig();
	
	config->setGroup( "General" );
	
	str = config->readEntry( "Antialiasing" );
	if ( !str.isNull() && str.find( "on" ) == 0 )
		page->antialias = TRUE;
	else
		page->antialias = FALSE;
		
	str = config->readEntry( "Fit to window" );
	if ( !str.isNull() && str.find( "yes" ) == 0 )
		fitWindow = TRUE;
	else
		fitWindow = FALSE;
	
	str = config->readEntry( "Statusbar" );
	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_statusbar = TRUE;
		statusPageLabel->hide();
		statusZoomLabel->hide();
		statusOrientLabel->hide();
		statusMiscLabel->hide();
	} else
		hide_statusbar = FALSE;
	
	str = config->readEntry( "Toolbar" );
	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_toolbar = TRUE;
		toolbar->hide();
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
	
	/*
	
	if ( !str.isNull() )
		saverLocation = str;
	else
	{
		char *pKDE = getenv( "KDEDIR" );
		if ( pKDE )
		{
			saverLocation = pKDE;
			saverLocation += "/bin";
		}
	}
	
	*/
}

void KGhostview::writeSettings()
{
	//printf("KGhostview::writeSettings\n");
	
	changed = TRUE;
	if ( !changed )
		return;

	KConfig *config = KApplication::getKApplication()->getConfig();
	
	config->setGroup( "General" );
	
	QString str1;
	str1.setNum( width() );
	config->writeEntry( "Width", str1 );
	QString str2;
	str2.setNum( height() );
	config->writeEntry( "Height", str2 );

	config->writeEntry( "Antialiasing", 
						page->antialias ? QString( "on" ) : QString( "off" ) );
	config->writeEntry( "Fit to window", 
						fitWindow ? QString( "yes" ) : QString( "no" ) );
	config->writeEntry( "Statusbar", 
						hide_statusbar ? QString( "off" ) : QString( "on" ) );
	config->writeEntry( "Toolbar", 
						hide_toolbar ? QString( "off" ) : QString( "on" ) );

	changed = FALSE;
}

void KGhostview::changeFonts(const QFont &newFont)
{
    //printf("KGhostview::changeFonts\n");

	m_file->setFont(newFont);
	m_view->setFont(newFont);
	m_go->setFont(newFont);
	m_options->setFont(newFont);
	m_help->setFont(newFont);
	menubar->setFont(newFont);
}

void KGhostview::enableToolbarButton(  int id, bool enable, QButtonGroup *toolbar )
{
    //printf("KGhostview::enableToolbarButton\n");

    if ( toolbar == 0L )
	return;
    
    //printf("+++++++++++++++++ 2\n");

    if ( !toolbar->isA( "QButtonGroup" ) )
	return;
    
    //printf("+++++++++++++++++ 1\n");
    
    QButtonGroup *grp = (QButtonGroup*) toolbar;
    QButton *b = grp->find( id );

        
    b->setEnabled( enable );
    b->setPixmap( toolbarPixmaps[ id*2 + (enable ? 0 : 1) ] );
}

void KGhostview::initToolBar()
{
    //printf("KGhostview::initToolBar\n");
   
	QString PIXDIR( PICS_PATH );
   
    toolbarPixmaps[0].load( PIXDIR+"back.xpm" );
    toolbarPixmaps[1].load( PIXDIR+"back_gray.xpm" );
    toolbarPixmaps[2].load( PIXDIR+"forward.xpm" );
    toolbarPixmaps[3].load( PIXDIR+"forward_gray.xpm" );
    toolbarPixmaps[4].load( PIXDIR+"page.xpm" );
    toolbarPixmaps[5].load( PIXDIR+"page_gray.xpm" );
    toolbarPixmaps[6].load( PIXDIR+"viewzoom.xpm" );
    toolbarPixmaps[7].load( PIXDIR+"viewzoom_gray.xpm" );
    toolbarPixmaps[8].load( PIXDIR+"fileprint.xpm" );
    toolbarPixmaps[9].load( PIXDIR+"fileprint_gray.xpm" );
    toolbarPixmaps[10].load( PIXDIR+"tick.xpm" );
    toolbarPixmaps[11].load( PIXDIR+"tick.xpm" );
    toolbarPixmaps[12].load( PIXDIR+"start.xpm" );
    toolbarPixmaps[13].load( PIXDIR+"start_gray.xpm" );
    toolbarPixmaps[14].load( PIXDIR+"finish.xpm" );
    toolbarPixmaps[15].load( PIXDIR+"finish_gray.xpm" );
    toolbarPixmaps[16].load( PIXDIR+"next.xpm" );
    toolbarPixmaps[17].load( PIXDIR+"next_gray.xpm" );
 
    int	pos = 3, buttonWidth, buttonHeight;
    KPushButton *pb;

    buttonWidth = BUTTON_WIDTH;
    buttonHeight = BUTTON_HEIGHT;

    toolbar = new QButtonGroup( this );
    	
    toolbar->setGeometry( 0, menubar->frameGeometry().height()-1,
    	20+9*buttonWidth+3*BUTTON_SEPARATION, 32 );
    toolbar->setFrameStyle( QFrame::Panel | QFrame::Raised );
    toolbar->setLineWidth(2);
    
    QFrame *decoration;
    decoration = new QFrame(  toolbar );
    decoration->setFrameStyle( QFrame::VLine | QFrame::Raised );
    decoration->setMidLineWidth(1);
    decoration->setGeometry( pos, 3, 4, toolbar->height()-8 );
    pos+=4;
    
    decoration = new QFrame(  toolbar );;
    decoration->setFrameStyle( QFrame::VLine | QFrame::Raised );
    decoration->setMidLineWidth(1);
    decoration->setGeometry( pos, 3, 4, toolbar->height()-8 );
    pos+=9;
    
    int y_off;
    y_off=(toolbar->height()-buttonHeight)/2;
    
    pb = new KPushButton( "Prev", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( prev_page() ) );
    pos += buttonWidth ;
    
    pb = new KPushButton( "Next", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( next_page() ) );
    pos += buttonWidth ;

    pb = new KPushButton( "Page", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( go_to_page() ) );
    pos += buttonWidth + BUTTON_SEPARATION ;

	decoration = new QFrame(  toolbar );
	decoration->setFrameStyle( QFrame::VLine | QFrame::Sunken );
    decoration->setMidLineWidth(0);
    decoration->setGeometry( pos-4, 2, 2, toolbar->height()-4 );

    pb = new KPushButton( "View", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( view_control() ) );
    pos += buttonWidth + BUTTON_SEPARATION;
    
    decoration = new QFrame(  toolbar );
	decoration->setFrameStyle( QFrame::VLine | QFrame::Sunken );
    decoration->setMidLineWidth(0);
    decoration->setGeometry( pos-4, 2, 2, toolbar->height()-4 );
    
    pb = new KPushButton( "Print", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( print() ) );
    pos += buttonWidth;
    
    pb = new KPushButton( "Mark", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( dummy() ) );
    pos += buttonWidth+ BUTTON_SEPARATION;
    
    decoration = new QFrame(  toolbar );
	decoration->setFrameStyle( QFrame::VLine | QFrame::Sunken );
    decoration->setMidLineWidth(0);
    decoration->setGeometry( pos-4, 2, 2, toolbar->height()-4 );
    
    pb = new KPushButton( "Prev", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( prev_page() ) );
    pos += buttonWidth ;
    
    pb = new KPushButton( "Next", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( next_page() ) );
    pos += buttonWidth ;

    pb = new KPushButton( "Page", toolbar );
    pb->setGeometry( pos, y_off, buttonWidth, buttonHeight );
    connect( pb, SIGNAL( clicked() ), SLOT( go_to_page() ) );
    pos += buttonWidth + BUTTON_SEPARATION ;
    
    enableToolbarButton( 0, FALSE, toolbar );
    enableToolbarButton( 1, FALSE, toolbar );
    enableToolbarButton( 2, FALSE, toolbar );
    enableToolbarButton( 3, FALSE, toolbar );
    enableToolbarButton( 4, FALSE, toolbar );
    enableToolbarButton( 5, FALSE, toolbar );
    enableToolbarButton( 6, FALSE, toolbar );
    enableToolbarButton( 7, FALSE, toolbar  );
    enableToolbarButton( 8, FALSE, toolbar  );

    topOffset =
    	menubar->frameGeometry().height() + toolbar->frameGeometry().height()
    	+ 5;
    
}


void KGhostview::resizeEvent(QResizeEvent *)
{
    //printf("KGhostview::resizeEvent\n");
    
    toolbar->setGeometry( 0, menubar->frameGeometry().height(),
    20+9*BUTTON_WIDTH+3*BUTTON_SEPARATION, toolbar->height() );
    
    if(hide_toolbar)
    	topOffset = menubar->frameGeometry().height()-2;
    else
    	topOffset =
    		menubar->frameGeometry().height() +
    		toolbar->frameGeometry().height()-2;

	if(hide_statusbar)
		bottomOffset =  0;
	else
		bottomOffset =  STATUSBAR_HEIGHT;
		
    frame->setGeometry(0, 
    	topOffset,
    	width(), height()-
    	(topOffset+bottomOffset));
    
    page->setGeometry(frame->x()+2,frame->y()+2,frame->width()-4,frame->height()-4);
    
    statusPageLabel->setGeometry( PAGE_X_OFFSET,
    	height()-STATUSBAR_HEIGHT, 
    			STATUSBAR_PAGE_WIDTH, STATUSBAR_HEIGHT );
    			
    statusZoomLabel->setGeometry( 2*PAGE_X_OFFSET+STATUSBAR_PAGE_WIDTH,
    	height()-STATUSBAR_HEIGHT,
    			STATUSBAR_ZOOM_WIDTH, STATUSBAR_HEIGHT );
    			
   statusOrientLabel->setGeometry(
   		3*PAGE_X_OFFSET+STATUSBAR_PAGE_WIDTH+STATUSBAR_ZOOM_WIDTH,
    	height()-STATUSBAR_HEIGHT,
    			STATUSBAR_ORIENT_WIDTH, STATUSBAR_HEIGHT ); 
    	
    statusMiscLabel->setGeometry(
    	4*PAGE_X_OFFSET+
    		STATUSBAR_PAGE_WIDTH+STATUSBAR_ZOOM_WIDTH+STATUSBAR_ORIENT_WIDTH,
    	height()-STATUSBAR_HEIGHT, 
    			width()-
    				(4*PAGE_X_OFFSET+
    					STATUSBAR_PAGE_WIDTH+
    					STATUSBAR_ZOOM_WIDTH+STATUSBAR_ORIENT_WIDTH),
    				STATUSBAR_HEIGHT );
}

//*********************************************************************************
//
//	SLOTS
//
//*********************************************************************************

void KGhostview::scroll_up()
{
	page->scrollUp();
}

void KGhostview::scroll_down()
{
	page->scrollDown();
}

void KGhostview::scroll_left()
{
	page->scrollLeft();
}

void KGhostview::scroll_right()
{
	page->scrollRight();
}

void KGhostview::help()
{
	if ( fork() == 0 )
	{
		QString path = DOCS_PATH;
		path += "/kghostview.html";
		execlp( "kdehelp", "kdehelp", path.data(), 0 );
		exit( 1 );
	}
}


void KGhostview::apply_view_changes()
{
    //printf("KGhostview::apply_view_changes\n");

	int orient=1;
	int selection;
	Bool layout_changed=False;
	
	selection = vc->orientComboBox->currentItem()+1;
	switch(selection) {
		case 1:	orient=1;
				break;
		case 2:	orient=4;
				statusOrientLabel->setText( "Landscape" );
				break;
		case 3:	orient=3;
				statusOrientLabel->setText( "Seascape" );
				break;
		case 4:	orient=2;
				statusOrientLabel->setText( "Upside down" );
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

void KGhostview::go_to_page()
{
    //printf("KGhostview::go_to_page\n");

	GoTo gt(0, "Go to");
	gt.setCaption("Go to");
	
	if( gt.exec() ) {
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


void KGhostview::view_control()
{
    //printf("KGhostview::view_control\n");

	vc->setCaption("View control");
	vc->magComboBox->setCurrentItem(magstep-1);
	
	if( vc->exec() && doc )
		apply_view_changes();
}

void KGhostview::zoom_in()
{
	int i;

    i = magstep + 1;
    if (i <= shrink_magsteps+expand_magsteps) {
		set_magstep(i);
	}
}

void KGhostview::zoom_out()
{
	int i;

    i = magstep - 1;
    if (i >= 1) {
		set_magstep(i);
	}
}

void KGhostview::next_page()
{
    //printf("KGhostview::next_page\n");

    int new_page = 0;

    if (toc_text) {
	    new_page = current_page + 1;
		if ((unsigned int) new_page >= doc->numpages)
			return;
    }
    show_page(new_page);
    page->scrollTop();
}

void KGhostview::prev_page()
{
    //printf("KGhostview::prev_page\n");

    int new_page = 0;
	
    if (toc_text) {
	    new_page = current_page - 1;
		if (new_page < 0)
			return;
    }
    show_page(new_page);
    page->scrollTop();
}

void KGhostview::open_new_file()
{
    printf("KGhostview::open_new_file\n");
	
	QString dir;
	if ( filename )
		dir = QFileInfo( filename ).dirPath();	
	
	QString s = QFileDialog::getOpenFileName( dir, "*.*ps", this);
	if ( s.isNull() )
		return;
    
    open_file(s);
}


void KGhostview::options_menu_activated(int item)
{
    //printf("KGhostview::options_menu_activated\n");
    //printf("item= %d\n", item);

	switch (item){
  		case 0:
  			if(page->antialias) {
  				page->antialias = FALSE;
  				m_options->changeItem("Turn antialiasing on", item);
  			} else {
  				m_options->changeItem("Turn antialiasing off", item);
  				page->antialias = TRUE;
  			}
			page->disableInterpreter();
			show_page(current_page);
			break;

		case 1:
		case 2:
			QMessageBox::message("Version 0.3", "Sorry, not ready yet" );
			break;
		case 3:
			if(hide_toolbar) {
				hide_toolbar=False;
				toolbar->show();
				resize( width(), height() );
				m_options->changeItem("Hide tool bar", item);
			} else {
				hide_toolbar=True;
				toolbar->hide();
				resize( width(), height() );
				m_options->changeItem("Show tool bar", item);
			}
			break;
		case 4:
			if(hide_statusbar) {
				hide_statusbar=False;
				statusPageLabel->show();
				statusZoomLabel->show();
				statusOrientLabel->show();
				statusMiscLabel->show();
				resize( width(), height() );
				m_options->changeItem("Hide status bar", item);
			} else {
				hide_statusbar=True;
				statusPageLabel->hide();
				statusZoomLabel->hide();
				statusOrientLabel->hide();
				statusMiscLabel->hide();
				resize( width(), height() );
				m_options->changeItem("Show status bar", item);
			}
			break;
		case 6:
			writeSettings();
	}
}		


void KGhostview::dummy()
{
	 QMessageBox::message( "Version 0.3", 
                             "Sorry, not ready yet", 
                             "OK" );


}

void KGhostview::about()
{
	 QMessageBox::message( "About this ...", 
                             "Version :	0.3 alpha\nAuthor : Mark Donohoe\nMail : donohoe@kde.org", 
                             "OK" );


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
				//fprintf(stderr, "Even\n");
				break;
				
		case 3:	mode=PRINT_MARKED;
				for(i=0;i<1000;i+=2)
					mark_page_list[i]=1;
				//fprintf(stderr, "Odd\n");
				break;
				
		case 4:	mode=PRINT_MARKED;
				for(i=0;i<1000;i++)
					mark_page_list[i]=copy_marked_list[i];
				//fprintf(stderr, "Marked\n");
				break;
	}
	
	name = pd->nameLineEdit->text();

	if (error = print_file(name, (mode == PRINT_WHOLE))) {
	    char *buf = (char *)malloc(strlen(error) +
				 strlen("Printer Name : ") + 2);
	    sprintf(buf, "%s\n%s", error.data(), "Printer Name : ");
	    //fprintf(stderr, "%s\n%s\n", error.data(), "Printer Name : "); //prompt
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
    int bytes;
    char buf[BUFSIZ];
    Bool failed;
    QString ret_val;

// For SYSV, SVR4, USG printer variable=LPDEST, print command=lp

    if (name.data() != '\0') {
		setenv("PRINTER", name, True); // print variable
    }
    
    oldsig = signal(SIGPIPE, SIG_IGN);
    printer = popen("lpr", "w");	// print command
    
    if (toc_text && !whole_mode) {
		pscopydoc(printer);
    } else {
		FILE *psfile = fopen(filename, "r");
	
		while ( (bytes = read(fileno(psfile), buf, BUFSIZ)) )
	   		bytes = write(fileno(printer), buf, bytes);
		fclose(psfile);
    }

    failed = pclose(printer) != 0;

    if (failed) {
		sprintf(buf, "Print failure : lpr"); // print error, command
		ret_val = QString(buf);
    } else {
		ret_val = NULL;
    }

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
    printf("KGhostview::open_file\n");

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
	    	
	    	printf("Open file failed\n");
	    	return( NULL );
	    	
		} else {

		 filename.sprintf( name.data() );
	    	oldfilename.sprintf( filename.data() );
	    	if (psfile) fclose(psfile);
	    	psfile = fp;
	    	stat(filename, &sbuf);
	    	mtime = sbuf.st_mtime;
	    	
	    	new_file(0);
	    	
	    	if (doc) {
	    		if(doc->title) {
	    			/*
	    			
	    			 You can add the document title to the window title if
	    			 you feel like it
	    			 
	    			 */
      			}
      		}
      		
	    	show_page(0);
	    	return( NULL );
		}
		
    } else {
    
		oldfilename.sprintf( filename.data() );
		filename.sprintf( name.data() );
		if (psfile) fclose(psfile);
		psfile = NULL;
		
		new_file(0);
		
		if(doc) {
      		if(doc->title) {
	    		/*

	    		Put doc->title in the window title if you like

	    		*/
      		}
      	}
      	
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

    int new_pagemedia;
    int new_llx;
    int new_lly;
    int new_urx;
    int new_ury;
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
    
    
    if(! filename) return False;
    
    //printf("Gone into setup\n");
    // Reset to a known state.
    psfree(olddoc);
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
    	//if (doc == NULL) printf(" NULL FILE - ");
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
	
    enableToolbarButton( 3, TRUE, toolbar  );
    enableToolbarButton( 4, TRUE, toolbar   );
    enableToolbarButton( 5, TRUE, toolbar   );
    m_file->setItemEnabled(printID, TRUE);
	if(toc_text) {
		if( (unsigned int)current_page+1<doc->numpages ) {
			m_go->setItemEnabled(nextID, TRUE);
			enableToolbarButton( 1, TRUE, toolbar   );
		}
		if( current_page-1>=0 ) {
			m_go->setItemEnabled(prevID, TRUE);
			enableToolbarButton( 0, TRUE, toolbar   );
		}
		m_go->setItemEnabled(goID, TRUE);
		enableToolbarButton( 2, TRUE, toolbar   );
	}
	m_view->setItemEnabled(viewID, TRUE);
	
	statusMiscLabel->setText(filename);
	switch(orientation) {
		case 1:	statusOrientLabel->setText( "Portrait" );
				break;
		case 2:	statusOrientLabel->setText( "Upside down" );
				break;
		case 3:	statusOrientLabel->setText( "Seascape" );
				break;
		case 4:	statusOrientLabel->setText( "Landscape" );
				break;
	}
	sprintf(temp_text, "%d%%", (int)(100*page->xdpi/default_xdpi));
	statusZoomLabel->setText(temp_text);

    //printf("Setup finished\n");
    return oldtoc_entry_length != toc_entry_length;
}


void KGhostview::new_file(int number)
{
    //printf("KGhostview::new_file\n");

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

    if (!filename) {
    	printf("No file !\n");
    	return;
    }

    if (psfile) {
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
		statusPageLabel->setText( position_label );

		if (current_page-1< 0) {
			enableToolbarButton(0, FALSE, toolbar  );
			m_go->setItemEnabled(prevID, FALSE);
		} else {
			enableToolbarButton(0, TRUE, toolbar  );
			m_go->setItemEnabled(prevID, TRUE);
		}
		if ((unsigned int)current_page+1 >= doc->numpages) {
			enableToolbarButton(1, FALSE, toolbar  );
			m_go->setItemEnabled(nextID, FALSE);
		} else {
			enableToolbarButton(1, TRUE, toolbar  );
			m_go->setItemEnabled(nextID, TRUE);
		}
	}
	else statusPageLabel->setText( "" );
}

void KGhostview::set_magstep(int i)
{
    //printf("KGhostview::set_magstep\n");

	char temp_text[20];
   
    magstep = i;
    if (set_new_magstep()) {
		page->layout();
		page->setGeometry(frame->x()+2,frame->y()+2,frame->width()-4,frame->height()-4);
		page->repaint();
		sprintf(temp_text, "%d%%", (int)(100*page->xdpi/default_xdpi));
		statusZoomLabel->setText(temp_text);
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





