
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
#include <kbuttonbox.h>

Zoom::Zoom( int *m, QWidget *parent, const char *name )
      : QDialog( parent, name, TRUE )
{
      setFocusPolicy(QWidget::StrongFocus);
      
      mag = m;
      
      int border = 10;
      
      QBoxLayout *topLayout = new QVBoxLayout( this, border );
      
      topLayout->addStretch( 10 );

      sbMag = new KNumericSpinBox( this );
      
      sbMag->setStep( 10 );
      sbMag->setRange( 10, 300 );
      sbMag->setValue( *mag );
      sbMag->adjustSize();
      sbMag->setMinimumSize( sbMag->size() );
      
      QLabel* tmpQLabel;
      tmpQLabel = new QLabel( sbMag, i18n("&Magnification"), this );
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
              
      QButton *ok = bbox->addButton( i18n("&OK") );
      connect( ok, SIGNAL(clicked()), SLOT(acceptMag()) );
      
      QButton *cancel = bbox->addButton( i18n("&Cancel") );
      connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
              
      bbox->layout();
      topLayout->addWidget( bbox );

      topLayout->activate();
      
      resize( 200,0 );
}

void Zoom::acceptMag()
{
      *mag = sbMag->getValue();
      accept();
}

