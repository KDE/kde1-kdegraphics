// -*- c++ -*-

#ifndef APP_H
#define APP_H

#include <kapp.h>
#include <qstring.h>
#include <kfm.h>
#include "kpaint.h"

class MyApp : public KApplication {
  Q_OBJECT

public:
   MyApp( int &argc, char **argv, const QString );
  static void usage();
  //   KPaint *kp;
   KFM *kfm;
};

#endif // APP_H


