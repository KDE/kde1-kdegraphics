
/////////////////////
//
// wview.cpp -- Methods for kview application widget.
//
// Sirtaj Kang, Oct 1996.
//
// wview.cpp,v 0.9 1997/03/15 09:52:36 ssk Exp

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qfiledlg.h>
#include <qaccel.h>

#include "wview.h"
#include "wview.moc"

#include "image_bmp.h"

#include "QwViewport.h"
#include "viewport.h"
#include "kerror.h"

#include <kurl.h>
//extern    QStrList  Fileman::fileList;

int winCount=0;
QList<WView> windowList;
KViewError WView::errHandler;

WView::WView(QWidget *parent=0, const char *name=0, WFlags f = 0)
	: QWidget(parent, name, f)
{
	int id;
	imagePath=0;
	lastPath=0;
	kfm = 0; // no connection to kfm;

	// no image yet;
	loaded = FALSE;
	loadSuccess = FALSE;
	currentFilename = QString("::no file loaded");

	QPixmap myIcon;
	myIcon.loadFromData(image_bmp_data, image_bmp_len);
	setIcon(myIcon);

	// Tell proggie about new window
	windowList.setAutoDelete(FALSE);
	windowList.append(this);
	++winCount;

	// Caption
	if (name)
	setCaption(name);

	// Martin
	imageWidth = 0;
	imageHeight = 0;
	
	
	///////////////// Menus //////////////

	Scroller = new QwViewport(this);
	Viewport = new WViewPort(0, Scroller->portHole());

	Menu = new QMenuBar(this);
	File = new QPopupMenu;
	Image = new QPopupMenu;
	Help = new QPopupMenu;

	ImageZoom = new QPopupMenu; 
	ImageRotate = new QPopupMenu; 
	ImageRoot = new QPopupMenu;

	// Main Menu

	Menu->insertItem("&File", File);
	Menu->insertItem("&Image", Image);
	Menu->insertSeparator();
	Menu->insertItem("&Help", Help);

	// File Menu

	File->insertItem("&Open File..", this, SLOT(loadImage()), CTRL+Key_O);
	File->insertItem("Open &URL..", this, SLOT(loadURL()), CTRL+Key_U);
	File->insertSeparator();
	File->insertSeparator();
	File->insertItem("&Close",this, SLOT(closeWindow()),CTRL+Key_C);
	File->insertItem("E&xit", qApp, SLOT(quit()), ALT + Key_Q );

	// Image Menu
	Image->insertItem("&Zoom", ImageZoom);
	Image->insertItem("&Rotate", ImageRotate);
	Image->insertItem("To &Desktop", ImageRoot);
	Image->insertSeparator();
	Image->insertItem("&Toggle Menu",this,SLOT(toggleMenu()));
	Image->insertItem("&Size Window", this, SLOT(sizeWindow()));
	Image->insertSeparator();
	Image->insertItem("I&mage Info", this, SLOT(imageInfo()));

	// Image/Zoom

	ImageZoom->insertItem("&in", this, SLOT(zoomIn()), Key_Plus);
	ImageZoom->insertItem("&out", this, SLOT(zoomOut()), Key_Minus);

	// Image/Rotate

	ImageRotate->insertItem("&Clockwise",this,SLOT(rotateClockwise()),
		Key_Greater);
	ImageRotate->insertItem("&Anti-clock", this, 
		SLOT(rotateAntiClockwise()), Key_Less);

	// Image/Root

	ImageRoot->insertItem("Ti&le", this, SLOT(tileToDesktop()));
	ImageRoot->insertItem("&Max Size", this, SLOT(maxToDesktop()));
	ImageRoot->insertItem("M&axpect", this, SLOT(maxpectToDesktop()));



	// Help Menu

	id= Help->insertItem("&Contents", this, SLOT(launchHelp()));


	// Toggle menu if Image is clicked.
	// "Feature": _must_ click on image (doesn't work with Viewport).

	
	connect(Viewport,SIGNAL(clicked()), this, SLOT(toggleMenu()));
	//connect(Viewport,SIGNAL(showDispManager()),
	//	this, SLOT(showManager()));
	connect(Viewport,SIGNAL(doResize()), this, SLOT(sizeWindow()));
	// Tells Scroller that image has changed size

	connect(Viewport,SIGNAL(resized()),Scroller, SLOT(resizeScrollBars()));

	// Quit on Q (more xv-ness)

	myAccel = new QAccel(this);
	myAccel->connectItem( myAccel->insertItem(Key_Q), qApp, SLOT(quit()) );


	// Set up screenlayout

	Scroller->move(0,Menu->height());
	Scroller->resize(width(),height()-Menu->height());
	Viewport->resize(Scroller->portHole()->width(),
			Scroller->portHole()->height());

	// DND Destination

        KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);

        connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
		this, SLOT( slotDropEvent( KDNDDropZone *) ) ); 

	connect(this, SIGNAL( kviewError( KViewError::Type ) ), 
		&errHandler, SLOT( showMessage( KViewError::Type ) ) );
	toggleMenu();

}

void WView::slotDropEvent( KDNDDropZone * dropZone ){
	QStrList & urlList = dropZone->getURLList();
	WView *viewWin=0;
	char *url;

	for( url = urlList.first(); url != 0; url = urlList.next() ) {
		if ( url == urlList.getFirst() ) {
			loadNetFile(url);
		}
		else {
			viewWin = new WView(0,0,0);

			if(viewWin && viewWin->loadNetFile( url ))
				viewWin->show();
		}
	}
}

bool WView::loadLocal(const char *filename)
{
	QFileInfo file(filename);

	if( !file.exists() ) {
		emit kviewError( KViewError::badFileName );
		if (!loaded) sendCloseSignal();
		loadSuccess = FALSE;
		return FALSE;
	}

	if( !file.isReadable() ) {
		emit kviewError( KViewError::accessDenied );
		if (!loaded) sendCloseSignal();
		loadSuccess = FALSE;
		return FALSE;
	}

	loadSuccess=Viewport->load(filename);

	if(loadSuccess){
		
		if(!Menu->isVisible())
			resize(Viewport->width(),Viewport->height());

		imageFormat = QPixmap::imageFormat(filename);

		// We know we have an image in the window now.
		loaded = TRUE;

	} else {
		emit kviewError( KViewError::badFormat );
		if (!loaded) sendCloseSignal();
	}

	return loadSuccess;
}

void WView::loadImage()
{
	QString filename(QFileDialog::getOpenFileName(lastPath, 0,0, 
		"kview: Open...") );
	
	if( filename.isEmpty() )
		return;

	load(filename);
}

void WView::loadURL()
{
	DlgLocation *locDlg = new DlgLocation("Enter URL to image", "");
	locDlg->setCaption("kview: enter URL..");	
	locDlg->show();	

	QString result = locDlg->getText();
	
	result.detach();

	delete locDlg;

	if(!result.isEmpty())
		loadNetFile(result.data());
}


void WView::toggleMenu()
{
  //    if(Menu->isVisible()){

        	Menu->hide();
		Scroller->move(0,0);
		Scroller->resize(width(),height());
   /*	}
	else {
	        Menu->show();
	  	Scroller->move(0, Menu->height());
	  	Scroller->resize(width(),height()-Menu->height());
		}
   */
		
		int item = windowList.findRef(this);
		emit windowClicked(item);
}

void WView::sizeWindow()
{
        Menu->hide();
	Scroller->move(0,0);
	resize(Viewport->width(), Viewport->height());
}
	

void WView::resizeEvent(QResizeEvent *)
{
	if(Menu->isVisible())
		Scroller->resize(width(), height()-Menu->height());
	else
		Scroller->resize(width(), height());

	//tell Viewport the size of the parent widget
	Viewport->parwidth=width();
	Viewport->parheight=height();
}

void WView::zoomIn()
{
	if(!loaded) return;

        Viewport->scale((float)1.5, (float)1.5);
	repaint();
}

void WView::zoomOut()
{
	if(!loaded) return;

        Viewport->scale((float)0.666666666666666666666666666666666666,
		        (float)0.6666666666666666666666666666666666);
	repaint();
}      
void WView::rotateClockwise(){
	if(!loaded) return;

	Viewport->rotateClockwise();

	repaint();
}

void WView::rotateAntiClockwise(){
	if(!loaded) return;

	Viewport->rotateAntiClockwise();

	repaint();
}

float WView::screenScale(){

	QWidget *deskWidget = qApp->desktop();
	QPixmap *currPix = Viewport->pixmap();

	float S = (float)deskWidget->height()/(float)deskWidget->width(),
		I=(float)currPix->height()/(float)currPix->width();

	if (S < I)
		return (float)deskWidget->height()/(float)currPix->height();
	else
		return (float)deskWidget->width()/(float)currPix->width();
}

void WView::maxToDesktop(){

	if(!loaded) return;

        QWMatrix mat;
        QPixmap image(*Viewport->pixmap());
	QWidget *deskWidget = qApp->desktop();
	float swid, shei;

	shei= (float)deskWidget->height()/(float)image.height();
	swid= (float)deskWidget->width()/(float)image.width();

	mat.scale(swid, shei);

        deskWidget->setBackgroundPixmap(image.xForm(mat));

        repaint();
}

void WView::maxpectToDesktop(){

	if(!loaded) return;

	float sc = screenScale();
	QWMatrix mat;
	QPixmap image(*Viewport->pixmap());

	mat.scale(sc,sc);
	
	qApp->desktop()->setBackgroundPixmap(image.xForm(mat));

	repaint();
}

void WView::tileToDesktop(){
	if(!loaded) return;

	qApp->desktop()->setBackgroundPixmap(*Viewport->pixmap());
}

WView::~WView(){
	windowList.removeRef(this);
	--winCount;
}



void WView::closeEvent( QCloseEvent *)
{
  //  int item = windowList.findRef(this);
  //  printf("close %d \n",item);
  //  closeWindow();
  sendCloseSignal();
}

void WView::sendCloseSignal()
{
  int item = windowList.findRef(this);
  //printf("close %d \n",item);
  emit closeClicked(item);
}

void WView::closeWindow()
{
  if(winCount > 1)
    delete this;
  else
    qApp->quit();
}




void WView::launchHelp(){

	if ( fork() == 0 )
	{
	        QString arg = "file:";
	        arg += getenv( "KDEDIR" );
	        arg += "/doc/HTML/kview.html";
	        execlp( "kdehelp", "kdehelp", arg.data(), 0 );
	        exit( 1 );
	}
}

bool WView::load(const char *file){
	if( !strchr(file,':') ) {
		loadLocal(file);
		
		if(loadSuccess){
			imagePath = file;
			currentFilename = imagePath; //Martin
			currentFilename.detach();
			redrawCaption();
			QPixmap * image = Viewport->realPixmap();
			imageWidth = image->width();
			imageHeight= image->height();
			emit doUpdate(1);
		}
			
	}
	else
		loadNetFile(file);
	return loadSuccess;
}

bool WView::loadNetFile(const char *file) {
	KURL url(file);

	if ( url.isMalformed() ) {
		emit kviewError( KViewError::badURL );
		return FALSE;
        }

	// load local file if it's just a "file:" url

	if( !strcmp(url.protocol(), "file") ) {
		load( url.path() );
		return loadSuccess;
	}


	// check if we're already running a kfm job.
	if ( kfm ) {
		emit kviewError( KViewError::ioJobRunning );
		return FALSE;
	}

	// new kfm connection
	kfm = new KFM;

	if( !kfm || !kfm->isOK() ) {
		QMessageBox::message( "Couldn't run kfm",
		"the KDE file manager is either not installed,\n\r"
		"or not installed properly. kview won't be able\n\r"
		"to load internet files.", "Bummer");

		return FALSE;
	}

	tmpFile = tmpnam(NULL);
	tmpFile.detach();
	netFile = file;
	netFile.detach();

	connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
	kfm->copy( netFile.data(), tmpFile.data() );
	kfmAction = WView::GET;

	return TRUE;
}

void WView::slotKFMFinished(){
	if ( kfmAction == WView::GET ) {

	        KURL u( tmpFile.data() );
	        loadLocal( u.path() );

		if(loadSuccess) {
		        imagePath = netFile.data();
			currentFilename = netFile;  //Martin
			currentFilename.detach();
			redrawCaption();
			QPixmap * image = Viewport->realPixmap();
			imageWidth = image->width();
			imageHeight= image->height();
			emit doUpdate(3);
		}

	        // Clean up
	        unlink( tmpFile.data() );
	}

	if ( kfmAction == WView::PUT ) {

	        unlink( tmpFile.data() );
	}

	disconnect( kfm );
	delete kfm;
	kfm = 0L;
}

void WView::imageInfo()
{
	QPixmap *image= Viewport->realPixmap();
        QString Info, num;

	if(!loaded)
		return;

        Info = imagePath + " : " + num.setNum(image->width())
                + "x" + num.setNum(image->height()) + " " +
                imageFormat;

        QMessageBox::message("Image Info", Info, "Ok");
}

void WView::redrawCaption()
{
  QString windowname;
  windowname.sprintf("KView(%d) ",windowList.findRef(this)+1);
  windowname += currentFilename;
  setCaption(windowname);
}


//------------------------------------------------------------
//------------------------------------------------------------





