
/****************************************************************************
 **
 ** A dialog for choosing the size of a document view. 
 **
 ** Copyright (C) 1997 by Mark Donohoe.
 **
 ** This code is freely distributable under the GNU Public License.
 **
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>

#include "zoom.h"
#include "zoom.moc"
#include <qaccel.h>
#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>


//mag is an index
//rmag is a value ("real magnification")


Zoom::Zoom( QWidget *parent, const char *name )
      : QDialog( parent, name)
{
      setFocusPolicy(QWidget::StrongFocus);
      
      mag = 1;
      
      int border = 10;
      
      QBoxLayout *topLayout = new QVBoxLayout( this, border );
      
      topLayout->addStretch( 10 );

      sbMag = new KSpinBox( this );


      // (2.0) make this configurable if needed -- at least don't hard code!

      unsigned int i;
      mags = new int [21];
      for (i = 1; i <= 10; i++)
	mags [i] = (int) (100*i/(10));

      for (i = 1; i <= 10; i++)
	mags [i+10] = (int)(100+200*i/(10));

      sbMag->setValue( withPercent (mags[mag]) );
      sbMag->adjustSize();
      sbMag->setMinimumSize( sbMag->size() );
      connect ( sbMag, SIGNAL (valueIncreased()),
		SLOT (slotValueIncreased()) );
      connect ( sbMag, SIGNAL (valueDecreased()),
		SLOT (slotValueDecreased()) );

      QLabel* tmpQLabel;
      tmpQLabel = new QLabel( sbMag, i18n("&Zoom factor"), this );
      tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
      
      topLayout->addWidget( tmpQLabel );
      topLayout->addWidget( sbMag );
      
      QFrame* tmpQFrame;
      tmpQFrame = new QFrame( this );
      tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
      tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
      
      topLayout->addWidget( tmpQFrame );
      
      // CREATE BUTTONS
      
      KButtonBox *bbox = new KButtonBox( this );
      bbox->addStretch( 10 );
              
      apply = bbox->addButton( i18n("&OK") );
      connect( apply, SIGNAL(clicked()), SLOT(applyZoom()) );
      apply->setEnabled (false);

      QButton *close = bbox->addButton( i18n("&Close") );
      connect( close, SIGNAL(clicked()), SLOT(reject()) );
              
      bbox->layout();
      topLayout->addWidget( bbox );

      topLayout->activate();
      
      resize( 200,0 );
}

void
Zoom::applyZoom()
{
      mag = reverseArray (atoi (sbMag->getValue()));
      emit applyChange();
}


unsigned int
Zoom::reverseArray (int magnification)
{
  int i;
  for (i=1;i<21;i++)
    if (mags [i] == magnification)
      return i;


  return 0; //not in array!
  
}

const char *
Zoom::withPercent (int rmag)
{
  wp.setNum (rmag);
  wp+="%";

  return (const char *)wp;
}


#define MAGMAX 20

void
Zoom::updateZoom (int newmag)
{
  if ( mag >=1 && mag <= MAGMAX)
    {
      mag=newmag; 
      sbMag->setValue (withPercent (mags[mag]));
      apply->setEnabled (false);
    }
}

void
Zoom::slotValueIncreased ()
{

  if (mag < MAGMAX)
    {
      mag++;
      sbMag->setValue (withPercent (mags[mag]));
      apply->setEnabled (true);
    }
}

void
Zoom::slotValueDecreased ()
{

  if (mag > 1)
    {
      mag--;
      sbMag->setValue (withPercent (mags[mag]));
      apply->setEnabled (true);
   }
}
