
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


#include "global.h"
#include "global.moc"


//----------------------------------------------------------------------------

KGlobalConfig::KGlobalConfig( QWidget *parent, const char *name )
	: KConfigWidget( parent, name )
{
	dict.setAutoDelete( false );
	keys = new KAccel( this );
	
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
	
	keys->setConfigGlobal( true );
	keys->setConfigGroup( "Global Keys" );
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

void KGlobalConfig::resizeEvent( QResizeEvent * )
{
 
}

void KGlobalConfig::readSettings( )
{
  
}

void KGlobalConfig::writeSettings(  )
{
	debug("Writing key settings");
	keys->writeSettings();
}

void KGlobalConfig::getDeskNameList()
{
   
}

void KGlobalConfig::setDesktop( int desk )
{
   
}

void KGlobalConfig::showSettings()
{ 
   
}

void KGlobalConfig::slotApply()
{
	writeSettings();
}

void KGlobalConfig::apply( bool force )
{
	
}

void KGlobalConfig::retainResources() {
	
}

void KGlobalConfig::setMonitor()
{
   
    
}

// Attempts to load the specified wallpaper and creates a centred/scaled
// version if necessary.
// Note that centred pixmaps are placed on a full screen image of background
// color1, so if you want to save memory use a small tiled pixmap.
//
int KGlobalConfig::loadWallpaper( const char *name, bool useContext )
{
	
}

void KGlobalConfig::slotSelectColor1( const QColor &col )
{
	
}

void KGlobalConfig::slotSelectColor2( const QColor &col )
{

}

void KGlobalConfig::slotBrowse()
{
	
}

void KGlobalConfig::slotWallpaper( const char *filename )
{
   
}

void KGlobalConfig::slotWallpaperMode( int m )
{

}

void KGlobalConfig::slotColorMode( int m )
{
	
}

void KGlobalConfig::slotSetup2Color()
{
   
}

void KGlobalConfig::slotStyleMode( int m )
{
   
}

void KGlobalConfig::slotSwitchDesk( int num )
{
   
}

void KGlobalConfig::slotRenameDesk()
{
   
}

void KGlobalConfig::slotHelp()
{

}

void KGlobalConfig::loadSettings()
{
   
}

void KGlobalConfig::applySettings()
{
	debug("apply settings");
	debug("No. of items in dict %d", dict.count() );
	keys->setKeyDict( dict );
	debug("set key dict");
    writeSettings();
}
