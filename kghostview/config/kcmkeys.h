//
// KDE Shotcut config module
//
// Copyright (c)  Mark Donohoe 1998
//

#ifndef __SHORTCUTS_H__
#define __SHORTCUTS_H__

#include <qwidget.h>
#include <qpushbt.h>
#include <qcombo.h>
#include <qlistbox.h>
#include <qradiobt.h>
#include <qlined.h>
#include <qbttngrp.h>
#include <kcontrol.h>

#include "display.h"

#include "kaccel.h"
#include "kkeydialog.h"

class KShortcuts : public KDisplayModule
{
	Q_OBJECT
public:
	KAccel *keys;
	QDict<KKeyEntry> dict;
	KKeyChooser *kc;
	enum { Portrait = 1, Landscape };
	enum { Gradient = 1, Pattern };

	KShortcuts( QWidget *parent, int mode, int desktop = 0 );

	virtual void readSettings( int deskNum = 0 );
	virtual void apply( bool force = FALSE );

        virtual void loadSettings();
        virtual void applySettings();

protected slots:
	void slotApply();
	void slotSelectColor1( const QColor &col );
	void slotSelectColor2( const QColor &col );
	void slotBrowse();
	void slotWallpaper( const char * );
	void slotWallpaperMode( int );
	void slotColorMode( int );
	void slotStyleMode( int );
	void slotSwitchDesk( int );
	void slotRenameDesk();
	void slotHelp();
	void slotSetup2Color();
	void resizeEvent( QResizeEvent * );

protected:
	void getDeskNameList();
	void setDesktop( int );
	void showSettings();
	void writeSettings( int deskNum = 0 );
	void setMonitor();
	int  loadWallpaper( const char *filename, bool useContext = TRUE );
	void retainResources();

protected:
	enum { Tiled = 1, Centred, Scaled };
	enum { OneColor = 1, TwoColor };
	
	

	QListBox     *deskListBox;
	QRadioButton *rbPattern;
	QRadioButton *rbGradient;
	QButtonGroup *ncGroup;
	QButtonGroup *stGroup;
	QButtonGroup *wpGroup;
	QPushButton  *changeButton;
	QComboBox *wpCombo;
	QColor color1;
	QColor color2;
	QString wallpaper;
	QPixmap wpPixmap;
	QString deskName;
	QStrList deskNames;
	int wpMode;
	int ncMode;
	int stMode;
	int orMode;
	int deskNum;
	int maxDesks;

        uint pattern[8];

	bool changed;
};


#endif

