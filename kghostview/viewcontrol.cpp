#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qpushbt.h>

#include "viewcontrol.h"
#include "viewcontrol.moc"

ViewControl::ViewControl( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QFrame* vcFrame;
	vcFrame = new QFrame( this );
	vcFrame->setGeometry( 5, 5, 250, 165 );
	vcFrame->setFrameStyle( 35 );
	vcFrame->setLineWidth( 2 );
	
	QGroupBox* vcGroupBox;
	vcGroupBox = new QGroupBox(  this );
	vcGroupBox->setGeometry( 15, 15, 230, 145 );
	vcGroupBox->setFrameStyle( 49 );
	vcGroupBox->setTitle( "Force changes" );
	vcGroupBox->setAlignment( 1 );

	
	orientComboBox = new QComboBox( this );
	orientComboBox->setGeometry( 120, 45, 110, 25 );
	orientComboBox->insertItem("Portrait");
	orientComboBox->insertItem("Landscape");
	orientComboBox->insertItem("Seascape");
	orientComboBox->insertItem("Upside down");
	orientComboBox->setFocus();
					
	
	magComboBox = new QComboBox( this );
	magComboBox->setGeometry( 120, 80, 110, 25 );

	mediaComboBox = new QComboBox( this );
	mediaComboBox->setGeometry( 120, 115, 110, 25 );

	QLabel* vcLabel;
	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 20, 75, 90, 30 );
	vcLabel->setText( "Magnification" );
	vcLabel->setAlignment( 290 );

	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 35, 40, 75, 30 );
	vcLabel->setText( "Orientation" );
	vcLabel->setAlignment( 290 );

	vcLabel = new QLabel( this );
	vcLabel->setGeometry( 35, 110, 75, 30 );
	vcLabel->setText( "Media" );
	vcLabel->setAlignment( 290 );

	QPushButton* ok;
	ok = new QPushButton( this );
	ok->setGeometry( 130, 180, 60, 30 );
	ok->setText( "Apply" );
	ok->setDefault(TRUE);
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );

	QPushButton* apply;
	apply = new QPushButton( this );
	apply->setGeometry( 195, 180, 60, 30 );
	apply->setText( "Cancel" );
	connect( apply, SIGNAL(clicked()), SLOT(reject()) );


	resize( 260, 215 );
}
