#ifndef KDRAGSOURCE_H
#define KDRAGSOURCE_H

#include <qdragobject.h>
#include <qlabel.h>
#include <kurl.h>
#include <qimage.h>
#include <qstring.h>
#include <qstrlist.h>

class KURLDrag: public QStoredDrag {
    Q_OBJECT
public:
    KURLDrag( QStrList urls,
	       QWidget * dragSource = 0, const char * name = 0 );
    KURLDrag( QWidget * dragSource = 0, const char * name = 0 );
    ~KURLDrag();

    void setURL( QStrList urls );

    static bool canDecode( QDragMoveEvent* e );
    static bool decode( QDropEvent* e, QString& s );
};

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
  QObject *provider;
};

#endif
