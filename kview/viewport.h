// ////////////////////
// viewport.h -- This class encapsulates the viewer widget.
//
// Sirtaj Kang, 1996.

// $Id$

#ifndef _SSK_WVIEWPORT_H
#define _SSK_WVIEWPORT_H

#include <qlabel.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qpopmenu.h>
#include <qpmcache.h>


/// 
/**	Widget containing the current image.

	@author  Sirtaj S. Kang <taj@kde.org>
	@version $Id$

*/
class WViewPort : public QLabel
{
	Q_OBJECT;
private:
	QString imagefile;
	QPixmap *image;
	QWMatrix matrix;
	int	oldContext;
	QPopupMenu *lb_popup;
	void fitToPixmap();

public:
	///
	WViewPort(const char *imagefile=0, QWidget *parent=0, 
		const char *name=0, WFlags f = 0);

	/// Load image from local file
	bool load(const char *filename);

	/// The image pixmap
	QPixmap *realPixmap(){ return image; };
	int parwidth,parheight;

public slots:	
	///
	void scale(float x,float y);
	///
	void turnPixmap( QWMatrix S);
	void doScalePlus();
	void doScaleMinus();
        void doScalePlusSmall();
        void doScaleMinusSmall();
	void rotateClockwise();
	void rotateAntiClockwise();
	void mirrorX();
	void mirrorY();
	void tileToDesktop();
	void maxToDesktop();
	void maxpectToDesktop();
	void fitWindowToPixmap();
	void fitPixmapToWindow();
	/**@name signals
	*/
	//@{

signals:
	
	///
	void clicked();
	///
	void resized();
	//@}
	void showDispManager();
	void doResize();

protected:

	///
	void registerFormats();
	///
	void mousePressEvent(QMouseEvent *);
	
};

#endif
