// -*- c++ -*-

// $Id$

#ifndef DEPTHDIALOG_H
#define DEPTHDIALOG_H

#include <qdialog.h>
#include <qcombo.h>
#include <qlabel.h>
#include "canvas.h"

class depthDialog : public QDialog
{
    Q_OBJECT

public:
    depthDialog(Canvas *, QWidget *parent= 0, const char *name= 0);
    QComboBox *depthBox;
};

#endif // DEPTHDIALOG_H
