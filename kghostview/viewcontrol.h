#ifndef VIEWCONTROL_H
#define VIEWCONTROL_H

#include <qdialog.h>
#include <qcombo.h>

class ViewControl : public QDialog {
	Q_OBJECT
public:
	ViewControl( QWidget *parent, const char *name );
	QComboBox* mediaComboBox;
	QComboBox* magComboBox;
	QComboBox* orientComboBox;
};

#endif
