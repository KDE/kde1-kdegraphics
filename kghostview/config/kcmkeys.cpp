
//
// KDE Display background setup module
//
// Copyright (c)  Martin R. Jones 1996
//
// Converted to a kcc module by Matthias Hoelzer 1997
//

#ifdef HAVE_CONFIG
#include <config.h>
#endif

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qimage.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qfiledlg.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include <qpainter.h>
#include <qlayout.h>

#include <kapp.h>
#include <kwm.h>
#include <kiconloader.h>
#include <ksimpleconfig.h>
#include <kbuttonbox.h>

#include <X11/Xlib.h>

#include "kaccel.h"


#include "kcmkeys.h"
#include "kcmkeys.moc"


//----------------------------------------------------------------------------

KShortcuts::KShortcuts( QWidget *parent, int mode, int desktop )
	: KDisplayModule( parent, mode, desktop )
{
	dict.setAutoDelete( false );
	keys = new KAccel( this );
	
	keys->insertItem(i18n("Quit"), "CTRL+Q");
	keys->insertItem(i18n("Open"), "CTRL+O");
	keys->insertItem(i18n("New"), "CTRL+N");
	keys->insertItem(i18n("Close"), "CTRL+W");
	keys->insertItem(i18n("Print"), "CTRL+P");
	keys->insertItem(i18n("Help"), "F1");
	keys->insertItem(i18n("Next"), "Next");
	keys->insertItem(i18n("Prior"), "Prior");
	keys->insertItem(i18n("Cut"), "CTRL+X");
	keys->insertItem(i18n("Copy"), "CTRL+C");
	keys->insertItem(i18n("Paste"), "CTRL+V");
	keys->insertItem(i18n("Undo"), "CTRL+Z");
	keys->insertItem(i18n("Find"), "CTRL+F");
	keys->insertItem(i18n("Replace"), "CTRL+R");
	keys->insertItem(i18n("Insert"), "CTRL+Insert");
	keys->insertItem(i18n("Home"), "CTRL+Home");
	keys->insertItem(i18n("End"), "CTRL+End");
	
	keys->insertItem(i18n("Switch to next window"), "ALT+Tab");
	keys->insertItem(i18n("Switch to previous window"), "SHIFT+ALT+Tab");
	keys->insertItem(i18n("Switch to next desktop"), "CTRL+Tab");
	keys->insertItem(i18n("Switch to previous desktop"), "SHIFT+CTRL+Tab");
	keys->insertItem(i18n("Task manager"), "CTRL+Escape");
	keys->insertItem(i18n("Toggle kill mode"), "CTRL+ALT+Escape");
	keys->insertItem(i18n("Execute command"), "ALT+F2");
	keys->insertItem(i18n("Pop-up window operations menu"), "ALT+F3");
	keys->insertItem(i18n("Close window"), "ALT+F4");
	keys->insertItem(i18n("Iconify window"), "");
	keys->insertItem(i18n("Resize window"), "");
	keys->insertItem(i18n("Move window"), "");
	keys->insertItem(i18n("Make window sticky"), "");
	keys->insertItem(i18n("Pop-up system menu"), "ALT+F1");
	
	keys->insertItem(i18n("Switch to first desktop"), "CTRL+F1");
	keys->insertItem(i18n("Switch to second desktop"), "CTRL+F2");
	keys->insertItem(i18n("Switch to third desktop"), "CTRL+F3");
	keys->insertItem(i18n("Switch to fourth desktop"), "CTRL+F4");
	keys->insertItem(i18n("Switch to fifth desktop"), "CTRL+F5");
	keys->insertItem(i18n("Switch to sixth desktop"), "CTRL+F6");
	keys->insertItem(i18n("Switch to seventh desktop"), "CTRL+F7");
	keys->insertItem(i18n("Switch to eighth desktop"), "CTRL+F8");
	
	debug("inserted keys");
	
	keys->setConfig( "Keys", true );
	keys->readSettings();
	
	debug("read settings");
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 10 );
	
	QBoxLayout *stackLayout = new QVBoxLayout( this, 4 );
	
	topLayout->addLayout( stackLayout );
	
	QListBox *lbSchemes = new QListBox( this );
	lbSchemes->insertItem( "Current scheme" );
	lbSchemes->insertItem( "KDE default" );
	lbSchemes->insertItem( "KDE lite" );
	
	lbSchemes->adjustSize();
	lbSchemes->setMinimumHeight( 2*lbSchemes->height() );
	
	QLabel *label = new QLabel( lbSchemes, "&Key scheme", this );
	label->adjustSize();
	label->setFixedHeight( label->height() );
	
	stackLayout->addWidget( label );
	stackLayout->addWidget( lbSchemes );
	
	dict = keys->keyDict();
	
	debug("got key dict");
	
	kc =  new KKeyChooser( &dict, this );
	
	debug("Make key chooser");
	
	topLayout->addWidget( kc, 10 );
	
	topLayout->activate();
	
	//keys->setKeyDict( dave );
}

void KShortcuts::resizeEvent( QResizeEvent * )
{
 
}

void KShortcuts::readSettings( int num )
{
  
}

void KShortcuts::writeSettings( int num )
{
	debug("Writing key settings");
	keys->writeSettings();
}

void KShortcuts::getDeskNameList()
{
   
}

void KShortcuts::setDesktop( int desk )
{
   
}

void KShortcuts::showSettings()
{ 
   
}

void KShortcuts::slotApply()
{
	writeSettings();
}

void KShortcuts::apply( bool force )
{
	
}

void KShortcuts::retainResources() {
	
}

void KShortcuts::setMonitor()
{
   
    
}

// Attempts to load the specified wallpaper and creates a centred/scaled
// version if necessary.
// Note that centred pixmaps are placed on a full screen image of background
// color1, so if you want to save memory use a small tiled pixmap.
//
int KShortcuts::loadWallpaper( const char *name, bool useContext )
{
	
}

void KShortcuts::slotSelectColor1( const QColor &col )
{
	
}

void KShortcuts::slotSelectColor2( const QColor &col )
{

}

void KShortcuts::slotBrowse()
{
	
}

void KShortcuts::slotWallpaper( const char *filename )
{
   
}

void KShortcuts::slotWallpaperMode( int m )
{

}

void KShortcuts::slotColorMode( int m )
{
	
}

void KShortcuts::slotSetup2Color()
{
   
}

void KShortcuts::slotStyleMode( int m )
{
   
}

void KShortcuts::slotSwitchDesk( int num )
{
   
}

void KShortcuts::slotRenameDesk()
{
   
}

void KShortcuts::slotHelp()
{

}

void KShortcuts::loadSettings()
{
   
}

void KShortcuts::applySettings()
{
	debug("apply settings");
	debug("No. of items in dict %d", dict.count() );
	keys->setKeyDict( dict );
	debug("set key dict");
    writeSettings();
}
