/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@earthling.net)

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
#include "kiconconfig.h"

KTemplateEditDlg::KTemplateEditDlg(QWidget *parent) : QDialog(parent, 0, true)
{
  ok = cancel = browse = 0L;
  QGroupBox *grp = new QGroupBox(i18n("Template"), this);
  lb_name = new QLabel(i18n("Description:"), this);
  lb_name->setFixedHeight(lb_name->sizeHint().height());
  lb_path = new QLabel(i18n("Path:"), this);
  lb_path->setFixedHeight(lb_path->sizeHint().height());

  ln_name = new QLineEdit(this);
  ln_name->setFixedHeight(20);
  connect( ln_name, SIGNAL(textChanged(const char *)), SLOT(slotTextChanged(const char*)) );

  ln_path = new QLineEdit(this);
  ln_path->setFixedHeight(20);
  connect( ln_path, SIGNAL(textChanged(const char *)), SLOT(slotTextChanged(const char*)) );

  QBoxLayout *ml = new QVBoxLayout(this, 10);
  //ml->addWidget(grp, 10, AlignLeft);
  QBoxLayout *l = new QVBoxLayout(grp, 25);
  l->addWidget(lb_name);
  l->addWidget(ln_name);
  l->addWidget(lb_path);
  l->addWidget(ln_path);

  KButtonBox *bbox = new KButtonBox( this );
	
  ok = bbox->addButton( i18n("&Ok") );
  connect( ok, SIGNAL(clicked()), SLOT(accept()) );
  ok->setEnabled(false);
	
  cancel = bbox->addButton( i18n("&Cancel") );
  connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
	
  bbox->addStretch( 10 );
		
  browse = bbox->addButton( i18n("&Browse...") );
  connect( browse, SIGNAL(clicked()), SLOT(slotBrowse()) );
		
  bbox->layout();
  bbox->setMaximumHeight(bbox->sizeHint().height());

  ml->addWidget(grp, 10, AlignLeft);
  ml->addWidget( bbox );

  l->activate();
  ml->activate();
}

void KTemplateEditDlg::slotTextChanged(const char *)
{
  if(ln_name->text() && strlen(ln_name->text()) > 0 && strlen(ln_path->text()) > 0)
  {
    if(ok) ok->setEnabled(true);
  }
  else
  {
    if(ok) ok->setEnabled(false);
  }
}

void KTemplateEditDlg::setName(QString name)
{
  ln_name->setText(name.data());
}

const char *KTemplateEditDlg::getName()
{
  return ln_name->text();
}

void KTemplateEditDlg::setPath(QString path)
{
  ln_path->setText(path.data());
}

const char *KTemplateEditDlg::getPath()
{
  return ln_path->text();
}

void KTemplateEditDlg::slotBrowse()
{
  QString path = KFilePreviewDialog::getOpenFileName("/", "*.xpm");
  if(!path.isEmpty())
    setPath(path);
}

KTemplateConfig::KTemplateConfig(QWidget *parent) : QWidget(parent)
{
  debug("KTemplateConfig constructor");
  btadd = btremove = btedit = 0L;
  grp = new QGroupBox(i18n("Templates:"), this);
  templates = new KIconListBox(grp);
  connect( templates, SIGNAL(highlighted(int)), SLOT(checkSelection(int)));
  ml = new QVBoxLayout(this, 10);
  ml->addWidget(grp);
  l = new  QVBoxLayout(grp, 15, AlignLeft);
  l->addWidget(templates);

  tlist = KIconTemplateContainer::getTemplates();

  for(int i = 0; i < (int)tlist->count(); i++)
    templates->insertItem(new KIconListBoxItem(tlist->at(i)));

  KButtonBox *bbox = new KButtonBox( grp );
	
  btadd = bbox->addButton( i18n("&Add") );
  connect( btadd, SIGNAL(clicked()), SLOT(add()) );
	
  btedit = bbox->addButton( i18n("&Edit") );
  connect( btedit, SIGNAL(clicked()), SLOT(edit()) );
  btedit->setEnabled(false);
	
  btremove = bbox->addButton( i18n("&Remove") );
  connect( btremove, SIGNAL(clicked()), SLOT(remove()) );
  btremove->setEnabled(false);
		
  bbox->addStretch( 10 );
		
  bbox->layout();
  bbox->setMaximumHeight(bbox->sizeHint().height());
  l->addWidget( bbox );

  l->activate();
  ml->activate();

  debug("KTemplateConfig constructor - done");
}

void KTemplateConfig::saveSettings()
{
  debug("KTemplateConfig::saveSettings");

  for(int i = 0; i < (int)templates->count(); i++)
  {
    if(!tlist->containsRef(templates->iconTemplate(i)))
    {
      KIconTemplate *it = new KIconTemplate;
      it->path = templates->path(i);
      //it->path.detach();
      it->title = templates->text(i);
      //it->title.detach();
      tlist->append(it);
      debug("Name: %s\nPath: %s", templates->text(i), templates->path(i).data());
      debug("Name: %s\nPath: %s", it->path.data(), it->path.data());
    }
  }
  debug("KTemplateConfig::saveSettings - done");

}

void KTemplateConfig::checkSelection(int)
{
  debug("KTemplateConfig::checkSelection");
  if(templates->currentItem() != -1)
  {
    if(btremove) btremove->setEnabled(true);
    if(btedit) btedit->setEnabled(true);
  }
  else
  {
    if(btremove) btremove->setEnabled(false);
    if(btedit) btedit->setEnabled(false);
  }
  debug("KTemplateConfig::checkSelection - done");
}

void KTemplateConfig::remove()
{
  templates->removeItem(templates->currentItem());
}

void KTemplateConfig::add()
{
  KTemplateEditDlg dlg(this);
  if(dlg.exec())
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = dlg.getPath();
    it->title = dlg.getName();
    //tlist->append(it);
    templates->insertItem(new KIconListBoxItem(it));
  }
}

void KTemplateConfig::edit()
{
  KTemplateEditDlg dlg(this);
  dlg.setPath(templates->path(templates->currentItem()).data());
  dlg.setName(templates->text(templates->currentItem()));
  if(dlg.exec())
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = dlg.getPath();
    it->title = dlg.getName();
    tlist->append(it);
    templates->insertItem(new KIconListBoxItem(it));
  }
}

KBackgroundConfig::KBackgroundConfig(QWidget *parent)
{
  debug("KBackgroundConfig - constructor");
  pprops = KIconEditProperties::getProperties(parent);
  initMetaObject();

  QGroupBox *grp = new QGroupBox(i18n("Color or pixmap"), this);

  btcolor = new KColorButton(grp);
  btcolor->setFixedSize(btcolor->sizeHint());
  connect(btcolor, SIGNAL( changed( const QColor &)),
         SLOT( colorChanged( const QColor &)));

  btbrowse = new QPushButton(i18n("Browse..."), grp);
  btbrowse->setFixedSize(btbrowse->sizeHint());
  connect(btbrowse, SIGNAL(clicked()), SLOT(selectPixmap()));

  QBoxLayout *ml = new QVBoxLayout(this, 10);
  ml->addWidget(grp);
  QBoxLayout *l = new  QVBoxLayout(grp, 10, AlignLeft);
  l->addWidget(btcolor);
  l->addWidget(btbrowse);

  l->activate();
  ml->activate();

  debug("KBackgroundConfig - constructor done");
}

KBackgroundConfig::~KBackgroundConfig()
{

}

void KBackgroundConfig::saveSettings()
{

}

void KBackgroundConfig::colorChanged( const QColor &newcolor )
{
  setBackgroundColor(newcolor);
}

void KBackgroundConfig::selectPixmap()
{
  QString path = KFilePreviewDialog::getOpenFileName("/", "*.xpm");
  if(!path.isEmpty())
  {
    QPixmap p(path.data());
    if(!p.isNull())
      setBackgroundPixmap(p);
  }
}

KIconConfig::KIconConfig(QWidget *parent) : KNoteBook(parent, 0, true)
{
  pprops = KIconEditProperties::getProperties(parent);
  debug("KIconConfig - constructor");
  initMetaObject();
  setCaption("Configure ");
  //keys = k; //new KAccel(parent);
  //dict = new QDict<KKeyEntry>( k->keyDict() );
  //debug("Keys: %u", k->keyDict().count());
  //debug("Keys: %u", dict->count());
  //CHECK_PTR(dict);
  pprops->keydict = pprops->keys->keyDict();
  debug("Keys: %u", pprops->keydict.count());

  setCancelButton();
  setOkButton();
  //setDirectionsReflectsPage(true);
  setEnablePopupMenu(true);
  setEnableArrowButtons(true);

  debug("KIconConfig - constructor");

  keychooser = new KKeyChooser(&pprops->keydict, this);
  debug("KeyChooser created");
  keychooser->setMinimumSize(300, 275);

  debug("KIconConfig - constructor");

  connect( this, SIGNAL(cancelclicked()), SLOT(reject()) );
  connect( this, SIGNAL(okclicked()), SLOT(saveSettings()) );

  QTab *tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Keys");
  tab->enabled = true;
  addTab( tab );
  KWizardPage *p = new KWizardPage;
  p->w = keychooser;
  p->title = i18n("Configure keys");
  p->enabled = true;
  addPage( p );

  temps = new KTemplateConfig(this);

  tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Templates");
  tab->enabled = true;
  addTab( tab );
  p = new KWizardPage;
  p->w = temps;
  p->title = i18n("Configure Icon templates");
  p->enabled = true;
  addPage( p );

  backs = new KBackgroundConfig(this);

  tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Appearance");
  tab->enabled = true;
  addTab( tab );
  p = new KWizardPage;
  p->w = backs;
  p->title = i18n("Configure background");
  p->enabled = true;
  addPage( p );

  setSizes();

  debug("KIconConfig - constructor - done");
}

KIconConfig::~KIconConfig()
{
  //delete dict;
}

void KIconConfig::saveSettings()
{
  debug("OK clicked");

  pprops->keys->setKeyDict(pprops->keydict);
  debug("KIconEditConfig::saveSettings - keys saved");
  temps->saveSettings();
  debug("KIconEditConfig::saveSettings - templates saved");
  accept();
}


