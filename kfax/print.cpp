/*
    $Id$

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1996 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
    $Log$
    Revision 1.4  1997/06/14 06:03:00  wuebben
    *** empty log message ***

    Revision 1.3  1997/06/13 05:46:54  wuebben
    *** empty log message ***

    Revision 1.2  1997/06/12 22:11:17  wuebben
    *** empty log message ***

    Revision 1.1  1997/06/12 16:29:19  wuebben
    Initial revision




*/


#include "print.h"
#include "stdio.h"

#define YOFFSET  5
#define XOFFSET  5
#define LABLE_LENGTH  40
#define LABLE_HEIGHT 20
#define SIZE_X 400
#define SIZE_Y 280
#define FONTLABLE_LENGTH 60
#define COMBO_BOX_HEIGHT 28
#define COMBO_ADJUST 3
#define OKBUTTONY 260
#define BUTTONHEIGHT 25

#include "print.moc"


PrintDialog::PrintDialog( QWidget *parent, const char *name,  bool )
    : QDialog( parent, name,FALSE)
{

  setCaption("Print Dialog");

  this->setFocusPolicy(QWidget::StrongFocus);
  
  bg = new QButtonGroup(this,"bg");
  


  lprbutton = new QRadioButton("Print as Postscript to Printer",bg,"rawbutton");
  lprbutton->setGeometry(15,20,250,25);
  lprbutton->setChecked(TRUE);
  connect(lprbutton,SIGNAL(clicked()),this,SLOT(filebuttontoggled()));
  
  cmdlabel = new QLabel ("Print Command:",this);
  cmdlabel->setGeometry(45,60,90,BUTTONHEIGHT);
  cmdlabel->setEnabled(TRUE);

  cmdedit = new QLineEdit(bg,"lprcmd");
  cmdedit->setGeometry(140,50,170,25);
  cmdedit->setEnabled(TRUE);
  cmdedit->setText("lpr");

  filebutton = new QRadioButton("Print as Postscript to File:",bg,"commandbutton");
  filebutton->setGeometry(15,90,250,25);
  connect(filebutton,SIGNAL(clicked()),this,SLOT(filebuttontoggled()));


  selectfile = new QPushButton ("Browse",this);
  selectfile->setGeometry(25,140,60,BUTTONHEIGHT);
  selectfile->setEnabled(FALSE);
  connect(selectfile,SIGNAL(clicked()),this,SLOT(choosefile()));

  filename = new QLineEdit(bg,"command");
  filename->setGeometry(85,130,225,25);
  filename->setEnabled(FALSE);  

  bg->setGeometry(10,10,330,180);
  
  cancel_button = new QPushButton("Cancel",this);

  cancel_button->setGeometry( 3*XOFFSET +100, 200, 80, BUTTONHEIGHT );
  connect( cancel_button, SIGNAL( clicked() ), SLOT( cancel() ) );

  ok_button = new QPushButton( "Ok", this );
  ok_button->setGeometry( 3*XOFFSET, 200, 80, BUTTONHEIGHT );
  connect( ok_button, SIGNAL( clicked() ), SLOT( ready() ) );	


  this->setFixedSize(350,230);


}


struct printinfo *  PrintDialog::getInfo(){



  pi.file  = filename->text();
  pi.cmd = cmdedit->text();
  if(lprbutton->isChecked())
    pi.lpr = 1;
  else
    pi.lpr = 0;
 
  
  return &pi;

}

void PrintDialog::setWidgets(struct printinfo* pi ){


  if (pi->lpr == 1){
    lprbutton->setChecked(TRUE);
    filebutton->setChecked(FALSE);
  }
  else{
    lprbutton->setChecked(TRUE);
    filebutton->setChecked(FALSE);
  }
  
  filename->setText(pi->file);

}

void PrintDialog::choosefile(){



  QFileDialog* box = new QFileDialog(this,"printfiledialog",TRUE);
  box->setCaption("Select Filename");

  if (!box->exec()) {
    return;
  }

  if(box->selectedFile().isEmpty()) {  

      return ;
  }

  filename->setText( box->selectedFile());

  delete box;

}

void PrintDialog::ready(){

  if(filebutton->isChecked() && QString(filename->text()).isEmpty()){

    QMessageBox::message("Sorry","You must enter a file name if you wish to "\
			 "print to a file.","OK");
    return;


  }

  if(lprbutton->isChecked() && QString(cmdedit->text()).isEmpty()){

    QMessageBox::message("Sorry","You must enter a print command such as \"lpr\"\n "\
			 "if you wish to "\
			 "print to a printer.","OK");
    return;


  }

  this->hide();
  emit print();


}

void PrintDialog::cancel(){

  this->hide();

}


void PrintDialog::filebuttontoggled(){

  if(filebutton->isChecked()){
    selectfile->setEnabled(TRUE); 
    filename->setEnabled(TRUE);
    cmdedit->setEnabled(FALSE);
    cmdlabel->setEnabled(FALSE);
  }
  else{
    selectfile->setEnabled(FALSE); 
    filename->setEnabled(FALSE);
    cmdedit->setEnabled(TRUE);
    cmdlabel->setEnabled(TRUE);
  }

}

void PrintDialog::focusInEvent(QFocusEvent* ){

  lprbutton->setFocus();

}
