////////////////
//
// viewport.cpp -- methods for ViewPort widget.
//
// Sirtaj Kang, Oct 1996.

// $Id$

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<qapp.h>
#include<qmsgbox.h>
#include<qcolor.h>
#include <qpixmap.h>

#include"viewport.moc"

WViewPort::WViewPort(const char *file=0, QWidget *parent=0, 
		const char *name=0, WFlags f = 0)
	: QLabel(parent, name, f)
{
	oldContext=0;
	registerFormats();

	setAutoResize(true);
	
	image = new QPixmap();
	imagefile="";

	load(file);
}

bool WViewPort::load(const char *filename)
{
	bool ret=0;
	QString save=imagefile;

	if(filename){
	
		// Save image file name	

		imagefile= filename;

		// Try internal types, fail if not found.
		QApplication::setOverrideCursor(waitCursor);

		hide();
		
		if(oldContext)
			QColor::destroyAllocContext( oldContext );

		oldContext = QColor::enterAllocContext();

		ret=image->load(filename);
		setPixmap(*image);

		QColor::leaveAllocContext();
		show();
		QApplication::restoreOverrideCursor();


		matrix.reset();

		fitToPixmap();	
	} else
		ret = 1;

	if(!ret)
		imagefile = save;

	return ret;
}

void WViewPort::mousePressEvent(QMouseEvent *)
{
	emit clicked();
}

void WViewPort::scale(float x, float y)
{
	QApplication::setOverrideCursor(waitCursor);

	matrix.scale(x,y);
	setPixmap(image->xForm(matrix));

	QApplication::restoreOverrideCursor();
	
	fitToPixmap();
}


void WViewPort::rotate( float angle )
{
	QApplication::setOverrideCursor(waitCursor);

	matrix.rotate(angle);
	setPixmap(image->xForm(matrix));

	QApplication::restoreOverrideCursor();

	fitToPixmap();
}

void WViewPort::fitToPixmap()
{
	resize( pixmap()->width(), pixmap()->height() );
	emit resized();
}
