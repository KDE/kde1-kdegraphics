#include <stdlib.h>
#include <stdio.h>

#include "print.h"
#include "print.moc"

#include <qaccel.h>

PrintDialog::PrintDialog( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setGeometry( 5, 5, 250, 100 );
	tmpQFrame->setFrameStyle( 35 );
	tmpQFrame->setLineWidth( 2 );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 25, 25, 80, 25 );
	tmpQLabel->setText( "Printer name" );
	tmpQLabel->setAlignment( 290 );

	
	nameLineEdit = new QLineEdit( this );
	nameLineEdit->setGeometry( 115, 25, 100, 25 );
	nameLineEdit->setText( "lp" );

	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 30, 60, 75, 25 );
	tmpQLabel->setText( "Pages" );
	tmpQLabel->setAlignment( 290 );

	QPushButton* ok;
	ok = new QPushButton( this );
	ok->setGeometry( 125, 110, 65, 30 );
	ok->setText( "OK" );
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );

	QPushButton* cancel;
	cancel = new QPushButton( this );
	cancel->setGeometry( 195, 110, 60, 30 );
	cancel->setText( "Cancel" );
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );


	
	modeComboBox = new QComboBox( this );
	modeComboBox->setGeometry( 115, 60, 100, 25 );
	modeComboBox->insertItem( "All" );
	modeComboBox->insertItem( "Even" );
	modeComboBox->insertItem( "Odd" );
	modeComboBox->insertItem( "Marked" );

	resize( 260, 145 );	
	
}




