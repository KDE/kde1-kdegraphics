/*
	KViewError : Displays error dialogs for kview errors
			and lists 

	$Revision$
*/

#ifndef _SSK_KVERROR_H
#define _SSK_KVERROR_H

#include <qobject.h>
///
class KViewError : public QObject{

	Q_OBJECT

private:
static	const char *errormsgs[8];

public:

///
	enum Type { unknown, 
			badFileName, 
			accessDenied,
			badFormat, 
			badURL,
			badRemoteFile,
			ioJobRunning,
			noKFM
	 };

///
	static bool message( Type err);

/**@name slots */
//@{

public slots:
///
	void showMessage(KViewError::Type err);
//@}

};

#endif
