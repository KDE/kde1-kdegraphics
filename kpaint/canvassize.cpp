// $Id$

#include <stdlib.h>
#include "canvassize.h"
#include <klocale.h>
#include <kapp.h>

canvasSizeDialog::canvasSizeDialog(QWidget *parent= 0, const char *name= 0)
 : QDialog (parent, name, TRUE)
{
   widthLabel= new QLabel(klocale->translate("Width"), this);
   heightLabel= new QLabel(klocale->translate("Height"), this);
   widthEdit= new QLineEdit(this);
   heightEdit= new QLineEdit(this);
   okButton= new QPushButton(klocale->translate("Ok"), this);
   cancelButton= new QPushButton(klocale->translate("Cancel"), this);
 
   connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
   connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );
   
   resize(240,150);
   widthLabel->move(20,20);
   widthEdit->move(60,20);
   widthEdit->resize(150, widthEdit->height());
   heightLabel->move(20,70);
   heightEdit->move(60,70);
   heightEdit->resize(150, heightEdit->height());
   okButton->move(20, 120);
   cancelButton->move(130, 120);
}

int canvasSizeDialog::getHeight(void)
{
   return atoi(heightEdit->text());
}
   
int canvasSizeDialog::getWidth(void)
{
   return atoi(widthEdit->text());
}



