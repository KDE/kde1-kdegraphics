/****************************************************************************
** $Id$
**
** Definition of QPushButton class
**
** Created : 940221
**
** Copyright (C) 1992-1996 Troll Tech AS.  All rights reserved.
**
** This file is part of the non-commercial distribution of Qt 1.1.
**
** See the file LICENSE included in the distribution for the usage
** and distribution terms, or http://www.troll.no/qt/license.html.
**
** (This file differs from the one in the commercial distribution of
** Qt only by this comment.)
**
*****************************************************************************/

#ifndef KPUSHBT_H
#define KPUSHBT_H

#include <qpushbt.h>


class KPushButton : public QPushButton
{
    Q_OBJECT
public:
    KPushButton(  const char *name=0, QWidget *parent=0);

    void	drawButton( QPainter * );
  
};


#endif // KPUSHBT_H
