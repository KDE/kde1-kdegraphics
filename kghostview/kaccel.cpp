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
	bEnabled = true;
	pAccel = new QAccel( parent, name );
}

KAccel::~KAccel()
{
}

void KAccel::clear()
{
	pAccel->clear();
	aKeyDict.clear();
}

void KAccel::connectItem( const char * action,
							const QObject* receiver, const char* member,
							bool activate )
{
    KKeyEntry *pEntry = aKeyDict[ action ];
	if ( !pEntry ) {
		QString str;
		str.sprintf(
			"KAccel : Cannot connect action %s"\
			"which is not in the object dictionary", action );
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

uint KAccel::count() const
{
	return aKeyDict.count();
}

uint KAccel::currentKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
		return 0;
	else
		return pEntry->aCurrentKeyCode;
}

uint KAccel::defaultKey( const char * action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
        return 0;
    else
        return pEntry->aDefaultKeyCode;
}

void  KAccel::disconnectItem( const char * action,
							const QObject* receiver, const char* member )
{
    KKeyEntry *pEntry = aKeyDict[ action ];
    if ( !pEntry ) 
		return;
	
	pAccel->disconnectItem( pEntry->aAccelId, receiver, member  );
}

const char * KAccel::findKey( int key ) const
{
	QDictIterator<KKeyEntry> aKeyIt( aKeyDict );
	aKeyIt.toFirst();
#define pE aKeyIt.current()
	while ( pE ) {
		if ( key == pE->aCurrentKeyCode ) return aKeyIt.currentKey();
		++aKeyIt;
	}
#undef pE
	return 0;	
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
	
	return TRUE;
}

bool KAccel::insertItem( const char * action, 
					   const char * keyCode, bool configurable )
{
	uint iKeyCode = stringToKey(keyCode);
	if ( iKeyCode==0 ) {
		QString str;
		str.sprintf(
			"KAccel : cannot insert item with invalid key string %s", keyCode );
		warning(str);
		return FALSE;
	}
	
	return insertItem(action, iKeyCode, configurable);
}

const char * KAccel::insertStdItem( int id )
{
	QString action, key;
	switch( id ) {
		case Open:
			action.sprintf( i18n("Open") );
			key.sprintf( i18n("CTRL+O") );
			break;
		case New:
			action.sprintf( i18n("New") );
			key.sprintf( i18n("CTRL+N") );
			break;
		case Close:
			action.sprintf( i18n("Close") );
			key.sprintf( i18n("CTRL+W") );
			break;
		case Save:
			action.sprintf( i18n("Save") );
			key.sprintf( i18n("CTRL+S") );
			break;
		case Print:
			action.sprintf( i18n("Print") );
			key.sprintf( i18n("CTRL+P") );
			break;
		case Quit:
			action.sprintf( i18n("Quit") );
			key.sprintf( i18n("CTRL+Q") );
			break;
		case Cut:
			action.sprintf( i18n("Cut") );
			key.sprintf( i18n("CTRL+X") );
			break;
		case Copy:
			action.sprintf( i18n("Copy") );
			key.sprintf( i18n("CTRL+C") );
			break;
		case Paste:
			action.sprintf( i18n("Paste") );
			key.sprintf( i18n("CTRL+V") );
			break;
		case Undo:
			action.sprintf( i18n("Undo") );
			key.sprintf( i18n("CTRL+Z") );
			break;
		case Find:
			action.sprintf( i18n("Find") );
			key.sprintf( i18n("CTRL+F") );
			break;
		case Replace:
			action.sprintf( i18n("Replace") );
			key.sprintf( i18n("CTRL+R") );
			break;
		case Insert:
			action.sprintf( i18n("Insert") );
			key.sprintf( i18n("CTRL+Insert") );
			break;
		case Home:
			action.sprintf( i18n("Home") );
			key.sprintf( i18n("CTRL+Home") );
			break;
		case End:
			action.sprintf( i18n("End") );
			key.sprintf( i18n("CTRL+End") );
			break;
		case Prior:
			action.sprintf( i18n("Prior") );
			key.sprintf( i18n("Prior") );
			break;
		case Next:
			action.sprintf( i18n("Next") );
			key.sprintf( i18n("Next") );
			break;
		case Help:
			action.sprintf( i18n("Help") );
			key.sprintf( i18n("F1") );
			break;
		default:
			return 0;
			break;
	}
	insertItem( action.data(), key.data(), false );
	return action.data();
}

bool KAccel::isEnabled()
{
	return bEnabled;
}

bool KAccel::isItemEnabled( const char *action )
{
	KKeyEntry *pEntry = aKeyDict[ action ];
	
	if ( !pEntry )
        return false;
    else
        return pEntry->bEnabled;
}

QDict<KKeyEntry> KAccel::keyDict()
{
	return aKeyDict;
}

void KAccel::readSettings()
{
	KConfig *pConfig = kapp->getConfig();
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

void KAccel::removeItem( const char * action )
{
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

void KAccel::setEnabled( bool activate )
{
	QDictIterator<KKeyEntry> aKeyIt( aKeyDict );
	aKeyIt.toFirst();
#define pE aKeyIt.current()
	while ( pE ) {
		setItemEnabled( aKeyIt.currentKey(), activate );
		++aKeyIt;
	}
#undef pE
	bEnabled = activate;
}

void KAccel::setItemEnabled( const char * action, bool activate )
{	
    KKeyEntry *pEntry = aKeyDict[ action ];
	if ( !pEntry ) {
		QString str;
		str.sprintf( 
			"KGlobalAccel : cannont enable action %s"\
			"which is not in the object dictionary", action );
		warning( str );
		return;
	}

	pAccel->setItemEnabled( pEntry->aAccelId, activate );
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

const char * KAccel::stdAction( int id ) {
	QString action;
	switch( id ) {
		case Open:
			action.sprintf( i18n("Open") );
			break;
		case New:
			action.sprintf( i18n("New") );
			break;
		case Close:
			action.sprintf( i18n("Close") );
			break;
		case Save:
			action.sprintf( i18n("Save") );
			break;
		case Print:
			action.sprintf( i18n("Print") );
			break;
		case Quit:
			action.sprintf( i18n("Quit") );
			break;
		case Cut:
			action.sprintf( i18n("Cut") );
			break;
		case Copy:
			action.sprintf( i18n("Copy") );
			break;
		case Paste:
			action.sprintf( i18n("Paste") );
			break;
		case Undo:
			action.sprintf( i18n("Undo") );
			break;
		case Find:
			action.sprintf( i18n("Find") );
			break;
		case Replace:
			action.sprintf( i18n("Replace") );
			break;
		case Insert:
			action.sprintf( i18n("Insert") );
			break;
		case Home:
			action.sprintf( i18n("Home") );
			break;
		case End:
			action.sprintf( i18n("End") );
			break;
		case Prior:
			action.sprintf( i18n("Prior") );
			break;
		case Next:
			action.sprintf( i18n("Next") );
			break;
		case Help:
			action.sprintf( i18n("Help") );
			break;
		default:
			return 0;
			break;
	}
	return action.data();
}

void KAccel::writeSettings()
{
	KConfig *pConfig = kapp->getConfig();
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
