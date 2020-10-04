#include <qpainter.h>
#include <qimage.h>
#include <kdebug.h>
#include "navwidget.h"

NavWidget::NavWidget(QWidget *parent, const char *name) 
  : QWidget(parent, name)
{

}

NavWidget::~NavWidget() 
{

}

void NavWidget::paintEvent(QPaintEvent *)
{
  double xfactor, yfactor;
  QPainter p;

  p.begin(this);
  p.drawImage(0, 0, view->convertToImage().smoothScale(width(), height()));
  p.end();
}

void NavWidget::sizeChanged()
{
  repaint(FALSE);
}

void NavWidget::setPixmap(QPixmap *p)
{
  view= p;
  repaint(FALSE);
}

void NavWidget::pixmapChanged()
{
  KDEBUG(KDEBUG_INFO, 3000, "NavWidget::pixmapChanged()\n");
  repaint(FALSE);
}

void NavWidget::viewportChanged(int, int, int, int)
{
  // Use this to draw the view rectangle
}

#include "navwidget.moc"
