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
  


*/

#include "kfax.h"
#include "options.h"


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

#include "options.moc"

extern MyApp *mykapp;

OptionsDialog::OptionsDialog( QWidget *parent, const char *name)
    : QDialog( parent, name,TRUE)
{

  setCaption(i18n("KFax Options Dialog"));

  this->setFocusPolicy(QWidget::StrongFocus);
  
  bg = new QGroupBox(this,"bg");
  bg->setGeometry(10,10,400,220);  

  resgroup = new QButtonGroup(this,"resgroup");
  resgroup->setGeometry(20,60,370,30);
  resgroup->setFrameStyle(QFrame::NoFrame);
  reslabel = new QLabel(i18n("Raw Fax Resolution:"),resgroup,"relabel");
  reslabel->move(5,10);
  reslabel->adjustSize();

  resauto = new QRadioButton(i18n("Auto"),resgroup,"resauto");
  resauto->move(130,2);

  fine = new QRadioButton(i18n("Fine"),resgroup,"fine");
  fine->move(215,2);

  normal = new QRadioButton(i18n("Normal"),resgroup,"normal");
  normal->move(290,2);

  /*  displaygroup = new QButtonGroup(this,"displaygroup");
  displaygroup->setGeometry(20,60,370,30);
  displaygroup->setFrameStyle(QFrame::NoFrame);*/

  displaylabel = new QLabel(i18n("Display Options:"),this,"displaylabel");
  displaylabel->move(25,30);
  displaylabel->adjustSize();

  landscape = new QCheckBox(i18n("Landscape"),this,"Landscape");
  landscape->move(150,30);
  landscape->adjustSize();

  flip = new QCheckBox(i18n("Upside Down"),this,"upsidedown");
  flip->move(238,30);
  flip->adjustSize();

  invert = new QCheckBox(i18n("Invert"),this,"invert");
  invert->move(340,30);
  invert->adjustSize();

  lsbgroup = new QButtonGroup(this,"lsbgroup");
  lsbgroup->setGeometry(20,100,370,30);
  lsbgroup->setFrameStyle(QFrame::NoFrame);


  lsblabel = new QLabel(i18n("Raw Fax Data are:"),lsbgroup,"lsblabel");
  lsblabel->move(5,10);
  lsblabel->adjustSize();
  
  lsb = new QCheckBox(i18n("LS-Bit first"),lsbgroup,"lsbitfirst");
  lsb->move(130,10);
  lsb->adjustSize();

  rawgroup = new QButtonGroup(this,"rawgroup");
  rawgroup->setGeometry(20,140,370,30);
  rawgroup->setFrameStyle(QFrame::NoFrame);

  rawlabel = new QLabel(i18n("Raw Facsimili are:"),rawgroup,"rawlabel");
  rawlabel->move(5,10);
  rawlabel->adjustSize();
  
  g3 = new QRadioButton("g3",rawgroup,"g3");
  g3->move(130,10);
  g3->adjustSize();
  connect(g3,SIGNAL(clicked()),this,SLOT(g3toggled()));

  g32d = new QRadioButton("g32d",rawgroup,"g32d");
  g32d->move(218,10);
  g32d->adjustSize();
  connect(g32d,SIGNAL(clicked()),this,SLOT(g32toggled()));


  g4 = new QRadioButton("g4",rawgroup,"g4");
  g4->move(320,10);
  g4->adjustSize();
  connect(g4,SIGNAL(clicked()),this,SLOT(g4toggled()));

  
  widthlabel = new QLabel(i18n("Raw Fax width:"),this,"widthlabel");
  widthlabel->move(25,190);
  widthlabel->adjustSize();

  widthedit = new KIntLineEdit(this,"widthedit");
  widthedit->setGeometry(135,185,60,22);

  heightlabel = new QLabel(i18n("height:"),this,"heightlabel");
  heightlabel->move(210,190);
  heightlabel->adjustSize();

  heightedit = new KIntLineEdit(this,"heightedit");
  heightedit->setGeometry(260,185,60,22);

  geomauto = new QCheckBox(i18n("Auto"),this,"geomauto");
  geomauto->setGeometry(340,185,50,22);
  connect(geomauto,SIGNAL(clicked()),this,SLOT(geomtoggled()));
  
  cancel_button = new QPushButton(i18n("Cancel"),this);

  cancel_button->setGeometry( 3*XOFFSET +100, 240, 80, BUTTONHEIGHT );
  connect( cancel_button, SIGNAL( clicked() ), SLOT( reject() ) );

  ok_button = new QPushButton( i18n("OK"), this );
  ok_button->setGeometry( 3*XOFFSET, 240, 80, BUTTONHEIGHT );
  connect( ok_button, SIGNAL( clicked() ), SLOT( ready() ) );	

  helpbutton = new QPushButton(i18n( "Help"), this );
  helpbutton->setGeometry( 410  - 80, 240, 80, BUTTONHEIGHT );
  connect( helpbutton, SIGNAL( clicked() ), SLOT( help() ) );	

  this->setFixedSize(420,270);


}


struct optionsinfo *  OptionsDialog::getInfo(){

  if(resauto->isChecked())
    oi.resauto = 1;

  if(fine->isChecked())
    oi.fine = 1;
  else
    oi.fine = 0;

  if(landscape->isChecked())
    oi.landscape = 1;
  else
    oi.landscape = 0;

  if(flip->isChecked())
    oi.flip = 1;
  else
    oi.flip = 0;

  if(invert->isChecked())
    oi.invert = 1;
  else
    oi.invert = 0;

  if(lsb->isChecked())
    oi.lsbfirst = 1;
  else
    oi.lsbfirst = 0;

  if(geomauto->isChecked())
    oi.geomauto = 1;


  if(g3->isChecked()){
    oi.raw = 3;
  }

  if(g32d->isChecked()){
    oi.raw = 2;
    oi.geomauto = 0;

  }

  if(g4->isChecked()){
    oi.raw = 4;
    oi.geomauto = 0;
  }

  oi.height = heightedit->getValue();
  oi.width = widthedit->getValue();



  return &oi;

}

void OptionsDialog::setWidgets(struct optionsinfo* newoi ){

  if(!newoi)
    return;

  if(newoi->resauto == 1){
    resauto->setChecked(TRUE);
    fine->setChecked(FALSE);
    normal->setChecked(FALSE);
  }
  else{
    if(newoi->fine == 1){
      resauto->setChecked(FALSE);
      fine->setChecked(TRUE);
      normal->setChecked(FALSE);
    }
    else{
      resauto->setChecked(FALSE);
      fine->setChecked(FALSE);
      normal->setChecked(TRUE);
    }
  }

  if(newoi->landscape == 1)
    landscape->setChecked(TRUE);
  else
    landscape->setChecked(FALSE);

  if(newoi->flip == 1)
    flip->setChecked(TRUE);
  else
    flip->setChecked(FALSE);

  if(newoi->invert == 1)
    invert->setChecked(TRUE);
  else
    invert->setChecked(FALSE);

  if(newoi->lsbfirst == 1)
    lsb->setChecked(TRUE);
  else
    lsb->setChecked(FALSE);

  if(newoi->raw == 3){
    geomauto->setEnabled(TRUE);
    g3->setChecked(TRUE);
  }

  if(newoi->raw == 2){
    geomauto->setEnabled(FALSE);
    g32d->setChecked(TRUE);
  }

  if(newoi->raw == 4){
    geomauto->setEnabled(FALSE);
    g4->setChecked(TRUE);
  }
  QString widthstr;
  widthstr.setNum(newoi->width);
  widthedit->setText(widthstr);

  QString heightstr;
  heightstr.setNum(newoi->height);
  heightedit->setText(heightstr);

  // auto height and width can only work with g3 faxes
  if(newoi->geomauto == 1 && newoi->raw != 4 && newoi->raw != 2){ 
    geomauto->setChecked(TRUE);
    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);
  }
  else{
    geomauto->setChecked(FALSE);
    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}


void OptionsDialog::g32toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);
  
}

void OptionsDialog::g4toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);


}


void OptionsDialog::g3toggled(){

  geomauto->setEnabled(TRUE);
  geomauto->setChecked(TRUE);
  widthedit->setEnabled(FALSE);
  heightedit->setEnabled(FALSE);


}

void OptionsDialog::geomtoggled(){

  if(geomauto->isChecked()){

    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);

  }
  else{

    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}

void OptionsDialog::ready(){

  accept();

}

void OptionsDialog::help(){

  mykapp->invokeHTMLHelp("","");

}




void OptionsDialog::focusInEvent(QFocusEvent* ){



}

