#ifndef _CONFIGHANDLER_H
#define _CONFIGHANDLER_H

#include <ktoolbar.h>
#include <qstring.h>

class KVConfigHandler
{
public:
  KVConfigHandler(){};
  ~KVConfigHandler(){};
  static void  readConfigEntries();
  static void  writeConfigEntries();

  static void printConfigEntries();

  // the config entries

  // General
  static int cacheSize;
  static int cachingOn;

  // Display Manager
  static int delay;
  static int frameSizeX;
  static int frameSizeY;
  static KToolBar::BarPosition toolbar1pos;
  static KToolBar::BarPosition toolbar2pos;


  // Image Window

private:
  static KToolBar::BarPosition QString2BarPos(QString aQString);
  static QString BarPos2QString(KToolBar::BarPosition aKTBPos); 
};

#endif
