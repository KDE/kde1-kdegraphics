/*  
    KDE Icon Editor - a small graphics drawing program for the KDE

    Copyright (C) 1998 Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include "debug.h"
#include <qlayout.h>
#include "kiconfiledlg.h"

KIconFileDlg::KIconFileDlg(const char *dir, const char *filter)
 : KFileBaseDialog(dir, filter, 0, 0, true, true)
{
  initMetaObject();
  w = new QWidget;
  preview = new QLabel(w);
  preview->setFixedSize(100, 100);

  init();
}

KIconFileDlg::~KIconFileDlg()
{
  delete w;
}

QWidget *KIconFileDlg::swallower()
{
  return w;
}

QString KIconFileDlg::getOpenFileName(const char *dir, const char *filter)
{
    QString filename;
    KIconFileDlg *dlg = new KIconFileDlg(dir, filter);
    
    dlg->setCaption(i18n("Open image"));
    
    if (dlg->exec() == QDialog::Accepted)
	filename = dlg->selectedFile();
    
    delete dlg;
    
    return filename;
}
 
QString KIconFileDlg::getSaveFileName(const char *dir, const char *filter)
{
    KIconFileDlg *dlg = new KIconFileDlg(dir, filter);
    
    dlg->setCaption(i18n("Save image As"));
    
    QString filename;
    
    if (dlg->exec() == QDialog::Accepted)
	filename= dlg->selectedFile();
    
    delete dlg;
    
    return filename;
}

KFileInfoContents *KIconFileDlg::initFileList( QWidget *parent )
{

    bool mixDirsAndFiles = 
	kapp->getConfig()->readBoolEntry("MixDirsAndFiles", false);
    
    bool showDetails = 
	(kapp->getConfig()->readEntry("ViewStyle", 
				      "SimpleView") == "DetailView");
    
    bool useSingleClick =
	kapp->getConfig()->readBoolEntry("SingleClick",true);
    
    QDir::SortSpec sort = static_cast<QDir::SortSpec>(dir->sorting() &
                                                      QDir::SortByMask);
                                                      
    if (kapp->getConfig()->readBoolEntry("KeepDirsFirst", true))
        sort = static_cast<QDir::SortSpec>(sort | QDir::DirsFirst);

    dir->setSorting(sort);    

    if (!mixDirsAndFiles)
	
	return new KCombiView(KCombiView::DirList, 
				  showDetails ? KCombiView::DetailView 
				  : KCombiView::SimpleView,
				  useSingleClick, dir->sorting(),
				  parent, "_combi");
    
    else
	
	if (showDetails)
	    return new KFileDetailList(useSingleClick, dir->sorting(), parent, "_details");
	else
	    return new KFileSimpleView(useSingleClick, dir->sorting(), parent, "_simple");
    
}


