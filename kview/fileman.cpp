//////////////////////////
//
// fileman.cpp -- The KView Display Manager Widget
//
// Martin Hartig
//
// $Id$
//////////////////////////

#define ID_CACHING   1
#define ID_CACHESIZE 2

#define ID_CZ_256  256
#define ID_CZ_1024 1024
#define ID_CZ_4096 4096
#define ID_CZ_8192 8192


#include <qevent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <qpixmap.h>
#include <qfiledlg.h>

#include <kurl.h>
#include <kiconloader.h>

#include "fileman.h"
#include "fileman.moc"

#include "version.h"
#include "toolpics.h"
#include "confighandler.h"

// global (shared) variables
QStrList fileList;

QList<Fileman> Fileman::manList;
int            Fileman::manCount;

extern KApplication *theApp;
extern QList<WView> windowList;



Fileman::Fileman(const char *name, WView *)
    : KTopLevelWidget(name)
{
  setMinimumSize(250, 350);
  setMaximumSize(700, 1400);  
  setCaption("KView Display Manager");

  // count all display manager windows
  manList.setAutoDelete(FALSE);
  manList.append(this);
  ++manCount;

  imageWindow=0L;

  // setup the display manager widget
  initMenuBar();
  initToolBar();
  initStatusBar();
  initMainWidget();
 
  setFrameBorderWidth (1);
  setView(mainwidget, FALSE);

  // fill the listbox
  updateListbox(3);
  
  // show display manager
  int sizeX = KVConfigHandler::frameSizeX;
  int sizeY = KVConfigHandler::frameSizeY;

  resize(sizeX,sizeY);
  show();
  resize(sizeX,sizeY);
  doPositioning();
  
  // other stuff
  showrunning = FALSE;
  fileList.setAutoDelete( TRUE );
  timerDelay = KVConfigHandler::delay;
  lastPath = 0;

  // DnD support 
  KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
  connect(dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
		this, SLOT( slotDropEvent( KDNDDropZone *) ) ); 
 
}

void Fileman::initMenuBar()
{
  file = new QPopupMenu ();
  file->insertItem("New Window",   this, SLOT(slotNew()));
  file->insertItem("Open...",      this, SLOT(slotOpen()));
  file->insertItem("Open URL...",  this, SLOT(slotOpenUrl()));
  file->insertItem("Quit",         this, SLOT(closeWindow()));

  cacheSize = new QPopupMenu();
  CHECK_PTR( cacheSize );
  cacheSize->insertItem("256 KB", ID_CZ_256);
  if (KVConfigHandler::cacheSize==256)
    cacheSize->setItemChecked(ID_CZ_256,  TRUE);
  cacheSize->insertItem("1024 KB", ID_CZ_1024);
  if (KVConfigHandler::cacheSize==1024)
    cacheSize->setItemChecked(ID_CZ_1024,  TRUE);
  cacheSize->insertItem("4096 KB", ID_CZ_4096);
  if (KVConfigHandler::cacheSize==4096)
    cacheSize->setItemChecked(ID_CZ_4096,  TRUE);
  cacheSize->insertItem("8192 KB", ID_CZ_8192);
  if (KVConfigHandler::cacheSize==1892)
    cacheSize->setItemChecked(ID_CZ_8192,  TRUE);
  connect(cacheSize, SIGNAL( activated( int ) ), this,
	  SLOT( setCacheSize( int ) ) );
  
  
  options = new QPopupMenu();
  CHECK_PTR( options );
  options->insertItem("Image Caching" ,ID_CACHING);
  connect(options, SIGNAL( activated( int ) ), this,
	  SLOT( imageCaching( int ) ) );
  if (KVConfigHandler::cachingOn == 0)
    options->setItemChecked( ID_CACHING, FALSE );
  else
    options->setItemChecked( ID_CACHING, TRUE );
 
  options->insertItem("Cache Size", cacheSize);
  options->insertSeparator();
  options->insertItem("Save Options", this, SLOT(saveOptions()));
		      
 
  help = new QPopupMenu ();
  help->insertItem("About KView",  this, SLOT(aboutKview()));
  help->insertItem("Help on KView",this, SLOT(invokeHelp()));

  
  menubar = new KMenuBar(this,"menubar");
  menubar->insertItem(i18n("File"), file);
  menubar->insertItem(i18n("Options"), options);
  //menubar->insertItem(i18n("Cache"), cacheSize);
  menubar->insertItem(i18n("Help"), help);
  setMenu(menubar);
}

void Fileman::initToolBar()
{
  ktoolbar1 = new KToolBar( this );
  ktoolbar2 = new KToolBar( this );
  
  QPixmap pixmap;

  //------------- initialize the first toolbar
  pixmap = theApp->getIconLoader()->loadIcon("filenew.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_NEWWINDOW,
			SIGNAL(clicked()), this,
			SLOT(slotNew()), TRUE,
			"New Window");

  pixmap = theApp->getIconLoader()->loadIcon("fileopen.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_OPENFILE,
			SIGNAL(clicked()), this,
			SLOT(slotOpen()), TRUE,
			"open new file");
  /*
  pixmap = theApp->getIconLoader()->loadIcon("reload.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_RELOAD,
			SIGNAL(clicked()), this,
			SLOT(slotReload()), TRUE,
			"reload file list");
  */
  
  ktoolbar1->insertSeparator();
  
  pixmap = theApp->getIconLoader()->loadIcon("viewmag-.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_MINIFY,
			SIGNAL(clicked()), this,
			SLOT(slotMin()), TRUE,
			"shrink -50%");

  pixmap = theApp->getIconLoader()->loadIcon("viewmag+.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_MAGNIFY,
			SIGNAL(clicked()), this,
			SLOT(slotMax()), TRUE,
			"magnify +50%");
  
  pixmap.loadFromData(resize_xpm_data, resize_xpm_len);
  ktoolbar1->insertButton(pixmap, ID_T_RESIZE,
			SIGNAL(clicked()), this,
			SLOT(slotResize()), TRUE,
			"size Window");
  ktoolbar1->insertSeparator();

  pixmap = theApp->getIconLoader()->loadIcon("help.xpm");
  ktoolbar1->insertButton(pixmap, ID_T_HELP,
			SIGNAL(clicked()), this,
			SLOT(invokeHelp()), TRUE,
			"launch help");

  //--------------- initialize the second toolbar
  pixmap = theApp->getIconLoader()->loadIcon("start.xpm");
  ktoolbar2->insertButton(pixmap, ID_T_FIRSTPIC,
			SIGNAL(clicked()), this,
			SLOT(firstClicked()), TRUE,
			"first pic");

  pixmap = theApp->getIconLoader()->loadIcon("finish.xpm");
  ktoolbar2->insertButton(pixmap, ID_T_LASTPIC,
			SIGNAL(clicked()), this,
			SLOT(lastClicked()), TRUE,
			"last pic");
  ktoolbar2->insertSeparator();

  pixmap = theApp->getIconLoader()->loadIcon("back.xpm");
  ktoolbar2->insertButton(pixmap, ID_T_PREVPIC,
			SIGNAL(clicked()), this,
			SLOT(prevClicked()), TRUE,
			"prev pic");

  pixmap = theApp->getIconLoader()->loadIcon("forward.xpm");
  ktoolbar2->insertButton(pixmap, ID_T_NEXTPIC,
			SIGNAL(clicked()), this,
			SLOT(nextClicked()), TRUE,
			"next pic");
  ktoolbar2->insertSeparator();
    
  pixmap.loadFromData(diashow_xpm_data, diashow_xpm_len );
  ktoolbar2->insertButton(pixmap, ID_T_STARTSHOW,
			SIGNAL(clicked()), this,
			SLOT(startShow()), TRUE,
			"start picture show");

  pixmap.loadFromData(random_xpm_data, random_xpm_len );
  ktoolbar2->insertButton(pixmap, ID_T_RANDOMSHOW,
			SIGNAL(clicked()), this,
			SLOT(randomShow()), TRUE,
			"random picture show");
  
  pixmap = theApp->getIconLoader()->loadIcon("stop.xpm");
  ktoolbar2->insertButton(pixmap, ID_T_STOPSHOW,
			SIGNAL(clicked()), this,
			SLOT(stopShow()), TRUE,
			"stop picture show");

  //add the toolbars to the top level widget
  ktoolbar1->setBarPos( KVConfigHandler::toolbar1pos );
  ktoolbar2->setBarPos( KVConfigHandler::toolbar2pos );

  //the order of the 4 folowing lines is decisioning
  iktoolbar1 = addToolBar(ktoolbar1);
  iktoolbar2 = addToolBar(ktoolbar2);

  //do a repositioning of the mainwidget after the toolbars have
  //been moved. Note that a repositioning of the op level widget
  //is been done before
  connect( ktoolbar1, SIGNAL( moved(BarPosition)), this, SLOT( doPositioning()));
  connect( ktoolbar2, SIGNAL( moved(BarPosition)), this, SLOT( doPositioning())); 
}

void Fileman::initMainWidget()
{
  mainwidget = new QWidget(this);
  
  listbox    = new QListBox(mainwidget, "_listbox");
  connect(listbox,SIGNAL(selected(int)),this, SLOT(listboxClicked(int)));

  lineEdit = 0L;

#ifdef LINEEDIT 
  lineEdit = new QLineEdit(mainwidget, "linedit");
  connect( lineEdit, SIGNAL( returnPressed()), SLOT( slotLineEdit()) );
#endif
  
  textSlow = new QLabel( mainwidget, "slow" );
  textSlow->setFont( QFont("times",10));
  textSlow->setText("slow");
  

  textFast = new QLabel( mainwidget, "fast");
  textFast->setFont( QFont("times",10));
  textFast->setText("fast");

  slider = new QSlider(10,50,
		       5,(KVConfigHandler::delay)/100,
		       QSlider::Horizontal, mainwidget);
  slider->setRange(10,50);
  slider->setSteps(5,10);
  QSlider::TickSetting sset;
  sset = (QSlider::TickSetting) 1;
  slider->setTickmarks(sset);
  connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));

  textOutput = new QLabel( mainwidget, "OutputDevice");
  textOutput->setFont( QFont("times",10));
  textOutput->setText("Output Device");

  combobox = new QComboBox(FALSE, mainwidget, "read Combo");
  updateCombobox();
  connect(combobox,SIGNAL(activated(int)),this,SLOT(comboboxSelected(int)));
}

void Fileman::initStatusBar()
{
  statusbar = new KStatusBar( this );
  statusbar->insertItem("Format XXXX  Size 0000x0000",ID_S_IMAGE);
  statusbar->insertItem("",ID_S_FILES);

  statusbar->setInsertOrder(KStatusBar::RightToLeft);
  setStatusBar( statusbar );
}


void Fileman::checkValidButtons()
{
  int maxPic = listbox->count()-1;
  int currentPic = listbox->currentItem();
  
  if (showrunning==FALSE)
    {
      if (currentPic==maxPic)
	{
	  ktoolbar2->setItemEnabled(ID_T_LASTPIC, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_NEXTPIC, FALSE);
	}
      else
	{
	  ktoolbar2->setItemEnabled(ID_T_LASTPIC, TRUE);
	  ktoolbar2->setItemEnabled(ID_T_NEXTPIC, TRUE);
	}
      if (currentPic==0)
	{
	  ktoolbar2->setItemEnabled(ID_T_FIRSTPIC, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_PREVPIC, FALSE);
	}
      else
	{
	  ktoolbar2->setItemEnabled(ID_T_FIRSTPIC, TRUE);
	  ktoolbar2->setItemEnabled(ID_T_PREVPIC, TRUE);
	}
      if (maxPic<0)
	{
	  ktoolbar2->setItemEnabled(ID_T_FIRSTPIC, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_PREVPIC, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_LASTPIC, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_NEXTPIC, FALSE);
	}

      if (maxPic<=0)
	{
	  ktoolbar2->setItemEnabled(ID_T_STARTSHOW, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_RANDOMSHOW, FALSE);
	  ktoolbar2->setItemEnabled(ID_T_STOPSHOW, FALSE);
	}
      else
	{
	  ktoolbar2->setItemEnabled(ID_T_STARTSHOW, TRUE);
	  ktoolbar2->setItemEnabled(ID_T_RANDOMSHOW, TRUE);
	  ktoolbar2->setItemEnabled(ID_T_STOPSHOW, FALSE);
	}
    }
  else
    {
      ktoolbar2->setItemEnabled(ID_T_FIRSTPIC, FALSE);
      ktoolbar2->setItemEnabled(ID_T_PREVPIC, FALSE);
      ktoolbar2->setItemEnabled(ID_T_LASTPIC, FALSE);
      ktoolbar2->setItemEnabled(ID_T_NEXTPIC, FALSE);
      ktoolbar2->setItemEnabled(ID_T_STARTSHOW, FALSE);
      ktoolbar2->setItemEnabled(ID_T_RANDOMSHOW, FALSE);
      ktoolbar2->setItemEnabled(ID_T_STOPSHOW, TRUE);
    }

  // update the statusbar
  QString ptext;
  ptext.sprintf(" Files: %d", listbox->count());
  statusbar->changeItem(ptext.data(),ID_S_FILES);

  char * filename;

  if (imageWindow!=0L)
  {
    filename = (imageWindow->getCurrentFilename());
    if (strcmp(filename,"::no file loaded")!=0)
      {
	ptext.sprintf(" Format: %s Size: %dx%d",
		      imageWindow->getImageType().data(),
		      imageWindow->getImageWidth(),
		      imageWindow->getImageHeight());
	statusbar->changeItem( ptext.data(),ID_S_IMAGE );
      }
    else
      statusbar->changeItem( "", ID_S_IMAGE );
  }
  else
    statusbar->changeItem( "", ID_S_IMAGE );

  // update the lineeditor
  int item= listbox->currentItem();
  
  if (listbox->count() > 0 && item !=-1 && lineEdit != 0L )
    {
	lineEdit->setText( listbox->text( item ) );
    }
}
void Fileman::resizeEvent(QResizeEvent * )
{
  KTopLevelWidget::updateRects();
  doPositioning();
}

void Fileman::doPositioning()
{
  int mheight,mwidth;
  mheight = mainwidget->height();
  
        
  if (lineEdit != 0L)
    {
      mheight -= 30;
      lineEdit->setGeometry(10,
			    mheight,
			    mainwidget->width()- 30,
			    20);
    }
    
  textFast->move(10,mheight-40);
  textSlow->move(mainwidget->width()-40, mheight-40);

  mwidth = mainwidget->width()-50;
  mwidth -= 30; //approx width of "slow"
  mwidth -= 30; //approx width of "fast"
  slider->setGeometry(textFast->x()+40,
		      mheight-35,
		      mwidth,
		      20);
  textOutput->move(textFast->x(), 
		   textFast->y()-30);

  combobox->setGeometry(slider->x()+30,
			slider->y()-30,
			slider->width(),
			20);
  
  listbox->setGeometry(10,
		       10,
		       mainwidget->width()-30,
		       combobox->y()-25);
}


void Fileman::updateListbox(int job)
{
  
  char * text,*filename;
  QString text1,fil;
  int showitem;
  
  // update all entries
  if (job == 3)
    {
      listbox->setAutoUpdate( FALSE );
      listbox->clear();
      for (text=fileList.first();
	   text!=0;
	   text=fileList.next())
	listbox->insertItem(text,-1);
      listbox->setAutoUpdate( TRUE );
      listbox->repaint();
    }

  if (imageWindow!=0L)
    {
      fil = shortenFilename(imageWindow->getCurrentFilename());
      filename = fil.data();
    }
  else
    filename="::no file loaded";
  
  if (strcmp(filename,"::no file loaded")==0)
    {
      checkValidButtons();
      return;
    }
  
  showitem = listbox->currentItem();

  // Quick test
  if (showitem != -1 )
    {
      if ( strcmp(listbox->text(showitem),filename) ==0 )
	{
	  if (listbox->count()>0)
	    listbox->setCurrentItem(showitem);
	  return;
	}
    }

  // Quick test failed

  // search item in fileList
  showitem = fileList.find( filename );
  if ( showitem == -1 )
    {
      // not found
      // so update fileList
      fileList.inSort( filename );
      listbox->inSort( filename );
      
      showitem = fileList.find( filename );
    }

  if (listbox->count()>0)
    listbox->setCurrentItem( showitem );
  checkValidButtons();
}

void Fileman::updateCombobox()
{
  WView *window;
  QString inserttext;
  int i=0,active=0;
  combobox->clear();
  for (window=windowList.first();
       window != 0;
       window=windowList.next())
    {
      i++;
      inserttext.sprintf("Window %d",i);
      combobox->insertItem(inserttext.data(),-1);
      if (window==imageWindow) 
	active = i;
    }
  if(active>0)
    combobox->setCurrentItem(active-1);
  if(i==0) 
    {
      combobox->insertItem("(none)",-1);
      combobox->setCurrentItem(0);
    }
}

void Fileman::slotReload()
{
  char * text;
  int item;

  item = listbox->currentItem();
  listbox->setAutoUpdate( FALSE );
  listbox->clear();
  for (text=fileList.first();
       text!=0;
       text=fileList.next())
    listbox->insertItem(text,-1);
  listbox->setAutoUpdate( TRUE );
  listbox->repaint();

  
  if (listbox->count()>0)
    listbox->setCurrentItem( item );
  
  listbox->centerCurrentItem();
  
  //updateListbox(1);
}

void Fileman::sliderChanged(int val)
{
  timerDelay = val*100;
}

void Fileman::slotNew()
{
  //Fileman *man = new Fileman(0L,0L);
  //man->show();
  //man->firstClicked();
  imageWindow = new WView(0,0L,0);
  connect(imageWindow,SIGNAL(doUpdate(int)),
	  this,SLOT(updateListbox(int)));
  connect(imageWindow,SIGNAL(closeClicked(int)),
	  this,SLOT(WViewCloseClicked(int)));
  connect(imageWindow,SIGNAL(windowClicked(int)),
	      this,SLOT(comboboxSelected(int)));
  imageWindow->show();
  updateCombobox();
  firstClicked();
}

void Fileman::slotOpen()
{
  /*
  if (imageWindow==0L) createWView();
  imageWindow->loadImage();
  if (!imageWindow==0L) imageWindow->show();
  checkValidButtons();
  */
  QString filename(QFileDialog::getOpenFileName(lastPath, 0,0, 
						"kview: Open...") );
	
  if( filename.isEmpty() )
    return;
  if (imageWindow==0L) createWView();
  imageWindow->loadLocal(filename.data());
  if (!imageWindow==0L) imageWindow->show();
  
}

void Fileman::slotOpenUrl()
{
  DlgLocation *locDlg = new DlgLocation("Enter URL to image", "");
  locDlg->setCaption("kview: enter URL..");	
  locDlg->show();	
  
  QString result = locDlg->getText();
  
  result.detach();
  
  delete locDlg;
  
  if(result.isEmpty())
    return;
  
  if (imageWindow==0L) createWView();
  imageWindow->loadNetFile(result.data());
  if (!imageWindow==0L) imageWindow->show();
  checkValidButtons();
        
}


void Fileman::slotMax()
{
  if(imageWindow!=0L)
    imageWindow->zoomIn();
}

void Fileman::slotMin()
{
  if(imageWindow!=0L)
    imageWindow->zoomOut();
}

void Fileman::slotResize()
{
  if(imageWindow!=0L)
    imageWindow->sizeWindow();
}

void Fileman::aboutKview()
{
  QString text;
  text.sprintf("kview %d.%d.%d \n",VERSIONNR,SUBVERSIONNR,PATCHLEVEL);
  text += "(c) 1996 - 1998 by \n";
  text += "Sirtaj Singh Kang,  <taj@kde.org>\n";
  text += "Martin Hartig,  <martinhartig@yahoo.com>";
  QMessageBox::message("About kview", text.data(),
			"Ok"); 
}

void Fileman::saveOptions()
{
  KVConfigHandler::toolbar2pos = ktoolbar2->barPos();
  KVConfigHandler::frameSizeX  = width();
  KVConfigHandler::frameSizeY  = height();
  KVConfigHandler::delay       = timerDelay;
  KVConfigHandler::writeConfigEntries();
  imageCaching(-1);
}

void Fileman::imageCaching(int)
{
  if (KVConfigHandler::cachingOn == 0)
    {
      KVConfigHandler::cachingOn = 1;
      options->setItemChecked( ID_CACHING, TRUE );
    }
  else
    {
      KVConfigHandler::cachingOn = 0;
      options->setItemChecked( ID_CACHING, FALSE );
    }  
}

void Fileman::setCacheSize(int i)
{
  switch(i){
  case ID_CZ_256:
    cacheSize->setItemChecked(ID_CZ_256,  TRUE);
    cacheSize->setItemChecked(ID_CZ_1024, FALSE);
    cacheSize->setItemChecked(ID_CZ_4096, FALSE);
    cacheSize->setItemChecked(ID_CZ_8192, FALSE);
    KVConfigHandler::cacheSize=256;
    break;
  case ID_CZ_1024:
    cacheSize->setItemChecked(ID_CZ_256,  FALSE);
    cacheSize->setItemChecked(ID_CZ_1024, TRUE);
    cacheSize->setItemChecked(ID_CZ_4096, FALSE);
    cacheSize->setItemChecked(ID_CZ_8192, FALSE);
    KVConfigHandler::cacheSize=1024;
    break;
  case ID_CZ_4096:
    cacheSize->setItemChecked(ID_CZ_256,  FALSE);
    cacheSize->setItemChecked(ID_CZ_1024, FALSE);
    cacheSize->setItemChecked(ID_CZ_4096, TRUE);
    cacheSize->setItemChecked(ID_CZ_8192, FALSE);
    KVConfigHandler::cacheSize=4096;
    break;
  case ID_CZ_8192:
    cacheSize->setItemChecked(ID_CZ_256,  FALSE);
    cacheSize->setItemChecked(ID_CZ_1024, FALSE);
    cacheSize->setItemChecked(ID_CZ_4096, FALSE);
    cacheSize->setItemChecked(ID_CZ_8192, TRUE);
    KVConfigHandler::cacheSize=8192;
    break;    
  }
}


void Fileman::invokeHelp()
{
  theApp->invokeHTMLHelp("", "");
}

void Fileman::firstClicked()
{
  
  if (listbox->count()>0)
    {
      listboxClicked(0);
      checkValidButtons();
      if(imageWindow!=0L)
	imageWindow->sizeWindow();
    }
}

void Fileman::prevClicked()
{
  int currentPic = listbox->currentItem();
  if (currentPic > 0)
     listboxClicked(currentPic-1);
  checkValidButtons();
}

void Fileman::nextClicked()
{
  int maxPic = listbox->count()-1;
  int currentPic = listbox->currentItem();
  if (currentPic < maxPic)
      listboxClicked(currentPic+1);
  checkValidButtons();
}

void Fileman::lastClicked()
{
  int maxPic = listbox->count()-1;
  listboxClicked(maxPic);
  checkValidButtons();
}

void Fileman::listboxClicked(int item)
{
  int     olditem;
  bool    load_successful;
  QString f;
  const char *  filename;
  char buffer[ 1024 ];


  olditem = listbox->currentItem();
  if (listbox->count()>0)
    listbox->setCurrentItem(item);
    
  filename = listbox->text(item);
  
  f = filename;
  f.detach();
  
  if ( f.find( ":/" ) == -1 && f.left(1) != "/" )
    {
      getcwd( buffer, 1023 );
      f.sprintf( "%s/%s", buffer, filename );
    }

  if (imageWindow==0L)
    {
      createWView();
      load_successful=imageWindow->load( f.data() );
      // this might look strange, but the image window deletes
      // itself, when a load error occured and no image has been loaded
      // so the existence of the image window has to be checked
      // a second time.
      if (!imageWindow==0L)  imageWindow->show();
    }
  else
    load_successful=imageWindow->load( f.data() );
  
  if (load_successful==FALSE)
    {
      item = fileList.find( filename );
      fileList.remove( item );
      item = olditem;
      slotReload();
    }
    

  checkValidButtons();
}

void Fileman::comboboxSelected(int item)
{
  if(strcmp(combobox->text(item),"(none)")==0)
    {
      imageWindow = 0L;
    }
  else
    {
      imageWindow = windowList.at(item);
      updateListbox(1);
      listbox->centerCurrentItem();
    }
  updateCombobox();
}


//---------------------------Show handling
void Fileman::startShow()
{
  showrunning = TRUE;
  showrandom  = FALSE;
  checkValidButtons();
  timerID = startTimer(timerDelay);
}

void Fileman::randomShow()
{
  showrunning = TRUE;
  showrandom  = TRUE;
  checkValidButtons();
  timerID = startTimer(timerDelay);
  //srand((unsigned) time((long *)0));
  srand((unsigned) time((time_t *)0));
}

void Fileman::stopShow()
{
  showrunning = FALSE; 
  killTimer (timerID);
  checkValidButtons();
}

void Fileman::timerEvent ( QTimerEvent *)
{
  int currentPic = listbox->currentItem();
  int maxPic     = listbox->count()-1;
  
  killTimer (timerID);
  if (showrandom==FALSE)
   {
     if (currentPic!=maxPic)
      listboxClicked(currentPic+1);
    else
      listboxClicked(0);
   }
  else
    listboxClicked(rand()%(maxPic+1));
  
  if (showrunning==TRUE)
    {
      timerID = startTimer(timerDelay);
    }
}


Fileman::~Fileman()
{
  manList.removeRef(this);
  --manCount;

  //clean up
  //delete dropZone;

  delete ktoolbar1;
  delete ktoolbar2;
  delete statusbar;

  delete file;
  delete help;
  delete menubar;
  
  delete listbox;
  delete textFast;
  delete textSlow;
  delete slider;
  if (lineEdit!=0L) delete lineEdit;
  delete mainwidget;	  
  delete imageWindow;
}

void Fileman::closeEvent(QCloseEvent *)
{
  closeWindow();
}

void Fileman::closeWindow()
{
  //saveOptions();
  theApp->quit();
}


//
// Don't look too close at this code - you'll get blind
//

void Fileman::appendFileList(QString filename)
{
  int item;
  item = fileList.find( filename );
  if ( item == -1)
    fileList.inSort( shortenFilename( filename ).data() );
}

QString Fileman::shortenFilename(QString filename)
{
  QString n,n2;
  int len;
  char buffer[1024];
  
  // deep copy
  n=filename;
  n.detach();
  
  
  // first cut "file:"
  if (n.left(5)=="file:")
    {
      len = n.length();
      n2  = n.right(len-5);
      n2.detach();
      n = n2;
      n.detach();
    }

  // then cut the current path
  getcwd(buffer,1023);
  n2 = buffer;
  n2 += "/";
  
  if(n.find(n2.data())==0)
    {
      len=n.length();
      n2 = n.right(len-n2.length());
      n = n2;
      n.detach();
    }
  return( n );
}


// handling of the line editor
// needs some security checks

void Fileman::slotLineEdit()
{
  int item;
  QString yo;
  QString filenameold;
  QString filenamenew;
  bool result;
  
  item = listbox->currentItem();
  
  filenameold = listbox->text(item);
  filenameold.detach();
  if (lineEdit!=0L)
    {
      filenamenew = lineEdit->text();
      filenamenew.detach();
    }
  
  listbox->removeItem( item );
  
  yo.sprintf("mv -i %s %s ", filenameold.data() , filenamenew.data() );
  system(yo.data() );
  printf("%s\n", yo.data() );
  
  item = fileList.find( filenameold.data() );
  printf("%d\n", item);

  result = fileList.remove( item );

  fileList.inSort( filenamenew.data() );

  slotReload();
  if (imageWindow!=0L)
    imageWindow->changeImageName( filenamenew.data() );
  updateListbox(3);

  if (listbox->count()>0){
    listbox->setCurrentItem( item );
    listbox->centerCurrentItem();
  }
}

void Fileman::WViewCloseClicked(int item)
{
  WView *window;
  window = windowList.at(item);
  
  //diconnect all old connections
  disconnect(window,SIGNAL(doUpdate(int)),
	     this,SLOT(updateListbox(int)));
  disconnect(window,SIGNAL(closeClicked(int)),
	     this,SLOT(WViewCloseClicked(int)));
  disconnect(imageWindow,SIGNAL(windowClicked(int)),
	      this,SLOT(comboboxSelected(int)));

  //special handling for the current output window
  if (window == imageWindow)
    {
      imageWindow = 0L;
    }

  // delete imagewindow
  delete window;

  //notify all other windows to change number
  for (window=windowList.first();
       window != 0;
       window=windowList.next())
    window->redrawCaption();

  //refill combobox
  updateCombobox();

  //choose first item in combobox, if there is 
  //no current output window
  if (imageWindow==0L)
    comboboxSelected(0);
}


void Fileman::createWView()
{
  if (imageWindow==0L)
    {
      imageWindow = new WView(0,0L,0);
      connect(imageWindow,SIGNAL(doUpdate(int)),
	      this,SLOT(updateListbox(int)));
      connect(imageWindow,SIGNAL(closeClicked(int)),
	      this,SLOT(WViewCloseClicked(int)));
      connect(imageWindow,SIGNAL(windowClicked(int)),
	      this,SLOT(comboboxSelected(int)));
    }
  updateCombobox();
}

void Fileman::slotDropEvent( KDNDDropZone * dropZone ){
	QStrList & urlList = dropZone->getURLList();
	QString filename;
		
	char *url;
	for ( url = urlList.first(); url != 0; url = urlList.next() ) {
	  filename = shortenFilename(QString(url));
	  appendFileList(filename);
	}
	updateListbox(3);
}


