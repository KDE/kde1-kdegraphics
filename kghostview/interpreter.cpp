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

#include <klocale.h>
#include <kapp.h>
#define i18n(X) klocale->translate(X)

InterpreterDialog::InterpreterDialog ( QWidget* parent, const char* name )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	setDefaultValues();
		
	generalBox = new QGroupBox( this, "generalBox" );
	generalBox->setGeometry( 5, 10, 430, 100 );
	generalBox->setFrameStyle( 49 );
	generalBox->setTitle( i18n("General") );
	generalBox->setAlignment( 1 );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this, "intLabel" );
	tmpQLabel->setGeometry( 20, 30, 100, 30 );
	tmpQLabel->setText( i18n("Interpreter name") );
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMargin( -1 );

	intEdit = new QLineEdit( this, "intEdit" );
	intEdit->setGeometry( 130, 30, 280, 30 );
	
	intEdit->setMaxLength( 50 );
	intEdit->setEchoMode( QLineEdit::Normal );
	intEdit->setFrame( TRUE );
	intEdit->setFocus( );

	antialiasBox = new QCheckBox( this, "antialiasBox" );
	antialiasBox->setGeometry( 30, 70, 100, 30 );
	antialiasBox->setText( i18n("Antialiasing") );
	antialiasBox->setFocusPolicy(QWidget::StrongFocus);
	
	fontsBox = new QCheckBox( this, "fontsBox" );
	fontsBox->setGeometry( 160, 70, 100, 30 );
	fontsBox->setText( i18n("Platform fonts") );
	fontsBox->setFocusPolicy(QWidget::StrongFocus);

	messageBox = new QCheckBox( this, "messageBox" );
	messageBox->setGeometry( 310, 70, 80, 30 );
	messageBox->setText( i18n("Messages") );
	messageBox->setFocusPolicy(QWidget::StrongFocus);

	paletteGroup = new QButtonGroup( this, "paletteGroup" );
	paletteGroup->setExclusive( TRUE );
	paletteGroup->setGeometry( 5, 120, 430, 60 );
	paletteGroup->setFrameStyle( 49 );
	paletteGroup->setTitle( i18n("Palette") );
	paletteGroup->setAlignment( 1 );

	colorButton = new QRadioButton( paletteGroup, "colorButton" );
	colorButton->setGeometry( 20, 20, 100, 30 );
	colorButton->setText( i18n("Color") );

	grayButton = new QRadioButton( paletteGroup, "grayButton" );
	grayButton->setGeometry( 150, 20, 100, 30 );
	grayButton->setText( i18n("Grayscale") );

	monoButton = new QRadioButton( paletteGroup, "monoButton" );
	monoButton->setGeometry( 300, 20, 100, 30 );
	monoButton->setText( i18n("Monochrome") );

	backingGroup = new QButtonGroup( this, "backGroup" );
	backingGroup->setGeometry( 5, 190, 430, 60 );
	backingGroup->setExclusive( TRUE );
	backingGroup->setFrameStyle( 49 );
	backingGroup->setTitle( i18n("Backing") );
	backingGroup->setAlignment( 1 );

	pixButton = new QRadioButton( backingGroup, "pixButton" );
	pixButton->setGeometry( 100, 20, 100, 30 );
	pixButton->setText( i18n("Pixmap") );

	storeButton = new QRadioButton( backingGroup, "storeButton" );
	storeButton->setGeometry( 220, 20, 100, 30 );
	storeButton->setText( i18n("Backing store") );

	okButton = new QPushButton( this, "okButton" );
	okButton->setGeometry( 225, 260, 60, 30 );
	okButton->setText( i18n("OK") );
	okButton->setAutoDefault( TRUE );
	connect( okButton, SIGNAL(clicked()), SLOT(setValues()) );

	cancelButton = new QPushButton( this, "cancelButton" );
	cancelButton->setGeometry( 295, 260, 60, 30 );
	cancelButton->setText( i18n("Cancel") );
	cancelButton->setAutoDefault( TRUE );
	connect( cancelButton, SIGNAL(clicked()), SLOT( reject() ) );

	defaultButton = new QPushButton( this, "defaultButton" );
	defaultButton->setGeometry( 365, 260, 60, 30 );
	defaultButton->setText( i18n("Defaults") );
	defaultButton->setAutoDefault( TRUE );
	connect( defaultButton, SIGNAL( clicked() ), SLOT( setDefaults() ) );

	resize( 440, 295 );
}

void InterpreterDialog::setDefaults()
{
	setDefaultValues();
	init();
}

void InterpreterDialog::setDefaultValues()
{
	antialias = TRUE;
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
