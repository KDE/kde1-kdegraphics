/****************************************************************************
**
** A dialog for the display of error messages from Ghostscript.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "messages.h"
#include "messages.moc"

#include <qaccel.h>

#include <klocale.h>
#include <kapp.h>

MessagesDialog::MessagesDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	setCaption(i18n("Ghostscript messages"));

	QFrame *frame = new QFrame ( this );
	frame->setGeometry( 0, 15, 505, 285 );
	frame ->setFrameStyle( QFrame::Panel | QFrame::Sunken);
   	frame ->setLineWidth(2);
	
	messageBox = new QMultiLineEdit( frame );
	messageBox->setGeometry( 2, 2, frame->width()-4,
	frame->height()-4 );
	messageBox->setFrameStyle( QFrame::NoFrame );
	messageBox->setFont( QFont("courier", 12) );
	messageBox->setReadOnly( TRUE );


	QPushButton* clear;
	clear = new QPushButton( this );
	clear->setGeometry( 365, 315, 65, 30 );
	clear->setText( i18n("Clear") );
	clear->setAutoDefault(TRUE);
	connect( clear, SIGNAL(clicked()), SLOT(clear()) );

	cancel = new QPushButton( this );
	cancel->setGeometry( 440, 315, 60, 30 );
	cancel->setText( i18n("Cancel") );
	cancel->setAutoDefault(TRUE);
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	resize( 510, 350 );
	setMaximumSize( 510, 350 );
	setMinimumSize( 510, 350 );
	
}

void MessagesDialog::clear() {
	messageBox->setText( "" );
}



