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
  depthBox->insertItem(klocale->translate("1 (Bitmap)"));
  depthBox->insertItem(klocale->translate("8 (256 Colour)"));
  depthBox->insertItem(klocale->translate("24 (True Colour)"));

  if (c->pixmap()->depth() == 1)
    depthBox->setCurrentItem(0);
  else if (c->pixmap()->depth() == 8)
    depthBox->setCurrentItem(1);
  else if (c->pixmap()->depth() == 24)
    depthBox->setCurrentItem(3);

  okButton= new QPushButton(klocale->translate("OK"), this);
  cancelButton= new QPushButton(klocale->translate("Cancel"), this);

  okButton->move(20, 50);
  cancelButton->move(140, 50);

  resize( 240, 80 );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

#include "depthdialog.moc"
