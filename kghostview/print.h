#ifndef PRINT_H
#define PRINT_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qcombo.h>

class PrintDialog : public QDialog {
	Q_OBJECT
public:

	PrintDialog( QWidget *parent, const char *name );
	QLineEdit* nameLineEdit;
	QComboBox* modeComboBox;
	
public slots:

};

#endif
