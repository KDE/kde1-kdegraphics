
#include <stdlib.h>
#include <stdio.h>

#include <qfiledlg.h>
#include <kmsgbox.h>
#include <kapp.h>

#include "kghostview.h"


Display 	*kde_display;

void Syntax(char *call)
{
    printf("Usage: %s\n", call);
    printf("[filename]\n");
    printf("See Qt documentation for command line options\n");
    exit(1);
}

class MyApp : public KApplication
{
public:

  	MyApp( int &argc, char **argv, const QString &rAppName );
 	virtual bool x11EventFilter( XEvent * );
};




MyApp::MyApp( int &argc, char **argv, const QString &rAppName )
			: KApplication( argc, argv, rAppName )
{
}


bool MyApp::x11EventFilter( XEvent *ev ) {
	
	for ( KGhostview *kg = KGhostview::windowList.first(); kg!=NULL;
		kg=KGhostview::windowList.next() )
	{ 
	
	if(ev->xany.type == ClientMessage) {
		kg->page->mwin = ev->xclient.data.l[0];
		
		XClientMessageEvent *cme = ( XClientMessageEvent * ) ev;
		
		if(cme->message_type == kg->page->gs_page) {
			kg->page->busy=False;
			kg->page->fullView->setCursor( arrowCursor );
			return TRUE;
			
		} else if(cme->message_type == kg->page->done) {
			kg->page->disableInterpreter();
			return TRUE;
			
		}
	}
	}
	
	if ( KApplication::x11EventFilter( ev ) )
		return TRUE;
	else
		return FALSE;	
}

int main( int argc, char **argv )
{
	MyApp a( argc, argv, "kghostview" );
	
	struct stat sbuf;
	
	KGhostview *kg = new KGhostview ();
	CHECK_PTR( kg );
	KGhostview::windowList.append( kg );
	
	if (argc > 2) Syntax(argv[0]);
	if (argc == 2) {
	
		kg->filename = argv[1];
		if (strcmp(kg->filename, "-")) {
			if ((kg->psfile = fopen(argv[1], "r")) == NULL) {
				QString s;
				s.sprintf( "The file\n\"%s\"\ncould not be found.", argv[1]);
 				KMsgBox::message(0, "Error", 
                             s, 2 );
			}
			stat(kg->filename, &sbuf);
			kg->mtime = sbuf.st_mtime;
		} else {
			printf("Didn't recognise file\n");
		}
	}
	
	kg->setMinimumSize( 250, 250 );
	kg->setCaption( "KGhostview: Version 0.5" );
	kg->bindKeys();
	kg->updateMenuAccel();
	kg->show();	
	
	if (kg->psfile) {
		kg->setup();
		kg->show_page(0);
	}
	
	
	return a.exec();
}

