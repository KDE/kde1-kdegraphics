
#include <stdlib.h>
#include <stdio.h>

#include <qfiledlg.h>
#include <qmsgbox.h>
#include <kapp.h>

#include "kghostview.h"

FILE		*psfile;
QString 	filename;	
QString		oldfilename;	
int			current_page;
int 		pages_in_part[10];
int 		num_parts;
time_t		mtime;		
struct 		document *doc;	
struct 		document *olddoc;
Atom 		gs_colors, ghostview, next, gs_page, done, KDEChangePalette,
					KDEChangeGeneral;
Display 	*gs_display;
Window 		gs_window;
Window 		mwin;
Bool 		busy;
Display 	*kde_display;

void Syntax(char *call)
{
    printf("Usage: %s\n", call);
    printf("[filename]\n");
    printf("See Qt documentation for command line options\n");
    exit(1);
}

class MyApp : public KApplication {
public:
  	MyApp( int &argc, char **argv );
 	virtual bool x11EventFilter( XEvent * );
  	KGhostview *kgv;
  	
  	void changePalette();
  	void changeGeneral();
  	void readSettings();
  
  	QColor inactiveTitleColor;
	QColor inactiveTextColor;
	QColor activeTitleColor;
	QColor activeTextColor;
	QColor backgroundColor;
	QColor textColor;
	QColor selectColor;
	QColor selectTextColor;
	QColor windowColor;
	QColor windowTextColor;
	QFont generalFont;
	GUIStyle applicationStyle;
};




MyApp::MyApp( int &argc, char **argv ) : KApplication( argc, argv )
{
	struct stat sbuf;
	
	kde_display = XOpenDisplay( NULL );
	KDEChangePalette = XInternAtom( kde_display, "KDEChangePalette", FALSE);
	KDEChangeGeneral = XInternAtom( kde_display, "KDEChangeGeneral", FALSE);

	readSettings();	
	changePalette();
	changeGeneral();
	
	if (argc > 2) Syntax(argv[0]);
	if (argc == 2) {
		filename = argv[1];
		if (strcmp(filename, "-")) {
			if ((psfile = fopen(argv[1], "r")) == NULL) {
				perror(argv[1]);
				exit(1);
			}
			stat(filename, &sbuf);
			mtime = sbuf.st_mtime;
		} else {
			printf("Didn't recognise file\n");
		} 
	}

	kgv = new KGhostview;
	CHECK_PTR( kgv );
	
	kgv->setMinimumSize( 250, 250 );
	kgv->setCaption( "Kghostview: Version 0.3.3" );
	
	setMainWidget( kgv );
	kgv->show();	
}


bool MyApp::x11EventFilter( XEvent *ev ) {
	
	if(ev->xany.type == ClientMessage) {
		mwin = ev->xclient.data.l[0];
		
		XClientMessageEvent *cme = ( XClientMessageEvent * ) ev;
		
		if(cme->message_type == gs_page) {
			busy=False;
			kgv->page->setCursor( arrowCursor );
			kgv->page->killTimers();
			kgv->page->repaint( False );
			return False;
		} else if(cme->message_type == done) {
			kgv->page->killTimers();
			kgv->page->disableInterpreter();
			kgv->page->repaint( False );
			return False;
		
		// These two event handlers act on messages sent by KDisplay-0.5+
		
		} else if(cme->message_type == KDEChangePalette) {
	
			kgv->page->disableInterpreter();
			readSettings();
			changePalette();

			return False;
						
		} else if(cme->message_type == KDEChangeGeneral) {
		
			kgv->page->disableInterpreter();
			readSettings();
			changeGeneral();
			changePalette();
			kgv->resize(kgv->width(),kgv->height());
			
			return False;
			
		} else
			KApplication::x11EventFilter( ev );
	} else
		KApplication::x11EventFilter( ev );
  
  return FALSE;
}

void MyApp::readSettings()
{
	//printf("MyApp::readSettings\n");
	
	// use the global config files
	KConfig config;
	
	QString str;
	
	// This is the default light gray for KDE
	QColor col;
	col.setRgb(214,214,214);


	// Read the color scheme group from config file
	// If unavailable set color scheme to KDE default
	
	config.setGroup( "Color Scheme");
	str = config.readEntry( "InactiveTitleBarColor" );
	if ( !str.isNull() )
		inactiveTitleColor.setNamedColor( str );
	else
		inactiveTitleColor = gray;
		
	str = config.readEntry( "InactiveTitleTextColor" );
	if ( !str.isNull() )
		inactiveTextColor.setNamedColor( str );
	else
		inactiveTextColor = col;
		
	str = config.readEntry( "ActiveTitleBarColor" );
	if ( !str.isNull() )
		activeTitleColor.setNamedColor( str );
	else
		activeTitleColor = black;
		
	str = config.readEntry( "ActiveTitleTextColor" );
	if ( !str.isNull() )
		activeTextColor.setNamedColor( str );
	else
		activeTextColor = white;
		
	str = config.readEntry( "TextColor" );
	if ( !str.isNull() )
		textColor.setNamedColor( str );
	else
		textColor = black;
		
	str = config.readEntry( "BackgroundColor" );
	if ( !str.isNull() )
		backgroundColor.setNamedColor( str );
	else
		backgroundColor = col;
		
	str = config.readEntry( "SelectColor" );
	if ( !str.isNull() )
		selectColor.setNamedColor( str );
	else
		selectColor = black;	
	
	str = config.readEntry( "SelectTextColor" );
	if ( !str.isNull() )
		selectTextColor.setNamedColor( str );
	else
		selectTextColor = white;
		
	str = config.readEntry( "WindowColor" );
	if ( !str.isNull() )
		windowColor.setNamedColor( str );
	else
		windowColor = white;
		
	str = config.readEntry( "WindowTextColor" );
	if ( !str.isNull() )
		windowTextColor.setNamedColor( str );
	else
		windowTextColor = black;
	
	
	//	Read the font specification from config.
	// 	Initialize fonts to default first or it won't work !!
		
	generalFont = QFont("helvetica", 12, QFont::Normal);
	
	config.setGroup( "General Font" );
	str = config.readEntry( "Family" );
	if ( !str.isNull() )
		generalFont.setFamily(str.data());
	
	str = config.readEntry( "Point Size" );
		if ( !str.isNull() )
		generalFont.setPointSize(atoi(str.data()));
	
	str = config.readEntry( "Weight" );
		if ( !str.isNull() )
	generalFont.setWeight(atoi(str.data()));
		
	str = config.readEntry( "Italic" );
		if ( !str.isNull() )
			if ( atoi(str.data()) != 0 )
				generalFont.setItalic(True);
				
	// Finally, read GUI style from config.
	
	config.setGroup( "GUI Style" );
	str = config.readEntry( "Style" );
	if ( !str.isNull() )
	{
		if( str == "Windows 95" )
			applicationStyle=WindowsStyle;
		else
			applicationStyle=MotifStyle;
	} else
			applicationStyle=MotifStyle;	
	
}

void MyApp::changePalette()
{
	//printf("MyApp::changePalette\n");
	
	// WARNING : QApplication::setPalette() produces inconsistent results.
	// There are 3 problems :-
	// 1) You can't change select colors
	// 2) You need different palettes to apply the same color scheme to
	//		different widgets !!
	// 3) Motif style needs a different palette to Windows style.
	
	if ( applicationStyle==MotifStyle ) {
		QColorGroup disabledgrp( textColor, backgroundColor, 
    							backgroundColor.light(),
    							backgroundColor.dark(), 
    							backgroundColor.dark(120),
                        	gray, windowColor );

    	QColorGroup colgrp( textColor, backgroundColor, 
    							backgroundColor.light(),
    							backgroundColor.dark(), 
    							backgroundColor.dark(120),
                        	textColor, windowColor );

    	QApplication::setPalette( QPalette(colgrp,disabledgrp,colgrp), TRUE );
    } else {
    	QColorGroup disabledgrp( textColor, backgroundColor, 
    							backgroundColor.light(150),
    							backgroundColor.dark(), 
    							backgroundColor.dark(120),
                        	gray, windowColor );

    	QColorGroup colgrp( textColor, backgroundColor, 
    							backgroundColor.light(150),
    							backgroundColor.dark(), 
    							backgroundColor.dark(120),
                        	textColor, windowColor );

    	QApplication::setPalette( QPalette(colgrp,disabledgrp,colgrp), TRUE );
	}
}

void MyApp::changeGeneral()
{   
    //printf("MyApp::changeGeneral\n");
    
    QApplication::setStyle( applicationStyle );
    
    // 	setStyle() works pretty well but may not change the style of combo
    //	boxes.
    
    QApplication::setFont( generalFont, TRUE );
    
    // setFont() works every time for me !
}	

int main( int argc, char **argv )
{
	
	MyApp a( argc, argv );
	
	if (psfile) {
		a.kgv->setup();
		a.kgv->show_page(0);
	}

	return a.exec();
}

