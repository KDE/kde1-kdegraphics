
//
// Main widget for kview application.
//
//@author Sirtaj Kang

// $Id$

#ifndef _SSK_WVIEW_H
#define _SSK_WVIEW_H

#include <kfm.h>
#include <kapp.h>

#include<stdio.h>

#include<qmsgbox.h>
#include<qwidget.h>
#include<qmenubar.h>
#include<qlist.h>
#include<qevent.h>  //Martin

#include"kerror.h"

class WViewPort;
class QwViewport;
class Fileman;

/// 
/** The main kview widget.

	@author  Sirtaj S. Kang (ssk@kde.org)
	@version $Id$
*/
class WView : public QWidget
{
	Q_OBJECT;

public:

	/// Possible IO types that kview is waiting for
	enum ioAction {GET, PUT};

private:

// Menus
	QMenuBar *Menu;
	QPopupMenu *File, *Image, *Help;
	QPopupMenu *ImageZoom, *ImageRotate, *ImageRoot;
	QAccel *myAccel;

// Image 
	WViewPort *Viewport;
	QwViewport *Scroller;

	QString imagePath;
	QString imageFormat;

	QString lastPath;

// Net IO
	KFM *kfm;
	ioAction kfmAction;
	QString netFile, tmpFile;

// Does the window contain an image?
	bool loaded,loadSuccess;

// returns the scale value to fit the current image to the desktop
// without losing aspect ratio.

	float screenScale();

// List of kview windows

  //static QList<WView> windowList;
  //static int winCount

// Error Handler
        static KViewError errHandler;
  
        QString currentFilename;
        int     imageWidth,imageHeight;
        Fileman *man;

        void sendCloseSignal();
public:


/// Loads an image from its URL, using a KFM connection.

	bool loadNetFile(const char *URL);

/// Loads a local image
	bool loadLocal(const char *filename=NULL);

///
	WView(QWidget *parent=0, const char *name=0, WFlags f = 0);

///
	~WView();

  
	char * getCurrentFilename() { return currentFilename.data(); }
	void   changeImageName(char * text)
	  {
	    currentFilename = text;
	    currentFilename.detach();
	    setCaption(QString("kview: ")+ currentFilename);
	  }
	void  redrawCaption();
	int    getImageWidth() { return imageWidth;}
	int    getImageHeight() { return imageHeight; }
        QString getImageType()  { return imageFormat; } 

  /**@name slots
*/
//@{

public slots:
/**@name image file manipulation
*/
//@{

/// 
/** Generic image loader.
Loads image with loadLocal if locator is a local file,
loads it with loadNetFile if it's a URL.

Check <i>loadSuccess</i> to check image load status.
*/
   bool load(const char *locator);

/// Loads the image the user selects from a File dialog.
	void loadImage();
/// Loads the URL the user types into a dialog.
	void loadURL();

/// returns true if the last attempt at load succeeded.
	bool loadSucceeded(){return ((kfm != 0)? TRUE : loadSuccess);};
//@}

/**@name Help routines
*/
//@{

///
	void launchHelp();

//@}


/**@name Window Operations
*/
//@{
///
	void toggleMenu();
///
	void closeWindow();
//@}


/**@name Current image operations
*/
//@{

///
	void sizeWindow();
///
	void zoomIn();
///
	void zoomOut();
///
	void rotateClockwise();
///
	void rotateAntiClockwise();

///
	void maxToDesktop();
///
	void maxpectToDesktop();
///
	void tileToDesktop();
///
	void imageInfo();

//@}


/// Drag and Drop - called when drop event occurs
	void slotDropEvent( KDNDDropZone * _dropZone );

/// internal - called when KFM has finished fetching the image.
	void slotKFMFinished();

//@}
	
/**@name signals */
//@{

signals:

/// Emitted when an error has occurred.
	
	void kviewError(KViewError::Type);

        void doUpdate(int);

        void closeClicked(int);

	void windowClicked(int);
//@}


protected:

	///
	//void mousePressEvent(QMouseEvent *);
	///
	void resizeEvent(QResizeEvent *);
  ///
        void closeEvent( QCloseEvent *);
};

#endif
