//////////////////////////
//
// fileman.cpp -- The KView Display Manager Widget
//
// Martin Hartig


#include <qevent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <qstring.h>
#include <qpixmap.h>

#include <kurl.h>

#include "fileman.h"
#include "wview.h"
#include "fileman.moc"

#include "version.h"

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
  setView(mainwidget, FALSE);

  // fill the listbox
  updateListbox(3);
  
  // show display manager
  resize(260,350);
  show();
  resize(260,350);

  // other stuff
  showrunning = FALSE;
  fileList.setAutoDelete( TRUE );
  timerDelay = 5*100;

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
  file->insertItem("Close Window", this, SLOT(closeWindow()));

  help = new QPopupMenu ();
  help->insertItem("About KView",  this, SLOT(aboutKview()));
  help->insertItem("Help on KView",this, SLOT(invokeHelp()));

  
  menubar = new KMenuBar(this,"menubar");
  menubar->insertItem("File", file);
  menubar->insertItem("Help", help);
  setMenu(menubar);
}

void Fileman::initToolBar()
{
  ktoolbar1 = new KToolBar( this );
  ktoolbar2 = new KToolBar( this );
  
  QString pixdir = KApplication::kdedir() + QString("/lib/pics/toolbar/");
  QPixmap pixmap;

  //------------- initialize the first toolbar
  pixmap.load(pixdir+"filenew.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_NEWWINDOW,
			SIGNAL(clicked()), this,
			SLOT(slotNew()), TRUE,
			"New Window");

  pixmap.load(pixdir+"fileopen.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_OPENFILE,
			SIGNAL(clicked()), this,
			SLOT(slotOpen()), TRUE,
			"open new file");
  
  pixmap.load(pixdir+"reload.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_RELOAD,
			SIGNAL(clicked()), this,
			SLOT(slotReload()), TRUE,
			"reload file list");
  ktoolbar1->insertSeparator();
  
  pixmap.load(pixdir+"viewmag-.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_MINIFY,
			SIGNAL(clicked()), this,
			SLOT(slotMin()), TRUE,
			"shrink -50%");

  pixmap.load(pixdir+"viewmag+.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_MAGNIFY,
			SIGNAL(clicked()), this,
			SLOT(slotMax()), TRUE,
			"magnify +50%");
  
  pixmap.load(pixdir+"resize.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_RESIZE,
			SIGNAL(clicked()), this,
			SLOT(slotResize()), TRUE,
			"size Window");
  ktoolbar1->insertSeparator();

  pixmap.load(pixdir+"help.xpm");
  ktoolbar1->insertItem(pixmap, ID_T_HELP,
			SIGNAL(clicked()), this,
			SLOT(invokeHelp()), TRUE,
			"launch help");

  //--------------- initialize the second toolbar
  pixmap.load(pixdir+"start.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_FIRSTPIC,
			SIGNAL(clicked()), this,
			SLOT(firstClicked()), TRUE,
			"first pic");

  pixmap.load(pixdir+"finish.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_LASTPIC,
			SIGNAL(clicked()), this,
			SLOT(lastClicked()), TRUE,
			"last pic");
  ktoolbar2->insertSeparator();

  pixmap.load(pixdir+"back.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_PREVPIC,
			SIGNAL(clicked()), this,
			SLOT(prevClicked()), TRUE,
			"prev pic");

  pixmap.load(pixdir+"forward.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_NEXTPIC,
			SIGNAL(clicked()), this,
			SLOT(nextClicked()), TRUE,
			"next pic");
  ktoolbar2->insertSeparator();
    
  pixmap.load(pixdir+"diashow.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_STARTSHOW,
			SIGNAL(clicked()), this,
			SLOT(startShow()), TRUE,
			"start picture show");

  pixmap.load(pixdir+"random.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_RANDOMSHOW,
			SIGNAL(clicked()), this,
			SLOT(randomShow()), TRUE,
			"stop picture show");
  
  pixmap.load(pixdir+"stop.xpm");
  ktoolbar2->insertItem(pixmap, ID_T_STOPSHOW,
			SIGNAL(clicked()), this,
			SLOT(stopShow()), TRUE,
			"stop picture show");

  //add the toolbars to the top level widget
  //default positions
  ktoolbar1->setBarPos( KToolBar::Top );
  ktoolbar2->setBarPos( KToolBar::Top );

  //the order of the 4 folowing lines is decisioning
  iktoolbar1 = addToolBar(ktoolbar1);
  iktoolbar2 = addToolBar(ktoolbar2);

  //do a repositioning of the mainwidget after the toolbars have
  //been moved. Note that a repositioning of the op level widget
  //is been done before
  connect( ktoolbar1, SIGNAL( moved(Position)), this, SLOT( doPositioning()));
  connect( ktoolbar2, SIGNAL( moved(Position)), this, SLOT( doPositioning())); 
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

  
  slider = new QSlider(50,10,5,10,QSlider::Horizontal, mainwidget);
  slider->setRange(10,50);
  slider->setSteps(5,10);
  connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));


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

  listbox->setGeometry(10,
		       10,
		       mainwidget->width()-30,
		       textSlow->y()-25);
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
  Fileman *man = new Fileman(0L,0L);
  man->show();
  man->firstClicked();
}

void Fileman::slotOpen()
{
  if (imageWindow==0L) createWView();
  imageWindow->loadImage();
  if (!imageWindow==0L) imageWindow->show();
  checkValidButtons();
}

void Fileman::slotOpenUrl()
{
  if (imageWindow==0L) createWView();
  imageWindow->loadURL();
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
  text += "(c) 1996, 1997 by \n";
  text += "Sirtaj Singh Kang  taj@kde.org\n";
  text += "Martin Hartig  hartig@mathematik.uni-kl.de";
  QMessageBox::message("About kview", text.data(),
			"Ok"); 
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
  srand((unsigned) time((long *)0));
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
  if(manCount > 1)
    delete this;
  else
    qApp->quit();
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

void Fileman::WViewCloseClicked(int)
{
  //kview 0.12.0  WView *imagewin;
  //              printf("REC %d\n",item);
  //              imagewin = windowList.at(item);
  disconnect(imageWindow,SIGNAL(doUpdate(int)),
	     this,SLOT(updateListbox(int)));
  disconnect(imageWindow,SIGNAL(closeClicked(int)),
	     this,SLOT(WViewCloseClicked(int)));
  //              delete imagewin;
  delete imageWindow;
  imageWindow = 0L;
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
    }
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


