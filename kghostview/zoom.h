#ifndef ZOOM_H
#define ZOOM_H

#include <qdialog.h>

#include <kspinbox.h>

class Zoom : public QDialog {
      Q_OBJECT
      
public:
      Zoom( int *mag, QWidget *parent = 0, const char *name = 0 );
      KNumericSpinBox *sbMag;
      int *mag;
      
public slots:
      void acceptMag();
      
};

#endif

