#include <qcolor.h>
#include <qtabdlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qcombo.h>
#include <stdio.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kapp.h>
#include "properties.h"

propertiesDialog::propertiesDialog(int pages,
				   QWidget *parent= 0, const char *name= 0)
  : QTabDialog(parent, name)
{
  // Create the pages
  lineProps= new linePropertiesWidget(this);
  addTab(lineProps, klocale->translate("Line Properties"));
  fillProps= new fillPropertiesWidget(this);
  addTab(fillProps, klocale->translate("Fill Properties"));

  setCancelButton();
  setApplyButton();
  setPages(pages);

}

QBrush *propertiesDialog::getBrush(void)
{
  return fillProps->getBrush();
}

QPen *propertiesDialog::getPen(void)
{
  return lineProps->getPen();
}

void propertiesDialog::setPages(int pages)
{
  // Adjust tabs to suit new tool
  if (pages & 1)
    lineProps->setEnabled(TRUE);
  else
    lineProps->setEnabled(FALSE);

  if (pages & 2)
    fillProps->setEnabled(TRUE);
  else
    fillProps->setEnabled(FALSE);
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

  patternLabel= new QLabel(klocale->translate("Fill Pattern:"), this);
  patternLabel->move(40, 40);

  // Need to use an item that can display a sample of each one
  patternBox= new QComboBox(FALSE, this, 0);
  patternBox->resize(130, patternBox->height());
  patternBox->insertItem(klocale->translate("None"));
  patternBox->insertItem(klocale->translate("Solid"));
  patternBox->insertItem(klocale->translate("Vertical Lines"));
  patternBox->insertItem(klocale->translate("Horizontal Lines"));
  patternBox->insertItem(klocale->translate("Grid"));
  patternBox->insertItem(klocale->translate("Diagonal Lines //"));
  patternBox->insertItem(klocale->translate("Diagonal Lines \\"));
  patternBox->insertItem(klocale->translate("Diagonal Grid"));

  patternBox->insertItem("94%");
  patternBox->insertItem("88%");
  patternBox->insertItem("63%");
  patternBox->insertItem("50%");
  patternBox->insertItem("37%");
  patternBox->insertItem("12%");
  patternBox->insertItem("6%");

  patternBox->insertItem(klocale->translate("Custom"));
  patternBox->move(150, 40);

  customPatternLabel= new QLabel(klocale->translate("Pattern Filename:"), this);
  customPatternLabel->move(40, 100);

  customPatternField= new QLineEdit(this);
  customPatternField->move(150, 100);
  customPatternField->resize(130, customPatternField->height());

  browseButton= new QPushButton(klocale->translate("Browse..."), this);
  browseButton->move(150, 150);

  setFillColourButton= new QPushButton(klocale->translate("Set Fill Colour..."), this);
  setFillColourButton->move(40, 200);

     pattern= new QPixmap();

   connect(setFillColourButton, SIGNAL(clicked()), this, SLOT(setFillColour()) );
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
    b= new QBrush(fillColour, VerPattern);
    break;
  case 3:
    b= new QBrush(fillColour, HorPattern);
    break;
  case 4:
    b= new QBrush(fillColour, CrossPattern);
    break;
  case 5:
    b= new QBrush(fillColour, BDiagPattern);
    break;
  case 6:
    b= new QBrush(fillColour, FDiagPattern);
    break;
  case 7:
    b= new QBrush(fillColour, DiagCrossPattern);
    break;

    // Greys
  case 8:
    b= new QBrush(fillColour, Dense1Pattern);
    break;
  case 9:
    b= new QBrush(fillColour, Dense2Pattern);
    break;
  case 10:
    b= new QBrush(fillColour, Dense3Pattern);
    break;
  case 11:
    b= new QBrush(fillColour, Dense4Pattern);
    break;
  case 12:
    b= new QBrush(fillColour, Dense5Pattern);
    break;
  case 13:
    b= new QBrush(fillColour, Dense6Pattern);
    break;
  case 14:
    b= new QBrush(fillColour, Dense7Pattern);
    break;

    // Custom Pattern 
  case 15:
    b= new QBrush(fillColour);
    pattern->load(customPatternField->text());
    b->setPixmap(*pattern); 
    break;

  default:
    b= new QBrush(fillColour, Dense7Pattern);
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
  case 2:
    p= new QPen(lineColour, widthBox->currentItem()+1, DotLine);
    break;
  case 3:
    p= new QPen(lineColour, widthBox->currentItem()+1, DashDotLine);
    break;
  case 4:
    p= new QPen(lineColour, widthBox->currentItem()+1, DashDotDotLine);
    break;
  case 5:
    p= new QPen(lineColour, widthBox->currentItem()+1, NoPen);
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

  styleLabel= new QLabel(klocale->translate("Line Style:"), this);
  styleLabel->move(40, 40);

  // These should have a picture
  styleBox= new QComboBox(FALSE, this, 0);
  styleBox->insertItem(klocale->translate("Solid"));
  styleBox->insertItem(klocale->translate("Dashed"));
  styleBox->insertItem(klocale->translate("Dotted"));
  styleBox->insertItem(klocale->translate("Dash Dot"));
  styleBox->insertItem(klocale->translate("Dash Dot Dot"));
  styleBox->insertItem(klocale->translate("None"));
  styleBox->move(150, 40);

  
  widthLabel= new QLabel(klocale->translate("Line Width:"), this);
  widthLabel->move(40, 100);

  widthBox= new QComboBox(TRUE, this, 0);
  widthBox->insertItem("1");
  widthBox->insertItem("2");
  widthBox->insertItem("3");
  widthBox->insertItem("4");
  widthBox->move(150, 100);

  endTypeLabel= new QLabel(klocale->translate("End style:"), this);
  endTypeLabel->setEnabled(FALSE);
  endTypeLabel->move(40, 150);

  endTypeBox= new QComboBox(FALSE, this, 0);
  endTypeBox->insertItem(klocale->translate("Not Yet"));
  //  endTypeBox->insertItem(klocale->translate("Rounded"));
  //  endTypeBox->insertItem(klocale->translate("Arrow"));
  //  endTypeBox->insertItem(klocale->translate("Flat"));
  endTypeBox->setEnabled(FALSE);
  endTypeBox->move(150, 150);

  joinStyleLabel= new QLabel(klocale->translate("Join style:"), this);
  joinStyleLabel->setEnabled(FALSE);
  joinStyleLabel->move(40, 200);

  joinStyleBox= new QComboBox(FALSE, this, 0);
  joinStyleBox->setEnabled(FALSE);
  joinStyleBox->insertItem(klocale->translate("Not Yet"));
  //  joinStyleBox->insertItem(klocale->translate("Rounded"));
  //  joinStyleBox->insertItem(klocale->translate("Straight"));
  joinStyleBox->move(150, 200);


  setLineColourButton= new QPushButton(klocale->translate("Set Line Colour..."), this);
  setLineColourButton->move(40, 240);
  connect(setLineColourButton, SIGNAL(clicked()), this, SLOT(setLineColour()) );


}

void linePropertiesWidget::setLineColour(void)
{
  QColor mycol;

 if (KColorDialog::getColor(mycol))
   lineColour= mycol;
}

#include "properties.moc"
