// $Id$

#include <kdebug.h>
#include <qpushbt.h>
#include <klocale.h>
#include <kapp.h>
#include "depthdialog.h"

depthDialog::depthDialog(Canvas *c, QWidget* parent, const char* name)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QPushButton *cancelButton;
  QLabel *tmpLabel;

  tmpLabel = new QLabel( this, "Label_7" );
  tmpLabel->setGeometry( 10, 10, 80, 30 );
  tmpLabel->setText( klocale->translate("Image Depth:") );

  depthBox = new QComboBox( FALSE, this, "ComboBox_1" );
  depthBox->setGeometry( 100, 10, 130, 30 );
  depthBox->setSizeLimit( 10 );
  depthBox->setAutoResize( FALSE );
  depthBox->insertItem(klocale->translate("1 (2 Colours)"), ID_COLOR_1);
  depthBox->insertItem(klocale->translate("4 (16 Colours)"), ID_COLOR_4);
  depthBox->insertItem(klocale->translate("8 (256 Colours)"), ID_COLOR_8);
  depthBox->insertItem(klocale->translate("15 (32k Colours)"), ID_COLOR_15);
  depthBox->insertItem(klocale->translate("16 (64k Colours)"), ID_COLOR_16);
  depthBox->insertItem(klocale->translate("24 (True Colour)"), ID_COLOR_24);
  depthBox->insertItem(klocale->translate("32 (True Colour)"), ID_COLOR_32);

  //  depthBox->setCurrentItem(c->pixmap()->depth());
  
  switch (c->pixmap()->depth()) {
  default: // fall to 1bit colordepth
  case 1:
    depthBox->setCurrentItem(ID_COLOR_1);
    break;
  case 4:
    depthBox->setCurrentItem(ID_COLOR_4);
    break;
  case 8:
    depthBox->setCurrentItem(ID_COLOR_8);
    break;
  case 15:
    depthBox->setCurrentItem(ID_COLOR_15);
    break;
  case 16:
    depthBox->setCurrentItem(ID_COLOR_16);
    break;
  case 24:
    depthBox->setCurrentItem(ID_COLOR_24);
    break;
  case 32:
    depthBox->setCurrentItem(ID_COLOR_32);
    break;
  }

  /*
  if (c->pixmap()->depth() == 1)
    depthBox->setCurrentItem(1);
  else if (c->pixmap()->depth() == 8)
    depthBox->setCurrentItem(8);
  else if (c->pixmap()->depth() == 24)
    depthBox->setCurrentItem(24);
  */

  okButton= new QPushButton(klocale->translate("OK"), this);
  cancelButton= new QPushButton(klocale->translate("Cancel"), this);

  okButton->move(20, 50);
  cancelButton->move(140, 50);

  resize( 240, 80 );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

#include "depthdialog.moc"
