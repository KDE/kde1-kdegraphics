#include <qcolor.h>
#include <qtabdlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qcombo.h>
#include <stdio.h>
#include <kcolordlg.h>
#include "properties.h"

propertiesDialog::propertiesDialog(QWidget *parent, const char *name)
  : QTabDialog(parent, name)
{
  // Create the pages
  lineProps= new linePropertiesWidget(this);
  addTab(lineProps, "Line Properties");
  fillProps= new fillPropertiesWidget(this);
  addTab(fillProps, "Fill Properties");

  setCancelButton ();
  setApplyButton ();
}

QBrush *propertiesDialog::getBrush(void)
{
  return fillProps->getBrush();
}

QPen *propertiesDialog::getPen(void)
{
  return lineProps->getPen();
}


fillPropertiesWidget::fillPropertiesWidget(QWidget *parent)
  : QWidget(parent)
{
  // Set the page size
  setMinimumSize(320,320);
  setMaximumSize(320,320);

  // Set the initial colour
  fillColour= red; 
  //
  // Now create the contents
  //

  patternLabel= new QLabel("Fill Pattern:", this);
  patternLabel->move(40, 40);

  patternBox= new QComboBox(FALSE, this, 0);
  patternBox->resize(130, patternBox->height());
fprintf(stderr, "Height of pattern box is %d\n", patternBox->height());
  patternBox->insertItem("None");
  patternBox->insertItem("Solid");
  patternBox->insertItem("Diagonal Lines");
  //  patternBox->insertItem("Grid");
  //  patternBox->insertItem("Custom");
  patternBox->move(150, 40);

  customPatternLabel= new QLabel("Pattern Filename:", this);
  customPatternLabel->move(40, 100);

  customPatternField= new QLineEdit(this);
  customPatternField->move(150, 100);
  customPatternField->resize(130, customPatternField->height());

  browseButton= new QPushButton("Browse...", this);
  browseButton->move(150, 150);

  setFillColourButton= new QPushButton("Set Fill Colour...", this);
  setFillColourButton->move(40, 200);
  connect(setFillColourButton, SIGNAL(clicked()), this, SLOT(setFillColour()) );

  customPatternLabel->setEnabled(FALSE);
  customPatternField->setEnabled(FALSE);
  browseButton->setEnabled(FALSE);
}

void fillPropertiesWidget::setFillColour(void)
{
  QColor mycol;

 if (KColorDialog::getColor(mycol))
   fillColour= mycol;
}


QBrush *fillPropertiesWidget::getBrush(void)
{
  QBrush *b;

  switch (patternBox->currentItem()) {
  case 0:
    b= new QBrush(fillColour, NoBrush);
    break;
  case 1:
    b= new QBrush(fillColour, SolidPattern);
    break;
  case 2:
    b= new QBrush(fillColour, BDiagPattern);
    break;
  default:
    b= new QBrush(fillColour, BDiagPattern);
    break;
  }
  return b;
}

QPen *linePropertiesWidget::getPen()
{
  QPen *p;

  switch (styleBox->currentItem()) {
  case 0:
    p= new QPen(lineColour, widthBox->currentItem()+1, SolidLine);
    break;
  case 1:
    p= new QPen(lineColour, widthBox->currentItem()+1, DashLine);
    break;
  default:
    p= new QPen(lineColour, widthBox->currentItem()+1, DotLine);
    break;
  }
  return p;
}

linePropertiesWidget::linePropertiesWidget(QWidget *parent)
  : QWidget(parent)
{
  setMinimumSize(320,320);
  setMaximumSize(320,320);

  lineColour= blue;
   
  //
  // Now create the contents
  //

  styleLabel= new QLabel("Line Style:", this);
  styleLabel->move(40, 40);

  styleBox= new QComboBox(FALSE, this, 0);
  styleBox->insertItem("Solid");
  styleBox->insertItem("Dashed");
  styleBox->insertItem("Dotted");
  styleBox->insertItem("Dash Dot");
  styleBox->move(150, 40);

  
  widthLabel= new QLabel("Line Width:", this);
  widthLabel->move(40, 100);

  widthBox= new QComboBox(TRUE, this, 0);
  widthBox->insertItem("1");
  widthBox->insertItem("2");
  widthBox->insertItem("3");
  widthBox->insertItem("4");
  widthBox->move(150, 100);

  endTypeLabel= new QLabel("End style:", this);
  endTypeLabel->setEnabled(FALSE);
  endTypeLabel->move(40, 150);

  endTypeBox= new QComboBox(FALSE, this, 0);
  endTypeBox->insertItem("Not Yet");
  //  endTypeBox->insertItem("Rounded");
  //  endTypeBox->insertItem("Arrow");
  //  endTypeBox->insertItem("Flat");
  endTypeBox->setEnabled(FALSE);
  endTypeBox->move(150, 150);

  joinStyleLabel= new QLabel("Join style:", this);
  joinStyleLabel->setEnabled(FALSE);
  joinStyleLabel->move(40, 200);

  joinStyleBox= new QComboBox(FALSE, this, 0);
  joinStyleBox->setEnabled(FALSE);
  joinStyleBox->insertItem("Not Yet");
  //  joinStyleBox->insertItem("Rounded");
  //  joinStyleBox->insertItem("Straight");
  joinStyleBox->move(150, 200);


  setLineColourButton= new QPushButton("Set Line Colour...", this);
  setLineColourButton->move(40, 240);
  connect(setLineColourButton, SIGNAL(clicked()), this, SLOT(setLineColour()) );


}

void linePropertiesWidget::setLineColour(void)
{
  QColor mycol;

 if (KColorDialog::getColor(mycol))
   lineColour= mycol;
}

#include "metas/properties.moc"
