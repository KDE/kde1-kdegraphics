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

ViewControl::ViewControl( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 10 );
		
	QGroupBox* vcGroupBox;
	vcGroupBox = new QGroupBox(  this );
	vcGroupBox->setFrameStyle( 49 );
	vcGroupBox->setTitle( i18n("Force changes to ...") );
	vcGroupBox->setAlignment( 1 );
	
	topLayout->addWidget( vcGroupBox, 10 );
	
	QGridLayout *grid = new QGridLayout( vcGroupBox, 5, 4, 5 );
	
	grid->setRowStretch(0,0);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);
	grid->setRowStretch(3,10);
	grid->setRowStretch(4,0);

	grid->setColStretch(0,0);
	grid->setColStretch(1,0);
	grid->setColStretch(2,10);
	grid->setColStretch(3,0);
	
	grid->addRowSpacing(0,10);

	magComboBox = new QComboBox( FALSE, vcGroupBox );
	magComboBox->setFixedHeight( magComboBox->sizeHint().height() );

	grid->addWidget( magComboBox, 1, 2 );
	
	mediaComboBox = new QComboBox( FALSE, vcGroupBox );
	mediaComboBox->setFixedHeight( magComboBox->sizeHint().height() );
	
	grid->addWidget( mediaComboBox, 2, 2 );
	
	orientComboBox = new QComboBox( FALSE, vcGroupBox );
	orientComboBox->insertItem(i18n("Portrait"));
	orientComboBox->insertItem(i18n("Landscape"));
	orientComboBox->insertItem(i18n("Seascape"));
	orientComboBox->insertItem(i18n("Upside down"));
	orientComboBox->setFixedHeight( magComboBox->sizeHint().height() );
					
	grid->addWidget( orientComboBox, 3, 2 );
		
	int labelWidth = 0;

	QLabel* vcLabel;
	vcLabel = new QLabel( magComboBox, i18n("&magnification"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );

	grid->addWidget( vcLabel, 1, 1 );

	vcLabel = new QLabel( mediaComboBox, i18n("m&edia"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 2, 1 );
	
	vcLabel = new QLabel( orientComboBox, i18n("&orientation"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 3, 1 );
	
	vcGroupBox->setMinimumHeight( 4*magComboBox->sizeHint().height()+20 );
	vcGroupBox->setMinimumWidth(
		40 + labelWidth + orientComboBox->sizeHint().width() );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	QPushButton *ok = bbox->addButton( i18n("&Apply") );
	connect( ok, SIGNAL(clicked()), SLOT(okClicked()) );

	QPushButton *apply = bbox->addButton( i18n("&Dismiss") );
	connect( apply, SIGNAL(clicked()), SLOT(reject()) );


	bbox->layout();
	topLayout->addWidget( bbox );

	topLayout->activate();
	
	resize( 250 ,0);
}

void ViewControl::okClicked()
{
	emit applyChanges();
}
