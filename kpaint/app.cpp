#include <stdio.h>
#include <kapp.h>
#include <kkeyconf.h>
#include <klocale.h>
#include "kpaint.h"
#include "version.h"
#include "app.h"

MyApp::MyApp( int &argc, char **argv, const QString appname)
  : KApplication( argc, argv, appname)
{
  KPaint *kp;

  if (argc == 2) {
    kp= new KPaint((const char *) (argv[1]));
  }
  else if (argc == 1) {
    kp= new KPaint();
  }
  else {
    usage();
    ::exit(1);
  }

  /* KKeyCode initialization */
  kKeys->addKey(klocale->translate("Quit"), "CTRL+Q");
  kKeys->addKey(klocale->translate("Close Window"), "CTRL+C");
  kKeys->addKey(klocale->translate("New Window"), "CTRL+C");
  kKeys->addKey(klocale->translate("Open File"), "CTRL+O");
  kKeys->addKey(klocale->translate("Save File"), "CTRL+S");
  kKeys->addKey(klocale->translate("Save File As"), "CTRL+A");
  kKeys->addKey(klocale->translate("Help"), "F1");
  kKeys->addKey(klocale->translate("Pen"), "P");
  kKeys->addKey(klocale->translate("Line"), "L");
  kKeys->addKey(klocale->translate("Rectangle"), "R");
  kKeys->addKey(klocale->translate("Ellipse"), "E");
  kKeys->addKey(klocale->translate("Circle"), "C");
  kKeys->addKey(klocale->translate("Spray Can"), "S");

  /* connections for kpaint */
  kKeys->registerWidget("kpaint", kp);
  kKeys->connectFunction("kpaint", klocale->translate("Quit"), kp, SLOT(fileQuit()));
  kKeys->connectFunction("kpaint", klocale->translate("New Window"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Close Window"), kp, SLOT(closeWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Open File"), kp, SLOT(fileOpen()));
  kKeys->connectFunction("kpaint", klocale->translate("Save File"), kp, SLOT(fileSave()));
  kKeys->connectFunction("kpaint", klocale->translate("Save File As"), kp, SLOT(fileSaveAs()));
  kKeys->connectFunction("kpaint", klocale->translate("Help"), kp, SLOT(helpContents()));
  /*  kKeys->connectFunction("kpaint", klocale->translate("Pen"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Line"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Rectangle"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Ellipse"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Circle"), kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", klocale->translate("Spray Can"), kp, SLOT(newWindow()));
  */

   KConfig *config = KApplication::getKApplication()->getConfig();
   config->setGroup( "Test" );
   config->writeEntry( "TestString", QString(klocale->translate("This was a QString")) );

   
  kp->show();
}

void MyApp::usage()
{
  printf("kpaint " APPVERSTR " " APPAUTHOREMAIL);
  printf( klocale->translate("\n(c) Richard J. Moore 1997 Released under GPL see LICENSE for details\n"));
  printf(klocale->translate("Usage: "));
  QString aParameter( APPNAME );
  aParameter += klocale->translate(" [url | filename]\n");
  printf(aParameter.data());
}

#include "app.moc"
