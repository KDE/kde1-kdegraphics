#include <qpushbt.h>
#include <klocale.h>
#include "palettedialog.h"

#define klocale KLocale::klocale()

paletteDialog::paletteDialog(QPixmap *p, QWidget *parent= 0, const char *name= 0)
  : QDialog(parent, name, TRUE)
{
  QPushButton *dismiss;
  pal= new paletteWidget(p, this);

  resize(340, 280);
  dismiss = new QPushButton( klocale->translate("Dismiss"), this );
  dismiss->setAutoResize(TRUE);
  pal->move(10, 10);
  pal->resize(320, 240);
  dismiss->move(140, 250);
  connect( dismiss, SIGNAL(clicked()), SLOT(accept()) );
  connect( pal, SIGNAL(colourSelected(int)), pal, SLOT(editColour(int)) );
}

QPixmap *paletteDialog::pixmap(void)
{
  return pal->pixmap();
}

#include "palettedialog.moc"
