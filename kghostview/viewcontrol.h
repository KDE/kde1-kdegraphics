#ifndef VIEWCONTROL_H
#define VIEWCONTROL_H

#include <qdialog.h>
#include <qcombo.h>


enum VCOrientation {VCPortrait=0, VCLandscape=1, VCUpsideDown=2, VCSeascape=3};

class ViewControl : public QDialog
{
  
  Q_OBJECT
public:

  ViewControl( QWidget *parent, const char *name );
  QComboBox* mediaComboBox;
  QComboBox* magComboBox;
  QComboBox* orientComboBox;

  void updateMedia (int media);
  void updateOrientation (int orientation);

public slots:
  void slotApplyClicked();
  
signals:
  void applyChanges();
};

#endif
