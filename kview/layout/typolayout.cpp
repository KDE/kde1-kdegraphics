/*
* typolayout.cpp -- Implementation of class KTypoLayout.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Tue May  5 01:12:37 EST 1998
*/

#include"baglayout.h"
#include"typolayout.h"

#include<assert.h>

#include<qpushbt.h>
#include<qbttngrp.h>
#include<qchkbox.h>
#include<qlabel.h>
#include<qlined.h>
#include<qmlined.h>
#include<qlistbox.h>
#include<qradiobt.h>

KTypoLayout::KTypoLayout( QWidget *parent )
	: KBagLayout( parent )
{
	assert( parent );

	_parent = parent;
	_group	= 0;
}

QLabel *KTypoLayout::newLabel( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QLabel *w = new QLabel( text, _parent );
	
	addWidget( w, x, y, xspan, yspan );

	return w;

}

QPushButton *KTypoLayout::newButton( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QPushButton *w = new QPushButton( text, _parent );

	if( _group != 0 ) {
		_group->insert( w );
	}

	addWidget( w, x, y, xspan, yspan );

	return w;
}

QRadioButton *KTypoLayout::newRadioButton( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QRadioButton *w = new QRadioButton ( text, _parent );

	if( _group != 0 ) {
		_group->insert( w );
	}

	addWidget( w, x, y, xspan, yspan );

	return w;
}

QLineEdit *KTypoLayout::newLineEdit( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QLineEdit * w = new QLineEdit( _parent );
	w->setText( text );

	KBagConstraints *c = addWidget( w, x, y, xspan, yspan );

	// line edits should only be resize horizontally by default

	c->setResizePolicy( KBagConstraints::Horizontal );

	return w;
}


QMultiLineEdit *KTypoLayout::newMultiLineEdit( const char *text, 
		int x, int y, int xspan, int yspan )
{
	QMultiLineEdit * w = new QMultiLineEdit( _parent );
	w->setText( text );

	addWidget( w, x, y, xspan, yspan );

	return w;
}

QCheckBox *KTypoLayout::newCheckBox( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QCheckBox *w = new QCheckBox ( text, _parent );

	addWidget( w, x, y, xspan, yspan );

	return w;
}

QListBox *KTypoLayout::newListBox( int x, int y, int xspan, int yspan)
{
	QListBox *w = new QListBox( _parent );

	addWidget( w, x, y, xspan, yspan );

	return w;
}

KTypoLayout *KTypoLayout::newSubLayout( int x, int y, int xspan, int yspan )
{
	QWidget *holder = new QWidget( _parent );

	addWidget( holder, x, y, xspan, yspan );

	KTypoLayout *sublayout = new KTypoLayout( holder );

	return sublayout;
}

void KTypoLayout::startGroup()
{
	if( _group ) {
		endGroup();
	}

	_group = new QButtonGroup( _parent );
	_group->setFrameStyle( QFrame::NoFrame );
}

void KTypoLayout::endGroup()
{
//	delete _group;
	_group = 0;
}
