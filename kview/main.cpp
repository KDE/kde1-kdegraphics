// kView -- Qt-toolkit based Graphic viewer for the KDE project.
//          
// Sirtaj Kang, Oct '96.
//
// $Id$

//#include <qslider.h>
#include "fileman.h"
#include "wview.h"
#include "confighandler.h"

#include <qimage.h>
#include <stdio.h>

//QStrList fileList;

KApplication *theApp;


int main(int argc, char **argv)
{
  int i=0;

  theApp = new KApplication (argc, argv, "kview");
  
  //KVConfigHandler::setConfigEntry("General","Cache",3);
  //i=(int)KVConfigHandler::getConfigEntry("General","Cache");
  
  //printf("%d\n",i);

  KVConfigHandler::readConfigEntries();
  
  if ( qApp->argc() > 1 )
    {
      i=1;
      while ( i < qApp->argc() )
	{
	  QString f = qApp->argv()[i];
	  
	  if (i==1)
	    {
	      /*    if ( f.find( ":/" ) == -1 && f.left(1) != "/" )
		{
		  char buffer[ 1024 ];
		  getcwd( buffer, 1023 );
		  f.sprintf( "%s/%s", buffer, argv[i] );
		}*/
	    }

	  // VERY UGLY HACK,  I don't have time to make it better
          if ( f != "Image Viewer" && f != "-caption" && f != "%u" && f != "%c" )
	  	Fileman::appendFileList(QString(qApp->argv()[i]));
	  i++;
	}
    }
  
  Fileman *man = new Fileman(0L,0L);
  man->show();
  man->firstClicked();
  int retval = theApp->exec();
  // KVConfigHandler::writeConfigEntries();
  return retval;
}
