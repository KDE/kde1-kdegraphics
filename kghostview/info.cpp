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

#include <klocale.h>
#include <kapp.h>
#define i18n(X) klocale->translate(X)

InfoDialog::InfoDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setGeometry( 5, 5, 250, 100 );
	tmpQFrame->setFrameStyle( 35 );
	tmpQFrame->setLineWidth( 2 );
	
	QLabel *tmpQLabel;
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 10, 10, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("File :"));
	
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 10, 40, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Title :"));
	
	tmpQLabel = new QLabel( this );
	tmpQLabel->setGeometry( 10, 70, 40, 30 );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Date :"));
	
	fileLabel = new QLabel( this );
	fileLabel->setGeometry( 55, 10, 170, 30 );
	fileLabel->setAlignment( AlignLeft|AlignVCenter );
	
	titleLabel = new QLabel( this );
	titleLabel->setGeometry( 55, 40, 170, 30 );
	titleLabel->setAlignment( AlignLeft|AlignVCenter );
	
	dateLabel = new QLabel( this );
	dateLabel->setGeometry( 55, 70, 170, 30 );
	dateLabel->setAlignment( AlignLeft|AlignVCenter );
	
	ok = new QPushButton( this );
	ok->setGeometry( 190, 115, 60, 30 );
	ok->setText( i18n("OK") );
	ok->setAutoDefault(TRUE);
	ok->setFocus();
	connect( ok, SIGNAL(clicked()), SLOT(reject()) );
	
	resize( 260, 150 );
	setMaximumSize( 260, 150 );
	setMinimumSize( 260, 150 );
	
}




