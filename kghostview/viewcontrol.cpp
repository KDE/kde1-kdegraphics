/****************************************************************************
**
** A dialog for the selection of the view of a document.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qlayout.h>

#include "viewcontrol.h"
#include "viewcontrol.moc"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

#define i18n(X) klocale->translate(X)

ViewControl::ViewControl( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 5 );
	
	topLayout->addSpacing( 10 );
	
	QBoxLayout *paddingLayout = new QHBoxLayout();
	topLayout->addLayout( paddingLayout, 10 );
	
	QGroupBox* vcGroupBox;
	vcGroupBox = new QGroupBox(  this );
	//vcGroupBox->setGeometry( 5, 10, 250, 150 );
	vcGroupBox->setFrameStyle( 49 );
	vcGroupBox->setTitle( i18n("Force changes to ") );
	vcGroupBox->setAlignment( 1 );
	
	paddingLayout->addSpacing( 0 );
	paddingLayout->addWidget( vcGroupBox, 10 );
	paddingLayout->addSpacing( 0 );
	
	topLayout->addSpacing( 10 );
	
	QGridLayout *grid = new QGridLayout( vcGroupBox, 5, 4, 10 );
	
	grid->setRowStretch(0,10);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);
	grid->setRowStretch(3,10);
	grid->setRowStretch(4,10);

	grid->setColStretch(0,0);
	grid->setColStretch(1,10);
	grid->setColStretch(2,100);
	grid->setColStretch(3,0);

	
	int labelWidth = 0;

	QLabel* vcLabel;
	vcLabel = new QLabel( vcGroupBox );
	//vcLabel->setGeometry( 20, 75, 90, 30 );
	vcLabel->setText( i18n("Magnification") );
	vcLabel->setAlignment( 290 );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );

	grid->addWidget( vcLabel, 1, 1 );

	vcLabel = new QLabel( vcGroupBox );
	//vcLabel->setGeometry( 35, 110, 75, 30 );
	vcLabel->setText( i18n("Media") );
	vcLabel->setAlignment( 290 );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 2, 1 );
	
	vcLabel = new QLabel( vcGroupBox );
	//vcLabel->setGeometry( 35, 40, 75, 30 );
	vcLabel->setText( i18n("Orientation") );
	vcLabel->setAlignment( 290 );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 3, 1 );

	orientComboBox = new QComboBox( FALSE, vcGroupBox );
	//orientComboBox->setGeometry( 120, 45, 110, 25 );
	orientComboBox->insertItem(i18n("Portrait"));
	orientComboBox->insertItem(i18n("Landscape"));
	orientComboBox->insertItem(i18n("Seascape"));
	orientComboBox->insertItem(i18n("Upside down"));
	//orientComboBox->setFocus();
	orientComboBox->setFixedHeight( orientComboBox->sizeHint().height() );
					
	grid->addWidget( orientComboBox, 1, 2 );
	
	mediaComboBox = new QComboBox( FALSE, vcGroupBox );
	//mediaComboBox->setGeometry( 120, 115, 110, 25 );
	mediaComboBox->setFixedHeight( orientComboBox->sizeHint().height() );
	
	grid->addWidget( mediaComboBox, 2, 2 );
	
	magComboBox = new QComboBox( FALSE, vcGroupBox );
	//magComboBox->setGeometry( 120, 80, 110, 25 );
	magComboBox->setFixedHeight( orientComboBox->sizeHint().height() );

	grid->addWidget( magComboBox, 3, 2 );
	
	vcGroupBox->setMinimumHeight( 4*(mediaComboBox->sizeHint().height()+10) );
	vcGroupBox->setMinimumWidth(
		40 + labelWidth + orientComboBox->sizeHint().width() );
	
	topLayout->addStretch( 10 );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	QPushButton* ok;
	ok = bbox->addButton( i18n("OK") );
	//ok = new QPushButton( this );
	//ok->setGeometry( 120, 170, 60, 30 );
	//ok->setText( i18n("Apply") );
	//ok->setAutoDefault(TRUE);
	connect( ok, SIGNAL(clicked()), SLOT(okClicked()) );

	QPushButton* apply;
	apply = bbox->addButton( i18n("Cancel") );
	//apply = new QPushButton( this );
	//apply->setGeometry( 190, 170, 60, 30 );
	//apply->setText( i18n("Cancel") );
	//apply->setAutoDefault(TRUE);
	connect( apply, SIGNAL(clicked()), SLOT(reject()) );


	bbox->layout();
	topLayout->addWidget( bbox );

	//resize( 260, 205 );
	//setMaximumSize( 260, 205 );
	//setMinimumSize( 260, 205 );
	
	topLayout->activate();
	
	resize( 250 ,0);
}

void ViewControl::okClicked()
{
	emit applyChanges();
}
