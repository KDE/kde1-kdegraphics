// $Id$

#include <qpushbt.h>
#include <klocale.h>
#include <kapp.h>
#include "formatdialog.h"
#include "formats.h"

extern FormatManager *formatMngr;

formatDialog::formatDialog(const char *format, QWidget* parent= 0, const char* name= 0)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QPushButton *cancelButton;
  QLabel *tmpLabel;

  tmpLabel = new QLabel( this, "Label_7" );
  tmpLabel->setGeometry( 10, 10, 80, 30 );
  tmpLabel->setText( klocale->translate("File Format:") );

  fileformat = new QComboBox( FALSE, this, "ComboBox_1" );
  fileformat->setGeometry( 100, 10, 130, 30 );
  fileformat->setSizeLimit( 10 );
  fileformat->setAutoResize( FALSE );
  fileformat->insertStrList(formatMngr->formats());
  fileformat->setCurrentItem(formatMngr->formats()->find(format));

  okButton= new QPushButton(klocale->translate("Ok"), this);
  cancelButton= new QPushButton(klocale->translate("Cancel"), this);

  okButton->move(20, 50);
  cancelButton->move(140, 50);

  resize( 240, 80 );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

#include "formatdialog.moc"
