// kView -- Qt-toolkit based Graphic viewer for the KDE project.
//          
// Sirtaj Kang, Oct '96.
//
// $Id$

#include<kfm.h>
#include<kapp.h>

#include"wview.h"

int main(int argc, char **argv)
{
	KApplication theApp(argc, argv, "kview");
	WView *newWidget=0;
	bool OneSucceeded=FALSE;
	
	// open windows with referenced images
	if(qApp->argc()>1){

		for(int ctr=1; ctr < theApp.argc(); ctr++) {

			if( newWidget == 0)
				newWidget = new WView(0, 0, 0);
			
			newWidget->load((theApp.argv())[ctr]);

			if(newWidget->loadSucceeded()){
				newWidget->sizeWindow();
				newWidget->show();
				newWidget = 0;
				OneSucceeded = TRUE;
			}
		}

		if( WView::windowCount() != 0) {
			if(!OneSucceeded)
				newWidget->show();

			return theApp.exec();
		}
		else {
			return -1;	
		}

	// No command line images... open empty dialog.
	} 
	else {

		WView *appWidget = new WView(0,"kview", 0);

		appWidget->show();

		return theApp.exec();

	}
	return -2;
}
