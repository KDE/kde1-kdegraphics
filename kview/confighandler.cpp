/////////////////////////////////
//
// confighandler.cpp -- handles entries to read/write in kviewrc
//
// Martin Hartig

#include <stdio.h>

#include <kapp.h>
#include <Kconfig.h>
#include <qstring.h>

#include "confighandler.h"


extern KApplication *theApp;

int KVConfigHandler::cacheSize=1024;
int KVConfigHandler::delay=1000;
int KVConfigHandler::frameSizeX=250;
int KVConfigHandler::frameSizeY=300;
KToolBar::BarPosition KVConfigHandler::toolbar1pos=KToolBar::Top;
KToolBar::BarPosition KVConfigHandler::toolbar2pos=KToolBar::Top;



void KVConfigHandler::readConfigEntries()
{
  KConfig *config;
  QString str;
  config = theApp->getConfig();
 
  config->setGroup("General");

  str = config->readEntry("CacheSize");
  if ( !str.isNull() ) cacheSize=atoi(str.data());
      
  config->setGroup("Display Manager");
  
  str = config->readEntry("Delay");
  if ( !str.isNull() ) delay=atoi(str.data());

  str = config->readEntry("FrameSizeX");
  if ( !str.isNull() ) frameSizeX=atoi(str.data());
 
  str = config->readEntry("FrameSizeY");
  if ( !str.isNull() ) frameSizeY=atoi(str.data());

  str = config->readEntry("Toolbar1Position");
  if ( !str.isNull() ) toolbar1pos=QString2BarPos(str);

  str = config->readEntry("Toolbar2Position");
  if ( !str.isNull() ) toolbar2pos=QString2BarPos(str); 

}

void KVConfigHandler::writeConfigEntries()
{
  KConfig *config;
  QString str;
  config = theApp->getConfig();
  KVConfigHandler handler;

  config->setGroup("General");
  str.sprintf("%d",cacheSize);
  config->writeEntry("CacheSize",str);

  config->setGroup("Display Manager");
  str.sprintf("%d",delay);
  config->writeEntry("Delay",str);
  str.sprintf("%d",frameSizeX);
  config->writeEntry("FrameSizeX",str);
  str.sprintf("%d",frameSizeY);
  config->writeEntry("FrameSizeY",str);
  
  config->writeEntry("Toolbar1Position",BarPos2QString(toolbar1pos));
  config->writeEntry("Toolbar2Position",BarPos2QString(toolbar2pos));

  config->sync();
}


QString KVConfigHandler::BarPos2QString(KToolBar::BarPosition aKTBPos)
{
  QString aQString;
  switch (aKTBPos)
    {
    case KToolBar::Top: aQString = "Top"; 
      break;
    case KToolBar::Bottom: aQString = "Bottom";
      break;
    case KToolBar::Left: aQString = "Left";
      break;
    case KToolBar::Right: aQString = "Right";
      break;
    default: aQString = "Floating";
    }
  return aQString;
}

KToolBar::BarPosition KVConfigHandler::QString2BarPos(QString aQString)
{
  KToolBar::BarPosition tbpos;
  tbpos = KToolBar::Right;

  if (aQString=="Top")      tbpos = KToolBar::Top;
  if (aQString=="Bottom")   tbpos = KToolBar::Bottom;
  if (aQString=="Left")     tbpos = KToolBar::Left;
  if (aQString=="Right")    tbpos = KToolBar::Right;
  if (aQString=="Floating") tbpos = KToolBar::Floating;
  
  return tbpos;
}



void KVConfigHandler::printConfigEntries()
{
  printf("%d %d %d %d\n", cacheSize, delay, frameSizeX, frameSizeY);
}
