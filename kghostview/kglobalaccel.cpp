/* This file is part of the KDE libraries
	Copyright (C) 1998 Jani Jaakkola (jjaakkol@cs.helsinki.fi),
					   Nicolas Hadacek <hadacek@via.ecp.fr>,
					   Matthias Ettrich (ettrich@kde.org) 

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public License
	along with this library; see the file COPYING.LIB.  If not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*/

#include "kglobalaccel.h"
#include "kglobalaccel.moc"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <string.h>

#include <qkeycode.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qapp.h>
#include <qdrawutl.h>
#include <qmsgbox.h>

#include <kapp.h>

// NOt ABOUT CONFIGURATION CHANGES
// Test if keys enabled because these keys have made X server grabs 
//



bool grabFailed;

static int XGrabErrorHandler(Display *, XErrorEvent *e) {
	if (e->error_code!=BadAccess) {
   warning("grabKey: got X error %d instead of BadAccess",e->type);
	}
	grabFailed=true;
	return 0;
}

bool KGlobalAccel::x11EventFilter( const XEvent *event_ ) {
	if ( aKeyDict.isEmpty() ) return false;
	if ( event_->type != KeyPress ) return false;
	
	uint mod=event_->xkey.state & (ControlMask | ShiftMask | Mod1Mask);
	uint keysym= XKeycodeToKeysym(qt_xdisplay(), event_->xkey.keycode, 0);
	
	debug("Key press event :: mod = %d, sym =%d", mod, keysym );
   
   KKeyEntry *pEntry =0;
   const char *action;
   
	QDictIterator<KKeyEntry> *aKeyIt = new QDictIterator<KKeyEntry>( aKeyDict );
	aKeyIt->toFirst();
#define pE aKeyIt->current()
	while( pE ) { //&& 
		//( mod != keyToXMod( pE->aCurrentKeyCode ) 
		//	|| keysym != keyToXSym( pE->aCurrentKeyCode ) ) ) {
		int kc = pE->aCurrentKeyCode;
		if( mod == keyToXMod( kc ) ) debug("That's the right modifier");
		if( keysym == keyToXSym( kc ) ) debug("That's the right symbol");
		if ( mod == keyToXMod( kc ) && keysym == keyToXSym( kc ) ) {
		//pEntry = pE;
		//action = aKeyIt->currentKey();
		//break;
		debug("Found key in dictionary for action %s", aKeyIt->currentKey());
		break;
		}
		++*aKeyIt;
	}
	
	if ( !pE ) {
		debug("Null KeyEntry object");
		return false; 
	}
	
	if ( !pE ) {
		debug("KeyEntry object not enabled");
		return false; 
	}

	debug("KGlobalAccel:: event action %s", aKeyIt->currentKey() );

	XAllowEvents(qt_xdisplay(), AsyncKeyboard, CurrentTime);
	
	connect( this, SIGNAL( activated() ), pE->receiver, pE->member->data() );
	emit activated();
	disconnect( this, SIGNAL( activated() ), pE->receiver, pE->member->data() );

//    warning("Signal has been sent!");
	return true;
}

bool KGlobalAccel::grabKey( uint keysym, uint mod ) {
	// Most of this comes from kpanel/main.C
	// Copyright (C) 1996,97 Matthias Ettrich
	static int NumLockMask = 0;
	
	debug("KGlobalAccel::grabKey");
	
	if (!XKeysymToKeycode(qt_xdisplay(), keysym)) return false; 
	if (!NumLockMask){
		XModifierKeymap* xmk = XGetModifierMapping(qt_xdisplay());
		int i;
		for (i=0; i<8; i++){
		   if (xmk->modifiermap[xmk->max_keypermod * i] == 
		   		XKeysymToKeycode(qt_xdisplay(), XK_Num_Lock))
		   			NumLockMask = (1<<i); 
		}
	}

	grabFailed = false;

	// We wan't to catch only our own errors
	XSync(qt_xdisplay(),0);
	XErrorHandler savedErrorHandler=XSetErrorHandler(XGrabErrorHandler);
	
	debug("Will grab key and variants with keyboard locks: %d, %d", keysym, mod);

	XGrabKey(qt_xdisplay(),
		XKeysymToKeycode(qt_xdisplay(), keysym), mod,
		qt_xrootwin(), True,
		GrabModeAsync, GrabModeSync);
	XGrabKey(qt_xdisplay(),
		XKeysymToKeycode(qt_xdisplay(), keysym), mod | LockMask,
		qt_xrootwin(), True,
		GrabModeAsync, GrabModeSync);
	XGrabKey(qt_xdisplay(),
		XKeysymToKeycode(qt_xdisplay(), keysym), mod | NumLockMask,
		qt_xrootwin(), True,
		GrabModeAsync, GrabModeSync);
	XGrabKey(qt_xdisplay(),
		XKeysymToKeycode(qt_xdisplay(), keysym), mod | LockMask | NumLockMask,
		qt_xrootwin(), True,
		GrabModeAsync, GrabModeSync);

	XSync(qt_xdisplay(),0);
	XSetErrorHandler(savedErrorHandler);
	if (grabFailed) {
		// FIXME: ungrab all successfull grabs!
		//warning("Global grab failed!");
   		return false;
	}
	return true;
}

KGlobalAccel::KGlobalAccel()
 : QObject(), aKeyDict(37)
{
	aAvailableId = 1;
	//aKeyDict.setAutoDelete(TRUE);
	QString keychar("W");
	uint keysym = XStringToKeysym( keychar.data() );
	debug("Key = %s, sym = %d", keychar.data(), keysym );
	keychar.sprintf("a");
	keysym = XStringToKeysym( keychar.data() );
	debug("Key = %s, sym = %d", keychar.data(), keysym );
	keychar.sprintf(" ");
	keysym = XStringToKeysym( keychar.data() );
	debug("Key = %s, sym = %d", keychar.data(), keysym );
}

KGlobalAccel::~KGlobalAccel()
{
	//setEnabled( false );
}

uint KGlobalAccel::currentKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
		return 0;
	else
		return pEntry->aCurrentKeyCode;
}

QDict<KKeyEntry> * KGlobalAccel::keyDict()
{
	return &aKeyDict;
}

bool KGlobalAccel::setKeyDict( QDict<KKeyEntry> nKeyDict )
{
	// ungrab all connected and enabled keys
	QDictIterator<KKeyEntry> *aKeyIt = new QDictIterator<KKeyEntry>( aKeyDict );
	aKeyIt->toFirst();
#define pE aKeyIt->current()
	while( pE ) {
		QString s;
		if ( pE->aAccelId ) {
		}
		++*aKeyIt;
	}
#undef pE
	
	// Clear the dictionary
	aKeyDict.clear();
	
	// Insert the new items into the dictionary and reconnect if neccessary
	// Note also swap config and current key codes !!!!!!
	aKeyIt = new QDictIterator<KKeyEntry>( nKeyDict );
	aKeyIt->toFirst();
#define pE aKeyIt->current()
	KKeyEntry *pEntry;
	while( pE ) {
		pEntry = new KKeyEntry;
		aKeyDict.insert( aKeyIt->currentKey(), pEntry );

		pEntry->aDefaultKeyCode = pE->aDefaultKeyCode;
		// Not we write config key code to current key code !!
		pEntry->aCurrentKeyCode = pE->aConfigKeyCode;
		pEntry->aConfigKeyCode = pE->aConfigKeyCode;
		pEntry->bConfigurable = pE->bConfigurable;
		pEntry->aAccelId = pE->aAccelId;
		pEntry->receiver = pE->receiver;
		pEntry->member = new QString( pE->member->data() );
		
		if ( pEntry->aAccelId && pEntry->aCurrentKeyCode ) {
		}
		++*aKeyIt;
	}
#undef pE
		
	return true;
}

uint KGlobalAccel::defaultKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
        return 0;
    else
        return pEntry->aDefaultKeyCode;
}


bool KGlobalAccel::insertItem( const char * action, uint keyCode,
					   bool configurable )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( pEntry )
		removeItem( action );

	pEntry = new KKeyEntry;
	aKeyDict.insert( action, pEntry );
	
	pEntry->aDefaultKeyCode = keyCode;
	pEntry->aCurrentKeyCode = keyCode;
	pEntry->bConfigurable = configurable;
	pEntry->bEnabled = false;
	pEntry->aAccelId = 0;
	pEntry->receiver = 0;
	pEntry->member = 0;

	if ( !configurable )
		return TRUE;
	
	return TRUE;
}

bool KGlobalAccel::insertItem( const char * action, 
					   const char * keyCode, bool configurable )
{
	uint iKeyCode = stringToKey(keyCode);
	if ( iKeyCode==0 ) {
		QString str;
		str.sprintf("insertItem : unrecognized key string %s", 
					(const char *)keyCode);
		warning(str);
		return FALSE;
	}
	
	return insertItem(action, iKeyCode, configurable);
}
	
void KGlobalAccel::removeItem( const char * action )
{
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
	
    if ( !pEntry ) 
		return;
	
	if ( pEntry->aAccelId ) {
	}
	
	aKeyDict.remove( action );
}

void KGlobalAccel::connectItem( const char * action,
							const QObject* receiver, const char* member,
							bool activate )
{
	QString str;
	
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
	if ( !pEntry ) {
		str.sprintf( "connectItem : \"%s\" function does not exist",
					 (const char *)action );
		warning(str);
		return;
	}
	
	pEntry->receiver = receiver;
	pEntry->member = new QString( member );
	pEntry->aAccelId = aAvailableId;
	aAvailableId++;
	
	setItemEnabled( action, activate );
}

void KGlobalAccel::setItemEnabled( const char * action, bool activate )
{	
	QString str;
	
	debug("KGlobalAccel::setItemEnabled");
	
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
	if ( !pEntry ) {
		str.sprintf( "KGlobalAccel::setItemEnabled() : \"%s\" action does not exist",
					 (const char *)action );
		warning(str);
		return;
	}
	
	if ( pEntry->bEnabled == activate ) {
		debug(" Already in this state");
		return;
	}
	if ( pEntry->aCurrentKeyCode == 0 ) {
		debug("key is undefined. Can't activate");
		return;
	}
	
	uint keysym = keyToXSym( pEntry->aCurrentKeyCode );
	uint mod = keyToXMod( pEntry->aCurrentKeyCode );
	
	if ( keysym == NoSymbol ) {
	debug(" X keysym not found - can't activate");
	return;
	}
	
	debug("OK key code - call grabKey");

	if ( !pEntry->bEnabled ) {
   		grabKey( keysym, mod );
   		pEntry->bEnabled = true;
		debug("		key grabbed and enabled bool set");
	} else {
		warning("KGlobalAccel::setItemEnabled() cannot disable keys");
	}
   
	return;

}

void KGlobalAccel::disconnectItem( const char * action,
							const QObject* receiver, const char* member )
{
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
    if ( !pEntry ) 
		return;
	
	//pAccel->disconnectItem( pEntry->aAccelId, receiver, member  );
}

void KGlobalAccel::writeSettings()
{
	KConfig *pConfig = kapp->getConfig();
	
	/* write the current values in aKeyDict to KConfig object */
	pConfig->setGroup( "Keys" );

	QDictIterator<KKeyEntry> aKeyIt( aKeyDict );
	aKeyIt.toFirst();
	while ( aKeyIt.current() ) {
		if ( aKeyIt.current()->bConfigurable )
			pConfig->writeEntry( aKeyIt.currentKey(),
				keyToString( aKeyIt.current()->aCurrentKeyCode ) );
		++aKeyIt;
	}
}

void KGlobalAccel::readSettings()
{
	KConfig *pConfig = kapp->getConfig();
	
	/* write the current values in aKeyDict to KConfig object */
	pConfig->setGroup( "Keys" );

	QDictIterator<KKeyEntry> aKeyIt( aKeyDict );
	aKeyIt.toFirst();
#define pE aKeyIt.current()
	while ( pE ) {
		pE->aConfigKeyCode =
			stringToKey( pConfig->readEntry( aKeyIt.currentKey(),
							keyToString( pE->aConfigKeyCode ) ) );
		pE->aCurrentKeyCode = pE->aConfigKeyCode;
		if ( pE->aAccelId && pE->aCurrentKeyCode ) {
			
		}
		++aKeyIt;
	}
#undef pE
}

//bool KGlobalAccel::configureKeys( QWidget *parent )
//{
//	QDictIterator<KKeyEntry> aKeyIt(aKeyDict);
//	KGlobalAccelure kDialog( &aKeyIt, parent );
//	
//	kDialog.resize(450,320);
//	
//	if ( kDialog.exec() ) {
//		bool modified = FALSE;
//		bool activated;
//  	  /* copy all modified configKeyCodes to
//  		 currentKeyCodes & redo the connections */
//  	  aKeyIt.toFirst();
//  	  #define pE aKeyIt.current()
//  	  while ( pE ) {
//  		  if ( pE->aCurrentKeyCode != pE->aConfigKeyCode ) {
//  			  modified = TRUE;
//  			  pE->aCurrentKeyCode = pE->aConfigKeyCode;
//  			  if ( pE->pConnectDict ) {
//  				  QDictIterator<KKeyConnectEntry> aCIt( *pE->pConnectDict );
//  				  aCIt.toFirst();
//  				  while ( aCIt.current() ) {
//  					  KKeyWidgetEntry *pWE =
//  						  kKeys->aWidgetDict[ aCIt.currentKey() ];
//  					  activated = pWE->isItemEnabled( pE->aAccelId );
//  					  pWE->deleteItem( pE->aAccelId,
//  								  aCIt.current()->pReceiver,
//  								  (const char *)aCIt.current()->sMember );
//  					  pWE->createItem( pE->aAccelId, pE->aCurrentKeyCode,
//  								  aCIt.current()->pReceiver,
//  								  (const char *)aCIt.current()->sMember );
//  					  pWE->setItemEnabled( pE->aAccelId, activated );
//  					  ++aCIt;
//  				  }
//  			  }
//  		  }
//  		  ++aKeyIt;
//		}
//
//		/* sync these new keys */
//  	  if ( modified ) {
//  		  sync();
//  		  return TRUE;
//  	  }
//    }
//
//    /* do nothing : no change */
//	return FALSE; 
//}

/*****************************************************************************/

/*bool KGlobalShortcut::setDefaultKey(const QString &key) {
	

	char *toks[4], *next_tok;
	int nb_toks = 0;
	char sKey[200];

	strncpy(sKey, (const char *)key, 200);
	next_tok = strtok(sKey,"+");
	
	if ( next_tok==NULL ) return 0;
	
	do {
   toks[nb_toks] = next_tok;
   nb_toks++;
   if ( nb_toks==5 ) return 0;
   next_tok = strtok(NULL, "+");
	} while ( next_tok!=NULL );
	
	bool  keyFound = FALSE;
	uint mod=0;
	uint keysym=0;
	for (int i=0; i<nb_toks; i++) {
   if ( strcmp(toks[i], "SHIFT")==0 )
	   mod |= ShiftMask;
   else if ( strcmp(toks[i], "CTRL")==0 )
	   mod |= ControlMask;
   else if ( strcmp(toks[i], "ALT")==0 )
	   mod |= Mod1Mask;
   else {
	  
	   if ( keyFound ) return 0;
	   keyFound = TRUE;

	   keysym=XStringToKeysym(toks[i]);
	   if (keysym==NoSymbol) return false;
		}
	}

	return setDefaultKey(keysym,mod);  
}*/

uint keyToXMod( uint keyCode )
{
	uint mod = 0;
	
	if ( keyCode == 0 ) return mod;
	
	if ( keyCode & SHIFT )
		 mod |= ShiftMask;
	if ( keyCode & CTRL )
		 mod |= ControlMask;
	if ( keyCode & ALT )
		 mod |= Mod1Mask;
		 
	return mod;
}

uint keyToXSym( uint keyCode )
{
	debug("keyToXSym");
	char *toks[4], *next_tok;
	int nb_toks = 0;
	char sKey[200];

	
	
	uint keysym = 0;
	QString s = keyToString( keyCode );
	s= s.lower();
	
	strncpy(sKey, (const char *)s.data(), 200);
	
	debug("Find key %s", sKey ); 
	
	if ( s.isEmpty() ) return keysym;
	
	next_tok = strtok( sKey, "+" );
	
	if ( next_tok == NULL ) return 0;
	
	do {
		toks[nb_toks] = next_tok;
		nb_toks++;
		if ( nb_toks == 5 ) return 0;
		next_tok = strtok( NULL, "+" );
	} while ( next_tok != NULL );

	// Test for exactly one key (other tokens are accelerators)
	// Fill the keycode with infos
	bool  keyFound = FALSE;
	for ( int i=0; i<nb_toks; i++ ) {
		debug("%d", i);
		if ( strcmp( toks[i], "shift" ) != 0 &&
			 strcmp( toks[i], "ctrl" ) != 0 &&
			 strcmp( toks[i], "alt" ) != 0 ) {
		   if ( keyFound ) return 0;
		   keyFound = TRUE;

		   keysym = XStringToKeysym( toks[i] );
		   debug("Key = %s, sym = %d", toks[i], keysym );
		   if ( keysym == NoSymbol ) {
		   	debug("Keysym no symbol (%d)", NoSymbol);
			return 0;
		  }
		}
	}
	
	//debug("Return %d", keysym);
	return keysym;
}
