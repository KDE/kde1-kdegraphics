#ifndef APP_H
#define APP_H

#include <kapp.h>
#include <qstring.h>
#include "kpaint.h"

class MyApp : public KApplication {
public:
  	MyApp( int &argc, char **argv, QString appname );
 	virtual bool x11EventFilter( XEvent * );
        KPaint *kp;
  	
  	void changePalette();
  	void changeGeneral();
  	void readSettings();
  
  	QColor inactiveTitleColor;
	QColor inactiveTextColor;
	QColor activeTitleColor;
	QColor activeTextColor;
	QColor backgroundColor;
	QColor textColor;
	QColor selectColor;
	QColor selectTextColor;
	QColor windowColor;
	QColor windowTextColor;
	QFont generalFont;
	GUIStyle applicationStyle;
};

#endif // APP_H
