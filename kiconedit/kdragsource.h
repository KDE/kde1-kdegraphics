#ifndef KDRAGSOURCE_H
#define KDRAGSOURCE_H

#include <qdragobject.h>
#include <qlabel.h>
#include <kurl.h>
#include <qimage.h>
#include <qstring.h>

class KDragSource: public QLabel
{
  Q_OBJECT

public:
  KDragSource( const char *dragtype, QObject *provider, const char *method,
                   QWidget *parent = 0, const char * name = 0 );
  ~KDragSource();

signals:
  void getimage(QImage*);
  void gettext(QString);
  void geturl(KURL);

protected:
  void mousePressEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );
  KURL url;
  QString type;
  QString text;
  bool ok;
};

#endif
