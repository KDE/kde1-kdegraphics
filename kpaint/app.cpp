#include <kdebug.h>
#include <stdio.h>
#include <kapp.h>
#include <klocale.h>
#include "kpaint.h"
#include "version.h"
#include "app.h"

MyApp::MyApp( int &argc, char **argv, const QString appname)
  : KApplication( argc, argv, appname)
{
  KPaint *kp;

  clipboard_= 0;

  if (isRestored()) {
    for (int i= 1; KTopLevelWidget::canBeRestored(i); i++) {
      kp= new KPaint();
      kp->restore(i);
    }
  }
  else {
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
    kp->show();
  }
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
