/****************************************************************************
**
** A dialog for configuring the Ghostscript interpreter.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include "interpreter.h"
#include "interpreter.moc"

#include <qbutton.h>
#include <qlabel.h>
#include <qapp.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

InterpreterDialog::InterpreterDialog ( QWidget* parent, const char* name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	setDefaultValues();
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 5 );
	
	topLayout->addSpacing( 10 );
	
	// GENERAL
	
	QBoxLayout *paddingLayout = new QHBoxLayout();
	topLayout->addLayout( paddingLayout, 10 );
		
	generalBox = new QGroupBox( this, "generalBox" );
	//generalBox->setGeometry( 5, 10, 430, 100 );
	generalBox->setFrameStyle( 49 );
	generalBox->setTitle( i18n("General") );
	generalBox->setAlignment( 1 );
	
	paddingLayout->addSpacing( 0 );
	paddingLayout->addWidget( generalBox, 10 );
	paddingLayout->addSpacing( 0 );
	
	topLayout->addSpacing( 10 );
	
	QGridLayout *grid = new QGridLayout( generalBox, 4, 5, 10 );
	
	grid->setRowStretch(0,10);
	grid->addRowSpacing(0,10);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);
	grid->setRowStretch(3,10);

	grid->setColStretch(0,10);
	grid->addColSpacing(0,10);
	grid->setColStretch(1,10);
	grid->setColStretch(2,10);
	grid->setColStretch(3,10);
	grid->setColStretch(4,10);

	int checkWidth = 0;

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this, "intLabel" );
	//tmpQLabel->setGeometry( 20, 30, 100, 30 );
	tmpQLabel->setText( i18n("Interpreter name") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMargin( -1 );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 1, 1 );

	intEdit = new QLineEdit( this, "intEdit" );
	
	intEdit->setMaxLength( 50 );
	intEdit->setEchoMode( QLineEdit::Normal );
	intEdit->setFrame( TRUE );
	intEdit->setFocus( );
	intEdit->setFixedHeight( intEdit->sizeHint().height() );
	
	grid->addMultiCellWidget( intEdit, 1, 1, 2, 3 );

	antialiasBox = new QCheckBox( this, "antialiasBox" );
	//antialiasBox->setGeometry( 30, 70, 100, 30 );
	antialiasBox->setText( i18n("Antialiasing") );
	antialiasBox->setFocusPolicy(QWidget::StrongFocus);
	if ( antialiasBox->sizeHint().width() > checkWidth )
		checkWidth = antialiasBox->sizeHint().width();
	antialiasBox->setMinimumWidth( checkWidth );
	
	grid->addWidget( antialiasBox, 3, 1 );
	
	fontsBox = new QCheckBox( this, "fontsBox" );
	//fontsBox->setGeometry( 160, 70, 100, 30 );
	fontsBox->setText( i18n("Platform fonts") );
	fontsBox->setFocusPolicy(QWidget::StrongFocus);
	if ( fontsBox->sizeHint().width() > checkWidth )
		checkWidth = fontsBox->sizeHint().width();
	fontsBox->setMinimumWidth( checkWidth );
	
	grid->addWidget( fontsBox, 3, 2 );

	messageBox = new QCheckBox( this, "messageBox" );
	//messageBox->setGeometry( 310, 70, 80, 30 );
	messageBox->setText( i18n("Messages") );
	messageBox->setFocusPolicy(QWidget::StrongFocus);
	if ( messageBox->sizeHint().width() > checkWidth )
		checkWidth = messageBox->sizeHint().width();
	messageBox->setMinimumWidth( checkWidth );
	messageBox->setMinimumHeight( messageBox->sizeHint().height() );
	
	grid->addWidget( messageBox, 3, 3 );
	
	generalBox->setMinimumHeight( 4*(fontsBox->sizeHint().height()+10) );
	generalBox->setMinimumWidth( 30 + 3*(checkWidth+10) );
	
	topLayout->addStretch( 10 );
	
	// PALETTE
	
	paddingLayout = new QHBoxLayout();
	topLayout->addLayout( paddingLayout, 10 );

	paletteGroup = new QButtonGroup( this, "paletteGroup" );
	paletteGroup->setExclusive( TRUE );
	paletteGroup->setGeometry( 5, 120, 430, 60 );
	paletteGroup->setFrameStyle( 49 );
	paletteGroup->setTitle( i18n("Palette") );
	paletteGroup->setAlignment( 1 );
	
	paddingLayout->addSpacing( 0 );
	paddingLayout->addWidget( paletteGroup, 10 );
	paddingLayout->addSpacing( 0 );
	
	topLayout->addSpacing( 10 );
	
	grid = new QGridLayout( paletteGroup, 3, 5, 5 );
	
	grid->setRowStretch(0,10);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);

	grid->setColStretch(0,10);
	grid->setColStretch(1,10);
	grid->setColStretch(2,10);
	grid->setColStretch(3,10);
	grid->setColStretch(4,10);

	colorButton = new QRadioButton( paletteGroup, "colorButton" );
	//colorButton->setGeometry( 20, 20, 100, 30 );
	colorButton->setText( i18n("Color") );
	if ( colorButton->sizeHint().width() > checkWidth )
		checkWidth = colorButton->sizeHint().width();
	colorButton->setMinimumWidth( checkWidth );
	
	grid->addWidget( colorButton, 1, 1 );

	grayButton = new QRadioButton( paletteGroup, "grayButton" );
	//grayButton->setGeometry( 150, 20, 100, 30 );
	grayButton->setText( i18n("Grayscale") );
	if ( grayButton->sizeHint().width() > checkWidth )
		checkWidth = grayButton->sizeHint().width();
	grayButton->setMinimumWidth( checkWidth );
	
	grid->addWidget( grayButton, 1, 2 );

	monoButton = new QRadioButton( paletteGroup, "monoButton" );
	//monoButton->setGeometry( 300, 20, 100, 30 );
	monoButton->setText( i18n("Monochrome") );
	if ( monoButton->sizeHint().width() > checkWidth )
		checkWidth = monoButton->sizeHint().width();
	monoButton->setMinimumWidth( checkWidth );
	monoButton->setMinimumHeight( monoButton->sizeHint().height() );
	
	grid->addWidget( monoButton, 1, 3 );
	
	paletteGroup->setMinimumHeight( 3*(colorButton->sizeHint().height()+5) );
	paletteGroup->setMinimumWidth( 30 + 3*(checkWidth+10) );
	
	topLayout->addStretch( 10 );
	
	//
	// BACKING
	//

	paddingLayout = new QHBoxLayout();
	topLayout->addLayout( paddingLayout, 10 );
	
	backingGroup = new QButtonGroup( this, "backGroup" );
	backingGroup->setGeometry( 5, 190, 430, 60 );
	backingGroup->setExclusive( TRUE );
	backingGroup->setFrameStyle( 49 );
	backingGroup->setTitle( i18n("Backing") );
	backingGroup->setAlignment( 1 );
	
	paddingLayout->addSpacing( 0 );
	paddingLayout->addWidget( backingGroup, 10 );
	paddingLayout->addSpacing( 0 );
	
	topLayout->addSpacing( 10 );
	
	grid = new QGridLayout( backingGroup, 3, 4, 5 );
	
	grid->setRowStretch(0,10);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);

	grid->setColStretch(0,10);
	grid->setColStretch(1,10);
	grid->setColStretch(2,10);
	grid->setColStretch(3,10);


	pixButton = new QRadioButton( backingGroup, "pixButton" );
	//pixButton->setGeometry( 100, 20, 100, 30 );
	pixButton->setText( i18n("Pixmap") );
	if ( pixButton->sizeHint().width() > checkWidth )
		checkWidth = pixButton->sizeHint().width();
	pixButton->setMinimumWidth( checkWidth );
	pixButton->setMinimumHeight( pixButton->sizeHint().height() );
	pixButton->setMaximumSize( 500, 500 );
	
	grid->addWidget( pixButton, 1, 1, AlignHCenter );

	storeButton = new QRadioButton( backingGroup, "storeButton" );
	//storeButton->setGeometry( 220, 20, 100, 30 );
	storeButton->setText( i18n("Backing store") );
	if ( storeButton->sizeHint().width() > checkWidth )
		checkWidth = storeButton->sizeHint().width();
	storeButton->setMinimumWidth( checkWidth );
	storeButton->setMaximumSize( 500, 500 );
	
	grid->addWidget( storeButton, 1, 2, AlignHCenter );
	
	backingGroup->setMinimumHeight( 3*(pixButton->sizeHint().height()+5) );
	backingGroup->setMinimumWidth(
		20 + 2*checkWidth );
	
	topLayout->addStretch( 10 );
	
	//
	// BUTTONS
	//
	
	KButtonBox *bbox = new KButtonBox( this );
	
	defaultButton = bbox->addButton( i18n("Defaults") );
	//defaultButton = new QPushButton( this, "defaultButton" );
	//defaultButton->setGeometry( 365, 260, 60, 30 );
	//defaultButton->setText( i18n("Defaults") );
	//defaultButton->setAutoDefault( TRUE );
	connect( defaultButton, SIGNAL( clicked() ), SLOT( setDefaults() ) );
	
	bbox->addStretch( 10 );

	okButton = bbox->addButton( i18n("OK") );
	//okButton = new QPushButton( this, "okButton" );
	//okButton->setGeometry( 225, 260, 60, 30 );
	//okButton->setText( i18n("OK") );
	//okButton->setAutoDefault( TRUE );
	connect( okButton, SIGNAL(clicked()), SLOT(setValues()) );
	
	cancelButton = bbox->addButton( i18n("Cancel") );
	//cancelButton = new QPushButton( this, "cancelButton" );
	//cancelButton->setGeometry( 295, 260, 60, 30 );
	//cancelButton->setText( i18n("Cancel") );
	//cancelButton->setAutoDefault( TRUE );
	connect( cancelButton, SIGNAL(clicked()), SLOT( reject() ) );
	
	bbox->layout();
	topLayout->addWidget( bbox );

	//resize( 440, 295 );

	topLayout->activate();
	
	resize(0,0);
}


void InterpreterDialog::setDefaults()
{
	setDefaultValues();
	init();
}

void InterpreterDialog::setDefaultValues()
{
	antialias = FALSE;
	show_messages = TRUE;
	platform_fonts = FALSE;
	paletteOpt = COLOR_PALETTE;
	backingOpt = PIX_BACKING;
}

void InterpreterDialog::setValues()
{
	if( antialiasBox->isChecked() )
		antialias = TRUE;
	else
		antialias = FALSE;
		
	if( fontsBox->isChecked() )
		platform_fonts = TRUE;
	else
		platform_fonts = FALSE;
		
	if( messageBox->isChecked() )
		show_messages = TRUE;
	else
		show_messages = FALSE;
		
	if( colorButton->isChecked() )
		paletteOpt = COLOR_PALETTE;
	else if( grayButton->isChecked() )
		paletteOpt = GRAY_PALETTE;
	else
		paletteOpt = MONO_PALETTE;
		
	if( pixButton->isChecked() )
		backingOpt = PIX_BACKING;
	else
		backingOpt = STORE_BACKING;
	
	accept();
}

void InterpreterDialog::init()
{
	
	intEdit->setText( i18n("gs") );
	
	if( antialias )
		antialiasBox->setChecked( TRUE );
	else
		antialiasBox->setChecked( FALSE );
		
	if( show_messages )
		messageBox->setChecked( TRUE );
	else
		messageBox->setChecked( FALSE );
		
	if( platform_fonts )
		fontsBox->setChecked( TRUE );
	else
		fontsBox->setChecked( FALSE );
	
	colorButton->setChecked( FALSE );
	grayButton->setChecked( FALSE );
	monoButton->setChecked( FALSE );				
	
	switch( paletteOpt ) {
		case COLOR_PALETTE:
			colorButton->setChecked( TRUE );
			break;
		case GRAY_PALETTE:
			grayButton->setChecked( TRUE );
			break;
		case MONO_PALETTE:
			monoButton->setChecked( TRUE );
			break;
	}
	
	pixButton->setChecked( FALSE );
	storeButton->setChecked( FALSE );
	
	switch( backingOpt ) {
		case PIX_BACKING:
			pixButton->setChecked( TRUE );
			break;
		case STORE_BACKING:
			storeButton->setChecked( TRUE );
			break;
	}
	
	okButton->setFocus();
}




InterpreterDialog::~InterpreterDialog()
{
}
