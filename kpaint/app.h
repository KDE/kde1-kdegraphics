#ifndef APP_H
#define APP_H

#include <kapp.h>
#include <qstring.h>
#include "kpaint.h"

class MyApp : public KApplication {
  Q_OBJECT

public:
   MyApp( int &argc, char **argv, const QString );
   KPaint *kp;
};

#endif // APP_H


