/*
* kview.h -- Declaration of class KView.
* Generated by newclass on Wed Oct 15 01:26:27 EST 1997.
*/
#ifndef SSK_KVIEW_H
#define SSK_KVIEW_H

#include<kapp.h>

class KFilterList;
class KFiltMenuFactory;

/**
* @short KView application object
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id$
*/
class KView
{
public:
	/**
	* KView Constructor
	*/
	KView(int argc, char **argv);

	/**
	* KView Destructor
	*/
	virtual ~KView();

	/**
	* Execute the application.
	*/
	virtual int exec();
private:
	KApplication _app;

	void registerBuiltinFilters();

	KFilterList *_filters;
	KFiltMenuFactory *_menuFact;
};

#endif // SSK_KVIEW_H