#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qpushbt.h>

#include "viewcontrol.h"
#include "viewcontrol.moc"

#include <klocale.h>
#include <kapp.h>
#define i18n(X) klocale->translate(X)

ViewControl::ViewControl( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QGroupBox* vcGroupBox;
	vcGroupBox = new QGroupBox(  this );
	vcGroupBox->setGeometry( 5, 10, 250, 150 );
	vcGroupBox->setFrameStyle( 49 );
	vcGroupBox->setTitle( i18n("Force changes") );
	vcGroupBox->setAlignment( 1 );

	
	orientComboBox = new QComboBox( this );
	orientComboBox->setGeometry( 120, 45, 110, 25 );
	orientComboBox->insertItem(i18n("Portrait"));
	orientComboBox->insertItem(i18n("Landscape"));
	orientComboBox->insertItem(i18n("Seascape"));
	orientComboBox->insertItem(i18n("Upside down"));
	orientComboBox->setFocus();
					
	
	magComboBox = new QComboBox( this );
	magComboBox->setGeometry( 120, 80, 110, 25 );

	mediaComboBox = new QComboBox( this );
	mediaComboBox->setGeometry( 120, 115, 110, 25 );

	QLabel* vcLabel;
	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 20, 75, 90, 30 );
	vcLabel->setText( i18n("Magnification") );
	vcLabel->setAlignment( 290 );

	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 35, 40, 75, 30 );
	vcLabel->setText( i18n("Orientation") );
	vcLabel->setAlignment( 290 );

	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 35, 110, 75, 30 );
	vcLabel->setText( i18n("Media") );
	vcLabel->setAlignment( 290 );

	QPushButton* ok;
	ok = new QPushButton( this );
	ok->setGeometry( 120, 170, 60, 30 );
	ok->setText( i18n("Apply") );
	ok->setAutoDefault(TRUE);
	connect( ok, SIGNAL(clicked()), SLOT(okClicked()) );

	QPushButton* apply;
	apply = new QPushButton( this );
	apply->setGeometry( 190, 170, 60, 30 );
	apply->setText( i18n("Cancel") );
	apply->setAutoDefault(TRUE);
	connect( apply, SIGNAL(clicked()), SLOT(reject()) );


	resize( 260, 205 );
	setMaximumSize( 260, 205 );
	setMinimumSize( 260, 205 );
}

void ViewControl::okClicked()
{
	emit applyChanges();
}
