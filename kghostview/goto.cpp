/****************************************************************************
**
** A dialog for choosing which page of a document to view.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "goto.h"
#include "goto.moc"

#include <qaccel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

GoTo::GoTo( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
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
	
	debug("added grid");

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 15, 25, 55, 25 );
	tmpQLabel->setText( i18n("Section") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );
	
	grid->addWidget( tmpQLabel, 1, 0 );

	partLine = new QLineEdit( this );
	//partLine->setGeometry( 80, 25, 70, 25 );
	partLine->setFocus();
	partLine->setFixedHeight( partLine->sizeHint().height() );
	connect(partLine, SIGNAL(returnPressed()), this, SLOT(partChange()) );
	
	grid->addWidget( partLine, 1, 1 );

	partLabel = new QLabel( this );
	//partLabel->setGeometry( 155, 25, 80, 25 );
	
	grid->addWidget( partLabel, 1, 2 );
	
	tmpQLabel = new QLabel( this );
	//tmpQLabel->setGeometry( 15, 60, 55, 25 );
	tmpQLabel->setText( i18n("Page") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 3, 0 );

	pageLine = new QLineEdit( this );
	//pageLine->setGeometry( 80, 60, 70, 25 );
	pageLine->setFixedHeight( partLine->sizeHint().height() );
	connect(pageLine, SIGNAL(returnPressed()), this, SLOT(pageChange()) );
	
	grid->addWidget( pageLine, 3, 1 );

	pageLabel = new QLabel( this );
	//pageLabel->setGeometry( 155, 60, 80, 25 );
	
	grid->addWidget( pageLabel, 3, 2 );
	
	debug("filled in grid");
	
	
	
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
	//ok->setGeometry( 115, 115, 65, 30 );
	//ok->setText( i18n("OK") );
	//ok->setAutoDefault(TRUE);
	
	ok = bbox->addButton( i18n("OK") );
	connect( ok, SIGNAL(clicked()), SLOT(setCurrentPage()) );

	//cancel = new QPushButton( this );
	//cancel->setGeometry( 190, 115, 60, 30 );
	//cancel->setText( i18n("Cancel") );
	//cancel->setAutoDefault(TRUE);
	
	cancel = bbox->addButton( i18n("Cancel") );
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
	
	debug("added buttons");
	
	bbox->layout();
	topLayout->addWidget( bbox );

	//resize( 260, 150 );
	//setMaximumSize( 260, 150 );
	//setMinimumSize( 260, 150 );
	
	topLayout->activate();
	
	resize( 2*bbox->sizeHint().width(),
		30 + 2*partLine->sizeHint().height() + bbox->sizeHint().height() );
}

void GoTo::init()
{
	QString temp;
	
	cumulative_pages=0;
	for(i=0;i<10;i++) {
		cumulative_pages+=pages_in_part[i];
		if (cumulative_pages>current_page) break;
	}
	cumulative_pages-=pages_in_part[i];
	part=i;
	page=current_page-cumulative_pages;
	pages=pages_in_part[i];
	
	temp.sprintf( "%d", part+1);
	partLine->setText( temp );
	
	temp.sprintf( i18n("of %d"), num_parts+1);
	partLabel->setText( temp );
	partLabel->setMinimumSize( partLabel->sizeHint() );
	
	temp.sprintf( "%d", page+1);
	pageLine->setText( temp );
	
	temp.sprintf( i18n("of %d"), pages);
	pageLabel->setText( temp );
	pageLabel->setMinimumSize( pageLabel->sizeHint() );	
}

void GoTo::partChange()
{
	int new_part;
	QString temp = partLine->text();
	new_part=atoi(temp.data());
	
	if(!(new_part<1 || new_part>num_parts+1) && (new_part-1) !=part) {
		part=new_part-1;
		page=0;
		pages=pages_in_part[part];
		temp.sprintf( "%d", page+1);
		pageLine->setText( temp );
		temp.sprintf( i18n("of %d"), pages);
		pageLabel->setText( temp );
	} else {
		temp.sprintf( "%d", part+1);
		partLine->setText( temp );
	}

}

void GoTo::pageChange()
{
	int new_page;
	QString temp = pageLine->text();
	new_page=atoi(temp.data());
	
	if(new_page<1 || new_page>pages_in_part[part]) {
		temp.sprintf( "%d", page+1);
		pageLine->setText( temp );
	} else 
		page=new_page;	
}

void GoTo::setCurrentPage()
{
	pageChange();
	partChange();
	
	cumulative_pages=0;
	for(i=0;i<part;i++) {
		cumulative_pages+=pages_in_part[i];
	}
	current_page=page-1+cumulative_pages;
	
	accept();
}


