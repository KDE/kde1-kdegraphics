/* This file is part of the KDE libraries
	Copyright (C) 1998 Jani Jaakkola (jjaakkol@cs.helsinki.fi),

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

// TODO: documentation, global configure files
// implement some unimplemented stuff in kglobalshortcut.cpp

#ifndef _KGLOBALACCEL_H_
#define _KGLOBALACCEL_H_

#include <kapp.h>
#include <qobject.h>
#include "kaccel.h"

/** 
 * Return the keycode corresponding to the given string. The string must
 *  be something like "SHIFT+A" or "F1+CTRL+ALT" or "Backspace"
 *	for instance.
 *	Return 0 if the string is not recognized.
 */	
uint keyToXMod( uint keyCode );
uint keyToXSym( uint keyCode );


/** 
 * The KGlobalAccel class allows the easy management of "function/key"
 *  associations. It allows the user to configure (configurable) keys via
 *  the config file or via a dialog window.
 *
 *  There is three differents levels :
 *  \begin{itemize}
 * 	\item the "action" : it is just a string which represents an action
 *   which can be handle via different methods in different widgets. 
 *  \item the "keyCode" : it represents a key + modifiers (SHIFT, CTRL
 *  or/and ALT).
 *  \item the "widgetName" : each widget using keys should have such a name.
 *  \end{itemize}
 * 	 
 * How it works :
 *  \begin{itemize}
 *  \item Each "action" is associated with exactly one "keyCode" :
 *  insertItem( action, keyCode )
 *  \item Each widget is registered with a "widgetName" :
 *  registerWidget( widgetName, this )
 *  \item For a given widget you can connect a "action/keyCode"
 *  association to a SLOT :
 *  connectItem( widgetName, action, receiver, SLOT(...) )
 *  \item Then KGlobalAccel manages automatically the keypress.
 * 	\end{itemize}
 *
 * @version $Id$
 * @short Allows easy management of function/accelerator association.
*/
class KGlobalAccel : public QObject
{
	Q_OBJECT
	
 public:
	/** 
	 * Construct a KGlobalAccel object. 
	 * Do not use directly this constructor; use initKGlobalAccel() instead. 
	 */
	KGlobalAccel( );
			
	/** 
	 * Call sync() and destroy the KGlobalAccel object. 
	 */
	~KGlobalAccel();
	
	bool x11EventFilter(const XEvent *);
	bool grabKey(uint keysym, uint mod);

	QDict<KKeyEntry> * keyDict();
	
	bool setKeyDict( QDict<KKeyEntry> nKeyDict );

	/** 
	* Read the current key code associated with the function. 
	*/
	uint currentKey( const char * action );
	
	/** 
	* Read the default key code associated with the function. 
	*/
	uint defaultKey( const char * action);
	
	/** 
	 * Add a "action/keyCode" association.
	 * 	 
	 * If "configurable" is set : search the KConfig object for a 
	 * value. If such a value exists, it is taken as the current key.
	 *
	 * NB : adding a preexisting function will remove the old association
	 *      and eventually the old connections.
	 * 	 
	 * @return FALSE if the keyCode is not correct.
	 */
	bool insertItem( const char * action, uint defaultKeyCode,
				 bool configurable = TRUE );
	
	/** 
	 * Add a "action/keyCode" association.
	 * 	 
	 * If "configurable" is set : search the KConfig object for a 
	 * value. If such a value exists, it is taken as the current key.
	 *
	 * NB : adding a preexisting function will remove the old association
	 *      and eventually the old connections.
	 * 	 
	 * @return FALSE if the keyCode is not correct.
	 */
	bool insertItem( const char * action, const char * defaultKeyCode,
				 bool configurable = TRUE );
		
 	/** 
	 * Remove a "action/keyCode" association.
	 * 	 
	 * NB : it will remove the eventual connections
     */
    void removeItem( const char * action );  
	
	/** 
	 * Connect the action to a slot for a specific widget. 
	 */
	void connectItem( const char * action,
						  const QObject* receiver, const char* member,
						  bool activate = TRUE );
	
	/** 
	 * Activate/disactivate a connection. 
	 */
	void setItemEnabled( const char * action, bool activate );
	
	/** 
	 * Remove the connection for a specific widget. 
	 */
	void disconnectItem( const char * action,
							const QObject* receiver, const char* member );
	
	/** 
	 * Write the current configurable associations in the KConfig object. 
	 */
	//void sync();
	
	void writeSettings();
	void readSettings();
	
	/** 
	 * Create a dialog showing all the associations and allow the user
	 * to configure the configurable ones. On OK it will use the new values,
	 * save them in the KConfig object and return TRUE.
	 * Nothing is change on CANCEL.
	 */
	//bool configureKeys( QWidget *parent );
 signals:
	void activated();	
 private:
	int aAvailableId;
	QDict<KKeyEntry> aKeyDict;

 protected:
};

#endif _KGLOBALACCEL_H_
