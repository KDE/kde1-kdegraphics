/****************************************************************************
**
** A dialog for the display of information about a PostScript document.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "info.h"
#include "info.moc"

#include <qaccel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

InfoDialog::InfoDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	//QFrame* tmpQFrame;
	//tmpQFrame = new QFrame( this );
	//tmpQFrame->setGeometry( 5, 5, 250, 100 );
	//tmpQFrame->setFrameStyle( 35 );
	//tmpQFrame->setLineWidth( 2 );
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 5 );
	
	topLayout->addStretch( 10 );
	
	QGridLayout *grid = new QGridLayout( 5, 3, 5 );
	topLayout->addLayout( grid );
	
	grid->addRowSpacing(0,10);
	//grid->setRowStretch(1,10);
	//grid->addRowSpacing(2,2);
	//grid->setRowStretch(3,10);
	grid->addRowSpacing(4,10);
	//grid->addRowSpacing(6,10);
	
	grid->setColStretch(2,10);
	
	QLabel *tmpQLabel;
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 10, 10, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("File :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
	
	grid->addWidget( tmpQLabel, 1, 0 );
	
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 10, 40, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Title :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
	
	grid->addWidget( tmpQLabel, 2, 0 );
	
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 10, 70, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Date :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
		
	grid->addWidget( tmpQLabel, 3, 0 );
	
	fileLabel = new QLabel( this );
	//fileLabel->setGeometry( 55, 10, 170, 30 );
	fileLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( fileLabel, 1, 1 );
	
	titleLabel = new QLabel( this );
	//titleLabel->setGeometry( 55, 40, 170, 30 );
	titleLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( titleLabel, 2, 1 );
	
	dateLabel = new QLabel( this );
	//dateLabel->setGeometry( 55, 70, 170, 30 );
	dateLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( dateLabel, 3, 1 );
	
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

	//ok = new QPushButton( this );
	//ok->setGeometry( 190, 115, 60, 30 );
	//ok->setText( i18n("OK") );
	
	//ok->setFocus();
	
	ok = bbox->addButton( i18n("OK") );
	ok->setDefault( TRUE );
	connect( ok, SIGNAL(clicked()), SLOT(reject()) );
	
	bbox->layout();
	topLayout->addWidget( bbox );
	
	//resize( 260, 150 );
	//setMaximumSize( 260, 150 );
	//setMinimumSize( 260, 150 );
	
	topLayout->activate();
	
	resize( 250, 
		30 + 3*tmpQLabel->sizeHint().height() + bbox->sizeHint().height() );
}




