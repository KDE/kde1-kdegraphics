#include <stdio.h>
#include <kapp.h>
#include <kkeyconf.h>
#include "kpaint.h"
#include "app.h"

MyApp::MyApp( int &argc, char **argv, const QString appname)
  : KApplication( argc, argv, appname)
{
#ifdef KPDEBUG
  switch (getConfigState()) {
  case APPCONFIG_READONLY:
    fprintf(stderr, "Opened config: RO\n");
    break;
  case APPCONFIG_READWRITE:
    fprintf(stderr, "Opened config: RW\n");
    break;
  case APPCONFIG_NONE:
    fprintf(stderr, "Opened config: NONE\n");
    break;
  default:
    fprintf(stderr, "Opened config: ??\n");
    break;
  }
#endif

  QObject::connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));

  kp= new KPaint();


  /* KKeyCode initialization */
  kKeys->addKey("Quit", "CTRL+Q");
  kKeys->addKey("Close Window", "CTRL+C");
  kKeys->addKey("New Window", "CTRL+C");
  kKeys->addKey("Open File", "CTRL+O");
  kKeys->addKey("Save File", "CTRL+S");
  kKeys->addKey("Save File As", "CTRL+A");
  kKeys->addKey("Help", "F1");
  kKeys->addKey("Pen", "P");
  kKeys->addKey("Line", "L");
  kKeys->addKey("Rectangle", "R");
  kKeys->addKey("Ellipse", "E");
  kKeys->addKey("Circle", "C");
  kKeys->addKey("Spray Can", "S");

  /* connections for kpaint */
  kKeys->registerWidget("kpaint", kp);
  kKeys->connectFunction("kpaint", "Quit", kp, SLOT(fileQuit()));
  kKeys->connectFunction("kpaint", "New Window", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Close Window", kp, SLOT(closeWindow()));
  kKeys->connectFunction("kpaint", "Open File", kp, SLOT(fileOpen()));
  kKeys->connectFunction("kpaint", "Save File", kp, SLOT(fileSave()));
  kKeys->connectFunction("kpaint", "Save File As", kp, SLOT(fileSaveAs()));
  kKeys->connectFunction("kpaint", "Help", kp, SLOT(helpContents()));
  /*  kKeys->connectFunction("kpaint", "Pen", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Line", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Rectangle", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Ellipse", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Circle", kp, SLOT(newWindow()));
  kKeys->connectFunction("kpaint", "Spray Can", kp, SLOT(newWindow()));
  */

  kp->show();
}

#include "app.moc"
