/*
* ilistdlg.h -- Declaration of class ImgListDlg.
* Generated by newclass on Wed Sep 23 16:04:52 EST 1998.
*/
#ifndef SSK_ILISTDLG_H
#define SSK_ILISTDLG_H

#include<qwidget.h>
#include<qtimer.h>

class QStrList;
class QListBox;
class QPushButton;
class KDNDDropZone;
class QCheckBox;

/**
* Image List dialog.
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id$
*/
class ImgListDlg : public QWidget
{
	Q_OBJECT

public:
	/**
	* ImgListDlg Constructor
	*/
	ImgListDlg( QWidget *parent = 0 );

	/**
	* ImgListDlg Destructor
	*/
	virtual ~ImgListDlg();
	
	void addURL( const char *, bool show = true );
	void addURLList( const QStrList& list );

public slots:

	void first();
	void last();
	
	void prev();
	void next();
	
	void shuffle();

	void startSlideShow();
	void stopSlideShow();
	void toggleSlideShow();

	void pauseSlideShow();
	void continueSlideShow();
	
	bool slideShowRunning() const;
	void setSlideInterval( int seconds );

	void dropEvent( KDNDDropZone * );

signals:
	void selected( const char *url );

private slots:
	/**
	* Selects image at specified index.
	*/
	void select( int imageIndex );

	void nextSlide();
	void setInterval();

private:
	ImgListDlg& operator=( ImgListDlg& );
	ImgListDlg( const ImgListDlg& );

	QTimer		*_slideTimer;
	int		_slideInterval;
	QPushButton	*_slideButton;
	QCheckBox	*_loop;
	bool		_paused;
	
	QStrList	*_list;
	QListBox	*_listBox;
};

inline bool ImgListDlg::slideShowRunning() const
{
	return ( _slideTimer && _slideTimer->isActive() );
}
	
#endif // SSK_ILISTDLG_H
