// $Id$

#ifndef IMAGEINFODIALOG_H
#define IMAGEINFODIALOG_H

#include <qdialog.h>
#include <qcombo.h>
#include <qlabel.h>
#include "canvas.h"

class imageInfoDialog : public QDialog
{
    Q_OBJECT

public:

    imageInfoDialog(Canvas *c, QWidget* parent = NULL, const char* name = NULL);

private:
    QLabel* filename;
    QLabel* colourDepth;
    QLabel* coloursUsed;
    QComboBox* fileformat;
    QLabel* width;
    QLabel* height;
};

#endif // IMAGEINFODIALOG_H
