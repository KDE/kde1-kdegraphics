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

#include "viewport.h"
#include "viewport.moc"
#include "confighandler.h"

WViewPort::WViewPort(const char *file=0, QWidget *parent=0, 
		const char *name=0, WFlags f = 0)
	: QLabel(parent, name, f)
{
	oldContext=0;
	registerFormats();

	setAutoResize(true);
	
	QPopupMenu *ImageZoom = new QPopupMenu;
	ImageZoom->insertItem("+50%", this,
			      SLOT(doScalePlus()));
	ImageZoom->insertItem("+10%", this,
			      SLOT(doScalePlusSmall()));
	ImageZoom->insertItem("-10%", this,
			      SLOT(doScaleMinusSmall()));
	ImageZoom->insertItem("-50%", this,
			      SLOT(doScaleMinus()));

	QPopupMenu *ImageRotate = new QPopupMenu;
	ImageRotate->insertItem("Clockwise",this,
				SLOT(rotateClockwise()));
	ImageRotate->insertItem("Anti-clock", this, 
				SLOT(rotateAntiClockwise()));
	ImageRotate->insertItem("Mirror X", this, 
				SLOT(mirrorX()));
	ImageRotate->insertItem("Mirror Y", this, 
				SLOT(mirrorY()));
	
	QPopupMenu *ImageRoot = new QPopupMenu;
	ImageRoot->insertItem("Ti&le", this, SLOT(tileToDesktop()));
	ImageRoot->insertItem("Max Size", this, SLOT(maxToDesktop()));
	ImageRoot->insertItem("Maxpect", this, SLOT(maxpectToDesktop()));
	
	lb_popup = new QPopupMenu;
	lb_popup->insertItem("Zoom", ImageZoom);
	lb_popup->insertItem("Rotate",ImageRotate);
	lb_popup->insertItem("To Desktop",ImageRoot);
	lb_popup->insertItem("Fit window size to pixmap size", this,
			     SLOT(fitWindowToPixmap()));
	lb_popup->insertItem("Fit pixmap size to window size", this,
			     SLOT(fitPixmapToWindow()));

	QPixmapCache::setCacheLimit( KVConfigHandler::cacheSize );

	image = new QPixmap();
	imagefile="";
	
	load(file);
}


bool WViewPort::load(const char *filename)
{
	bool ret=0;
	QString save=imagefile;
	QPixmap *copyimage;

	if(filename){
	
		// Save image file name	

		imagefile= filename;

		// Try internal types, fail if not found.
		QApplication::setOverrideCursor(waitCursor);

		hide();
		
		if(oldContext)
			QColor::destroyAllocContext( oldContext );

		oldContext = QColor::enterAllocContext();
		
		copyimage = 0L;

		//query if the image "filename" is in cache 
		copyimage = QPixmapCache::find(filename);
		
		if (copyimage==0L)
		  { //image not cached 
		    
		    ret=image->load(filename);
		    setPixmap(*image);
		    if (ret==TRUE)
		      {
			// picture load was succesful.
			// let's put it in cache.

			// make a duplicate of image, because 
			// QPixmapCache doesn't allocate memory,
			// and put the duplicate in the cache.
			copyimage = new QPixmap; 
			*copyimage = *image;    
			QPixmapCache::insert(filename,copyimage);
			// Rem. On cache removal, the allocated 
			// memory is freed by QPixmapCache
		      }
		  }
		else
		  { //image cached 

		    *image = *copyimage; //no pionter copy, copy contructor!
		    setPixmap(*image);
		    ret = TRUE;
		  }

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


/*
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
*/
void WViewPort::mousePressEvent(QMouseEvent *e)
{
  static QPoint tmp_point;
  if(e->button() == RightButton) 
    {
      tmp_point = QCursor::pos();
      if(lb_popup)
	lb_popup->popup(tmp_point);
    }
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


void WViewPort::doScalePlus()
{
  QWMatrix S(1.5F, 0.0F, 0.0F, 1.5F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::doScalePlusSmall()
{
  QWMatrix S(1.1F, 0.0F, 0.0F, 1.1F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::doScaleMinusSmall()
{
  QWMatrix S(0.9090909090909F, 0.0F, 0.0F, 0.9090909090909F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::doScaleMinus()
{
  QWMatrix S(0.66666666666F, 0.0F, 0.0F, 0.66666666666F, 0.0F, 0.0F);
  turnPixmap(S);
}


void WViewPort::mirrorX()
{
  QWMatrix S(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::mirrorY()
{
  QWMatrix S(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::rotateClockwise()
{
  QWMatrix S(0.0F, 1.0F, -1.0F, 0.0F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::rotateAntiClockwise()
{
  QWMatrix S(0.0F, -1.0F, 1.0F, 0.0F, 0.0F, 0.0F);
  turnPixmap(S);
}

void WViewPort::turnPixmap( QWMatrix S)
{
  matrix = matrix * S;

  QApplication::setOverrideCursor(waitCursor);
  setPixmap(image->xForm(matrix));
  QApplication::restoreOverrideCursor();
  fitToPixmap();

  emit doResize();
}

void WViewPort::fitToPixmap()
{
	resize( pixmap()->width(), pixmap()->height() );
	emit resized();
}


void WViewPort::tileToDesktop()
{
  qApp->desktop()->setBackgroundPixmap( *this->pixmap());
}

void WViewPort::maxToDesktop()
{ 
  QWMatrix mat;
  QPixmap image(*this->pixmap());
  QWidget *deskWidget = qApp->desktop();
  float swid, shei;
  
  shei= (float)deskWidget->height()/(float)image.height();
  swid= (float)deskWidget->width()/(float)image.width();
  
  mat.scale(swid, shei);
  
  deskWidget->setBackgroundPixmap(image.xForm(mat));
  
  repaint();
  
}

void WViewPort::maxpectToDesktop()
{
  
  float sc;
  QWidget *deskWidget = qApp->desktop();
  QPixmap *currPix = pixmap();

  float S = (float)deskWidget->height()/(float)deskWidget->width(),
    I=(float)currPix->height()/(float)currPix->width();

  if (S < I)
    sc= (float)deskWidget->height()/(float)currPix->height();
  else
    sc= (float)deskWidget->width()/(float)currPix->width();
  
  QWMatrix mat;
  QPixmap image(*this->pixmap());
  
  mat.scale(sc,sc);
  
  qApp->desktop()->setBackgroundPixmap(image.xForm(mat));
  
  repaint();
}

void WViewPort::fitWindowToPixmap()
{
  emit doResize();
}

void WViewPort::fitPixmapToWindow()
{
  float sx, sy;
  sx = (float) parwidth / (float) (width());
  sy = (float) parheight / (float) (height());
  QWMatrix S(sx, 0.0F, 0.0F, sy, 0.0F, 0.0F);
  turnPixmap(S);
}
