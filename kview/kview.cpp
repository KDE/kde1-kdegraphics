/*
* kview.cpp -- Implementation of class KView.
* Author:	Sirtaj Singh Kang
* Version:	$Id$
* Generated:	Wed Oct 15 01:26:27 EST 1997
*/

#ifdef CORBA
#include"kmicoapp.h"
#else
#include<kapp.h>
#endif

#include<assert.h>

#include"kview.h"
#include"viewer.h"
#include<qimage.h>
#include<kimgio.h>

#include"filter.h"
#include"filtlist.h"
#include"filtmenu.h"
#include"colour.h"

KView::KView(int argc, char **argv)
	: _app( argc, argv, "kView" ),
	_filters( new KFilterList ),
	_menuFact( new KFiltMenuFactory( _filters ) )
{
	assert( _filters );

	kimgioRegister();

	registerBuiltinFilters();
}

KView::~KView()
{
	delete _menuFact;
	delete _filters;
}

int KView::exec()
{
	KImageViewer *viewer = new KImageViewer;

	_app.setMainWidget( viewer );

	viewer->setFilterMenu( _menuFact );

	if( _app.isRestored() ) {
		debug( "Restoring kview %d", 1 );
		viewer->restore( 1 );
	}
	else {
		viewer->show();

		// process arguments only if not restored
		for( int i = 1; i <= _app.argc(); i++ ) {
			viewer->appendURL( _app.argv()[ i ],
				(i == 1)  );
		}
	}



	int ret = _app.exec();

	delete viewer;

	return ret;
}

void KView::registerBuiltinFilters()
{
	_filters->registerFilter( new BriteFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new DarkFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new GreyFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new SmoothFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new GammaFilter, 
		KFilterList::AutoDelete );
}

