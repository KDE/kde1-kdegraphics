// $Id$

#include <string.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qimage.h>
#include "infodialog.h"
#include "formats.h"

extern FormatManager *formatMngr;

imageInfoDialog::imageInfoDialog(Canvas *c, QWidget* parent= 0, const char* name= 0)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QPushButton *cancelButton;
  QLabel *tmpQLabel;

  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 30, 30, 100, 30 );
  tmpQLabel->setText( "Filename:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  QString name(c->filename());
  filename = new QLabel( this, "Label_2" );
  filename->setGeometry( 140, 30, 200, 30 );
  filename->setText( name );
  filename->setAlignment( 289 );
  filename->setMargin( -1 );

  tmpQLabel = new QLabel( this, "Label_3" );
  tmpQLabel->setGeometry( 30, 80, 100, 30 );
  tmpQLabel->setText( "Colour Depth:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  colourDepth = new QLabel( this, "Label_4" );
  colourDepth->setGeometry( 140, 80, 190, 30 );

  QString text;
  text.sprintf("%d", c->pixmap()->depth());
  colourDepth->setText( text );
  colourDepth->setAlignment( 289 );
  colourDepth->setMargin( -1 );

  tmpQLabel = new QLabel( this, "Label_5" );
  tmpQLabel->setGeometry( 30, 130, 100, 30 );
  tmpQLabel->setText( "Colours Used:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  coloursUsed = new QLabel( this, "Label_6" );
  coloursUsed->setGeometry( 140, 130, 190, 30 );
  text.sprintf("%d", c->pixmap()->convertToImage().numColors());
  coloursUsed->setText( text );
  coloursUsed->setAlignment( 289 );
  coloursUsed->setMargin( -1 );

  tmpQLabel = new QLabel( this, "Label_7" );
  tmpQLabel->setGeometry( 30, 180, 100, 30 );
  tmpQLabel->setText( "File Format:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  fileformat = new QComboBox( FALSE, this, "ComboBox_1" );
  fileformat->setGeometry( 140, 180, 130, 30 );
  fileformat->setSizeLimit( 10 );
  fileformat->setAutoResize( FALSE );
  fileformat->insertStrList(formatMngr->formats());
  //  fileformat->setCurrentItem(formatMngr->formats()->find("GIF"));

  tmpQLabel = new QLabel( this, "Label_8" );
  tmpQLabel->setGeometry( 30, 230, 120, 30 );
  tmpQLabel->setText( "Transparent Colour:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  QLineEdit* tmpQLineEdit;
  tmpQLineEdit = new QLineEdit( this, "LineEdit_1" );
  tmpQLineEdit->setGeometry( 170, 230, 100, 30 );
  tmpQLineEdit->setText( "" );
  tmpQLineEdit->setMaxLength( 32767 );
  tmpQLineEdit->setEchoMode( QLineEdit::Normal );
  tmpQLineEdit->setFrame( TRUE );

  tmpQLabel = new QLabel( this, "Label_9" );
  tmpQLabel->setGeometry( 30, 280, 100, 30 );
  tmpQLabel->setText( "Image Width:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  width = new QLabel( this, "Label_10" );
  width->setGeometry( 150, 280, 100, 30 );
  width->setText( "" );
  width->setAlignment( 289 );
  width->setMargin( -1 );

  tmpQLabel = new QLabel( this, "Label_11" );
  tmpQLabel->setGeometry( 30, 330, 100, 30 );
  tmpQLabel->setText( "Image Height:" );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  height = new QLabel( this, "Label_12" );
  height->setGeometry( 150, 330, 100, 30 );
  height->setText( "" );
  height->setAlignment( 289 );
  height->setMargin( -1 );

  okButton= new QPushButton("Ok", this);
  cancelButton= new QPushButton("Cancel", this);

  okButton->move(100, 380);
  cancelButton->move(220, 380);

  resize( 360, 410 );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );

}

#include "infodialog.moc"

