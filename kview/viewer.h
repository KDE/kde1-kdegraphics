/*
* viewer.h -- Declaration of class KImageViewer.
* Generated by newclass on Wed Oct 15 11:37:16 EST 1997.
*/
#ifndef SSK_VIEWER_H
#define SSK_VIEWER_H

#include<ktopwidget.h>
#include<qwmatrix.h>

class KDNDDropZone;

class KFiltMenuFactory;
class KConfigGroup;
class KImageCanvas;
class KImageFilter;
class KLocale;
class KMenuBar;
class KStatusBar;
class QMenuData;
class QPopupMenu;
class QMouseEvent;
class QString;
class QTimer;
class ImgListDlg;
class KFM;

/**
* @short KImageViewer
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id$
*/
class KImageViewer : public KTopLevelWidget
{
	Q_OBJECT
public:
	/**
	* KImageViewer Constructor
	*/
	KImageViewer();

	/**
	* KImageViewer Destructor
	*/
	virtual ~KImageViewer();

	/**
	* This should be called _before_ show!
	*/
	void setFilterMenu( KFiltMenuFactory *filters );

public slots:
	// File operations
	/** */
	void load();
	/** */
	void saveAs();

	/** */
	void closeWindow();
	/** */
	void quitApp();

	// Size
	/** */
	void zoomIn10();
	/** */
	void zoomOut10();
	/** */
	void zoomIn200();
	/** */
	void zoomOut50();
	/** */
	void zoomCustom();

	// Desktop
	void tile();
	void max();
	void maxpect();

	// Transformation
	/** */
	void rotateClock();
	/** */
	void rotateAntiClock();
	/** */
	void flipVertical();
	/** */
	void flipHorizontal();

	void reset();

	// Information
	/** */
	void help();
	/** */
	void about();

	/**
	* Loads a file from a URL.
	*/
	void loadURL( const char *url );

	/**
	* Queues into the view list. if
	*/
	void appendURL( const char *url, bool show = false );

	/**
	* Toggle full screen mode.
	*/
	void fullScreen();

protected:

	virtual void mousePressEvent( QMouseEvent * );

	virtual void saveProperties( KConfig * ) const;
	virtual void restoreProperties( KConfig * );

	virtual void saveOptions( KConfig * ) const;
	virtual void restoreOptions( const KConfig * );

private:
	enum TransferDir {
		Get,
		Put
	};

	/** */
	KImageCanvas	*_canvas;
	bool		_imageLoaded;
	
	int		_barFilterID;
	int		_popFilterID;
	
	QAccel		*_accel;

	KMenuBar	*_menubar;
	KStatusBar	*_statusbar;
	QPopupMenu	*_contextMenu;

	QPopupMenu	*_file;
	QPopupMenu	*_zoom;
	QPopupMenu	*_transform;
	QPopupMenu	*_desktop;
	QPopupMenu	*_aggreg;
	QPopupMenu	*_help;

	KFM		*_kfm;
	QString		*_transSrc;
	QString		*_transDest;
	TransferDir	_transDir;

	void makeRootMenu(QMenuData *);
	void makeRootMenu(KMenuBar * );
	void makePopupMenus();

	KFiltMenuFactory *_menuFact;

	QString 	*_pctBuffer;
	int		_lastPct;

	QTimer		*_msgTimer;

	ImgListDlg	*_imageList;

	QPoint		_posSave;
	QSize		_sizeSave;
	QWMatrix	_mat;

	int		_zoomFactor;

	void loadFile( const char *file, const char *url = 0 );

private slots:

	void urlFetchDone();
	void urlFetchError( int code, const char *text );

	void invokeFilter( KImageFilter *filter );

	void setStatus( const char *status );
	void message( const char *message );
	void setProgress( int pct );

	void toggleImageList();

	void setSize();
};

#endif // SSK_VIEWER_H
