/****************************************************************************
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <qfiledlg.h>
#include <kmsgbox.h>
#include <klocale.h>
#include <kapp.h>
#include <kurl.h>

#include "kghostview.h"

Display *kde_display;

void Syntax(char *call)
{
    printf(i18n("Usage: %s\n"), call);
    printf(i18n("[filename]\n"));
    printf(i18n("See Qt documentation for command line options\n"));
    exit(1);
}

class MyApp : public KApplication
{
public:

  	MyApp( int &argc, char **argv, const QString &rAppName );
 	virtual bool x11EventFilter( XEvent * );
	~MyApp();
};

MyApp::MyApp( int &argc, char **argv, const QString &rAppName )
			: KApplication( argc, argv, rAppName )
{
}

MyApp::~MyApp()
{
	//printf("MyApp::~MyApp()\n");
}

bool MyApp::x11EventFilter( XEvent *ev ) {
	
	for ( KGhostview *kg = KGhostview::windowList.first(); kg!=0;
		kg=KGhostview::windowList.next() )
	{ 
	
	if(ev->xany.type == ClientMessage) {
		kg->page->mwin = ev->xclient.data.l[0];
		fprintf(stderr, 
			"kghostview: Ghostscript communication window set as 0x%lx\n",
			kg->page->mwin);
		
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
	
	signal( SIGCHLD, SIG_DFL );
	
	struct stat sbuf;
	
	KGhostview::windowList.setAutoDelete( FALSE );
	
	if (argc > 2) Syntax(argv[0]);
	
	if ( a.isRestored() ) {
		int n = 1;
		while (KTopLevelWidget::canBeRestored(n)) {
	    	KGhostview *kg = new KGhostview ();
			CHECK_PTR( kg );
		}
    } else if (argc == 2) {
	
		KGhostview *kg = new KGhostview ();
		CHECK_PTR( kg );
	
		kg->filename = argv[1];
		if (strcmp(kg->filename, "-")) {
		        KURL u( kg->filename );
			if ( strcmp( u.protocol(), "file" ) != 0 || u.hasSubProtocol() )
			{
			    kg->openNetFile( kg->filename );
			}
			else
			{
			    if ((kg->psfile = fopen(argv[1], "r")) == 0) {
				QString s;
				s.sprintf( i18n("The file\n\"%s\"\ncould not be found."),
					   argv[1] );
 				KMsgBox::message(0, i18n("Error"), s, 2 );
			    }
			    stat(kg->filename, &sbuf);
			    kg->mtime = sbuf.st_mtime;
			}
		} else {
			printf( i18n("Didn't recognise file\n") );
		}
	} else {
		KGhostview *kg = new KGhostview ();
		CHECK_PTR( kg );
	}
	
	return a.exec();
}

