/* This file is part of the KDE libraries
	Copyright (C) 1998 Mark Donohoe <donohoe@kde.org>
    Copyright (C) 1997 Nicolas Hadacek <hadacek@via.ecp.fr>

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

#include <qkeycode.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qapp.h>
#include <qdrawutl.h>
#include <qmsgbox.h>

#include <kapp.h>

#include "kaccel.h"

KAccel::KAccel( QWidget * parent, const char * name )
	: aKeyDict(37)
{
	aAvailableId = 1;
	//aKeyDict.setAutoDelete(TRUE);
	pAccel = new QAccel( parent );
}

KAccel::~KAccel()
{
}

uint KAccel::currentKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
		return 0;
	else
		return pEntry->aCurrentKeyCode;
}

QDict<KKeyEntry> * KAccel::keyDict()
{
	return &aKeyDict;
}

bool KAccel::setKeyDict( QDict<KKeyEntry> nKeyDict )
{
	// Disconnect and remove all items in pAccel
	QDictIterator<KKeyEntry> *aKeyIt = new QDictIterator<KKeyEntry>( aKeyDict );
	aKeyIt->toFirst();
#define pE aKeyIt->current()
	while( pE ) {
		QString s;
		if ( pE->aAccelId ) {
			pAccel->disconnectItem( pE->aAccelId, pE->receiver, 
												pE->member->data() );
			pAccel->removeItem( pE->aAccelId );
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
			pAccel->insertItem( pEntry->aCurrentKeyCode, pEntry->aAccelId );
			pAccel->connectItem( pEntry->aAccelId, pEntry->receiver, 
													pEntry->member->data() );
		}
		++*aKeyIt;
	}
#undef pE
		
	return true;
}

uint KAccel::defaultKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
        return 0;
    else
        return pEntry->aDefaultKeyCode;
}


bool KAccel::insertItem( const char * action, uint keyCode,
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
	pEntry->aAccelId = 0;
	pEntry->receiver = 0;
	pEntry->member = 0;

	if ( !configurable )
		return TRUE;
	
	return TRUE;
}

bool KAccel::insertItem( const char * action, 
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
	
void KAccel::removeItem( const char * action )
{
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
	
    if ( !pEntry ) 
		return;
	
	if ( pEntry->aAccelId ) {
		pAccel->disconnectItem( pEntry->aAccelId, pEntry->receiver, 
					pEntry->member->data());
		pAccel->removeItem( pEntry->aAccelId );
	}
	
	aKeyDict.remove( action );
}

void KAccel::connectItem( const char * action,
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
	
	pAccel->insertItem( pEntry->aCurrentKeyCode, pEntry->aAccelId );
	pAccel->connectItem( pEntry->aAccelId, receiver, member );
	
	if ( !activate )
		setItemEnabled( action, FALSE );
}

void KAccel::setItemEnabled( const char * action, bool activate )
{	
	QString str;
	
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
	if ( !pEntry ) {
		str.sprintf( "(dis)activateFunction : \"%s\" function does not exist",
					 (const char *)action );
		warning(str);
		return;
	}

	pAccel->setItemEnabled( pEntry->aAccelId, activate );
}

void  KAccel::disconnectItem( const char * action,
							const QObject* receiver, const char* member )
{
	/* search for an existing "action" entry */
    KKeyEntry *pEntry = aKeyDict[ action ];
    if ( !pEntry ) 
		return;
	
	pAccel->disconnectItem( pEntry->aAccelId, receiver, member  );
}

void KAccel::writeSettings()
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

void KAccel::readSettings()
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
			pAccel->disconnectItem( pE->aAccelId, pE->receiver, 
													pE->member->data() );
			pAccel->removeItem( pE->aAccelId );
			pAccel->insertItem( pE->aCurrentKeyCode, pE->aAccelId );
			pAccel->connectItem( pE->aAccelId, pE->receiver, 
													pE->member->data() );
		}
		++aKeyIt;
	}
#undef pE
}

//bool KAccel::configureKeys( QWidget *parent )
//{
//	QDictIterator<KKeyEntry> aKeyIt(aKeyDict);
//	KAccelure kDialog( &aKeyIt, parent );
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
const QString keyToString( uint keyCode )
{
	QString res;
	
	if ( keyCode == 0 ) {
		res.sprintf( "" );
		return res;
	}
	
	if ( keyCode & SHIFT )
		res = "SHIFT+";
	if ( keyCode & CTRL )
		res += "CTRL+";
	if ( keyCode & ALT )
		res += "ALT+";
	
	uint kCode = keyCode & ~(SHIFT | CTRL | ALT);

	for (int i=0; i<NB_KEYS; i++) {
		if ( kCode == (uint)KKeys[i].code ) {
			res += KKeys[i].name;
			return res;
		}
	}
	
	return 0;
}

uint stringToKey(const char * key )
{
	char *toks[4], *next_tok;
	uint keyCode = 0;
	int j, nb_toks = 0;
	char sKey[200];
	
	strncpy(sKey, (const char *)key, 200);
	
	if ( strcmp( sKey, "" ) == -1  ) return keyCode;
	
	next_tok = strtok(sKey,"+");
	
	if ( next_tok==NULL ) return 0;
	
	do {
		toks[nb_toks] = next_tok;
		nb_toks++;
		if ( nb_toks==5 ) return 0;
		next_tok = strtok(NULL, "+");
	} while ( next_tok!=NULL );
	
	/* we test if there is one and only one key (the others tokens
	   are accelerators) ; we also fill the keycode with infos */
	bool  keyFound = FALSE;
	for (int i=0; i<nb_toks; i++) {
		if ( strcmp(toks[i], "SHIFT")==0 )
			keyCode |= SHIFT;
		else if ( strcmp(toks[i], "CTRL")==0 )
		    keyCode |= CTRL;
	    else if ( strcmp(toks[i], "ALT")==0 )
		    keyCode |= ALT;
	    else {
			/* key already found ? */
			if ( keyFound ) return 0;
			keyFound = TRUE;
			
			/* search key */
			for(j=0; j<NB_KEYS; j++) {
				if ( strcmp(toks[i], KKeys[j].name)==0 ) {
				    keyCode |= KKeys[j].code;
					break;
				}
			}
			
			/* key name ot found ? */
			if ( j==NB_KEYS ) return 0;
		}
	}
	
	return keyCode;
}
