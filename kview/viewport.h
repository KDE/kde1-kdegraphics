// ////////////////////
// viewport.h -- This class encapsulates the viewer widget.
//
// Sirtaj Kang, 1996.

// $Id$

#ifndef _SSK_WVIEWPORT_H
#define _SSK_WVIEWPORT_H

#include<qlabel.h>
#include<kpixmap.h>
#include<qwmatrix.h>

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
	
public:
	///
	WViewPort(const char *imagefile=0, QWidget *parent=0, 
		const char *name=0, WFlags f = 0);

	/// Load image from local file
	bool load(const char *filename);

	/// The image pixmap
	QPixmap *realPixmap(){ return image; };

	///
	void scale(float x,float y);
	///
	void rotate(float angle);

	/**@name signals
	*/
	//@{

signals:
	
	///
	void clicked();
	///
	void resized();
	//@}
	
protected:

	///
	void registerFormats();
	///
	void mousePressEvent(QMouseEvent *);

};

#endif
