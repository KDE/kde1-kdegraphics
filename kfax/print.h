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

    Revision 1.3  1997/06/13 05:46:55  wuebben
    *** empty log message ***

    Revision 1.2  1997/06/12 22:11:17  wuebben
    *** empty log message ***

    Revision 1.1  1997/06/12 16:29:19  wuebben
    Initial revision




*/


#ifndef _PRINT_DIALOG_H_
#define _PRINT_DIALOG_H_

#include <qmsgbox.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qfiledlg.h>
#include <qframe.h> 
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qscrbar.h>
#include <qtooltip.h>

#include <qstring.h>
#include <qfont.h>


struct printinfo {
  QString file;
  QString cmd;
  int lpr;
};

class PrintDialog : public QDialog {

    Q_OBJECT

public:
    PrintDialog( QWidget *parent = NULL, const char *name = NULL ,bool = FALSE);

    struct printinfo* getInfo();
    void setWidgets(struct printinfo *pi);

protected:
    void focusInEvent ( QFocusEvent *);

signals:
    void print();

public slots:
    void choosefile();
    void ready();
    void cancel();
    void filebuttontoggled();

private:

    QString command;
    QLineEdit *filename;
    QLineEdit *cmdedit;
    QLabel *cmdlabel;
    QRadioButton *filebutton;
    QRadioButton *lprbutton;
   
    QButtonGroup *bg;

    QGroupBox	 *box1;

    
    QPushButton	 *ok_button;
    QPushButton	 *cancel_button;
    QPushButton	 *selectfile;
    
  struct printinfo pi;

};


#endif
