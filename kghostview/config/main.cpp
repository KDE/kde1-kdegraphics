/*
  main.cpp - A sample KControl Application

  written 1997 by Matthias Hoelzer
  
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


//#include <kapp.h>
//#include <qobject.h>
#include <kslider.h>

#include <kcontrol.h>
#include "kcmkeys.h"

#include "display.h"


class KDisplayApplication : public KControlApplication
{
public:

  KDisplayApplication(int &argc, char **arg, const char *name);

  void init();
  void apply();

private:

  KShortcuts *background;
};


KDisplayApplication::KDisplayApplication(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
  background = 0;

  if (runGUI())
    {
    
      if (!pages || pages->contains("background"))
        addPage(background = new KShortcuts(dialog, KDisplayModule::Setup), 
		klocale->translate("&Desktop shortcuts"), "kdisplay-3.html");
     
      
      if (background)
        dialog->show();
      else
        {
          fprintf(stderr, klocale->translate("usage: kcmdisplay [-init | {background,screensaver,colors,style}]\n"));
          justInit = TRUE;
        }
      
    }
}


void KDisplayApplication::init()
{
 
  KShortcuts *background =  new KShortcuts(0, KDisplayModule::Init);
  delete background;
 
}


void KDisplayApplication::apply()
{

  if (background)
    background->applySettings();

}

int main(int argc, char **argv)
{
  //QApplication::setColorSpec( QApplication::ManyColor );
  // Please don't use this as it prevents users from choosing exactly the
  // colors they want - Mark Donohoe
  
  KDisplayApplication app(argc, argv, "kdisplay");
  app.setTitle(klocale->translate("Input devices settings"));
  
  if (app.runGUI())
    return app.exec();
  else
    { 
      app.init();
      return 0;
    }
}
