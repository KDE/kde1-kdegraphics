/****************************************************************************
**
** A dialog for the selection of priting options.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "print.h"
#include "print.moc"

#include <qaccel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

#define i18n(X) klocale->translate(X)

PrintDialog::PrintDialog( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	//QFrame* tmpQFrame;
	//tmpQFrame = new QFrame( this );
	//tmpQFrame->setGeometry( 5, 5, 250, 100 );
	//tmpQFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
	//tmpQFrame->setLineWidth( 1 );

	int border = 5;
	int vertSpacing = 0;
	
	QBoxLayout *topLayout = new QVBoxLayout( this, border );
	
	topLayout->addStretch( 10 );
	
	QGridLayout *grid = new QGridLayout( 5, 3, 5 );
	topLayout->addLayout( grid );
	
	grid->addRowSpacing(0,10);
	//grid->setRowStretch(1,10);
	grid->addRowSpacing(2,2);
	//grid->setRowStretch(3,10);
	grid->addRowSpacing(4,10);

	grid->setColStretch(0,10);
	grid->setColStretch(1,20);
	grid->setColStretch(2,10);
	//grid->setColStretch(3,10);
	//grid->setColStretch(4,0);

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 25, 25, 80, 25 );
	tmpQLabel->setText( i18n("Printer name") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );

	grid->addWidget( tmpQLabel, 1, 0 );
	
	nameLineEdit = new QLineEdit( this );
	//nameLineEdit->setGeometry( 115, 25, 100, 25 );
	nameLineEdit->setText( "lp" );
	nameLineEdit->setFocus();
	nameLineEdit->setFixedHeight( nameLineEdit->sizeHint().height() );
	
	grid->addWidget( nameLineEdit, 1, 1 );

	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 30, 60, 75, 25 );
	tmpQLabel->setText( i18n("Pages") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 3, 0 );
	
	modeComboBox = new QComboBox( FALSE, this );
	//modeComboBox->setGeometry( 115, 60, 100, 25 );
	modeComboBox->insertItem( i18n("All") );
	//modeComboBox->insertItem( i18n("Even") );
	//modeComboBox->insertItem( i18n("Odd") );
	modeComboBox->insertItem( i18n("Marked") );
	modeComboBox->setFixedHeight( modeComboBox->sizeHint().height() );

	grid->addWidget( modeComboBox, 3, 1 );
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	//tmpQFrame->setGeometry( 5, 5, 250, 100 );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	//tmpQFrame->setLineWidth( 1 );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	topLayout->addWidget( tmpQFrame );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	QPushButton* ok;
	ok = bbox->addButton( i18n("OK") );
	
	
	//ok = new QPushButton( this );
	//ok->setGeometry( 115, 115, 65, 30 );
	//ok->setText( i18n("OK") );
	//ok->setAutoDefault(TRUE);
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );

	QPushButton* cancel;
	cancel = bbox->addButton( i18n("Cancel") );
	
	//cancel = new QPushButton( this );
	//cancel->setGeometry( 190, 115, 60, 30 );
	//cancel->setText( i18n("Cancel") );
	//cancel->setAutoDefault(TRUE);
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	bbox->layout();
	topLayout->addWidget( bbox );
	
	//resize( 260, 150 );
	//setMaximumSize( 260, 150 );
	//setMinimumSize( 260, 150 );
	
	topLayout->activate();
	
	resize( 250, 0 );
	
}




