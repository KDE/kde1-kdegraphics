#include <stdlib.h>
#include <stdio.h>

#include "goto.h"
#include "goto.moc"

#include <qaccel.h>

GoTo::GoTo( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setGeometry( 5, 5, 250, 100 );
	tmpQFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
	tmpQFrame->setLineWidth( 1 );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 15, 25, 55, 25 );
	tmpQLabel->setText( "Section" );
	tmpQLabel->setAlignment( 290 );

	partLine = new QLineEdit( this );
	partLine->setGeometry( 80, 25, 70, 25 );
	partLine->setFocus();
	connect(partLine, SIGNAL(returnPressed()), this, SLOT(partChange()) );

	partLabel = new QLabel( this );
	partLabel->setGeometry( 155, 25, 80, 25 );
	
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 15, 60, 55, 25 );
	tmpQLabel->setText( "Page" );
	tmpQLabel->setAlignment( 290 );

	pageLine = new QLineEdit( this );
	pageLine->setGeometry( 80, 60, 70, 25 );
	connect(pageLine, SIGNAL(returnPressed()), this, SLOT(pageChange()) );

	pageLabel = new QLabel( this );
	pageLabel->setGeometry( 155, 60, 80, 25 );
	
	ok = new QPushButton( this );
	ok->setGeometry( 115, 115, 65, 30 );
	ok->setText( "OK" );
	ok->setAutoDefault(TRUE);
	connect( ok, SIGNAL(clicked()), SLOT(setCurrentPage()) );

	cancel = new QPushButton( this );
	cancel->setGeometry( 190, 115, 60, 30 );
	cancel->setText( "Cancel" );
	cancel->setAutoDefault(TRUE);
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	resize( 260, 150 );
	setMaximumSize( 260, 150 );
	setMinimumSize( 260, 150 );
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
	
	temp.sprintf( "of %d", num_parts+1);
	partLabel->setText( temp );
	
	temp.sprintf( "%d", page+1);
	pageLine->setText( temp );
	
	temp.sprintf( "of %d", pages);
	pageLabel->setText( temp );	
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
		temp.sprintf( "of %d", pages);
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


