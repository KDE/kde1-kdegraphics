/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include "debug.h"
#include "kicongrid.h"
#include "main.h"


KIconEditGrid::KIconEditGrid(QWidget *parent, const char *name)
 : KColorGrid(parent, name, 1)
{
  initMetaObject();

  selected = 0;

  //setBackgroundMode(NoBackground);
  //kdeColor(1);

  for(uint i = 0; i < 42; i++)
    iconcolors.append(iconpalette[i]);

  setupImageHandlers();
  debug("Formats: %d", formats->count());
  btndown = isselecting = ispasting = modified = false;
  //modified = false;
  img = new QImage(32, 32, 32);
  img->fill(TRANSPARENT);
  //numcolors = 0;
  currentcolor = qRgb(0,0,0)|OPAQUE_MASK;

  debug("Mouse tracking: %d", hasMouseTracking());
  setMouseTracking(true);
  debug("Mouse tracking: %d", hasMouseTracking());

  setNumRows(32);
  setNumCols(32);
  fill(TRANSPARENT);

  connect( kapp->clipboard(), SIGNAL(dataChanged()), SLOT(checkClipboard()));

  // this is kind of a hack but the application global clipboard object
  // doesn't always send the signals.
  QTimer *cbtimer = new QTimer(this);
  connect( cbtimer, SIGNAL(timeout()), SLOT(checkClipboard()));
  cbtimer->start(2500);

  createCursors();
}

KIconEditGrid::~KIconEditGrid()
{
  debug("KIconEditGrid - destructor: done");
}

void KIconEditGrid::paintCell( QPainter *painter, int row, int col )
{
  //KColorGrid::paintCell(painter, row, col);
  //debug("KIconEditGrid::paintCell()");
  //bool tp = colorAt( row * numCols() + col ) == TRANSPARENT;
  QBrush brush(colorAt( row * numCols() + col ));
  int s = cellSize();

  if(cellSize() == 1)
  {
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawPoint(0, 0);
      return;
    }
    else
    {
      painter->setPen(colorAt(row * numCols() + col));
      painter->drawPoint(0, 0);
    }
  }
  else
  {
    if(hasGrid())
    {
      painter->setPen(black);
      painter->drawLine(1, s, s, s);
      painter->drawLine(s, s, s, 1);
      //if(!tp)
        painter->fillRect(1, 1, s-1, s-1, brush);
      //qDrawPlainRect( painter, 0, 0, s, s, black, 1, &brush);
    }
    else //if(!tp)
      painter->fillRect(0, 0, s, s, brush);
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawWinFocusRect( 1, 1, s-1, s-1);
      return;
    }
  }

  switch( tool )
  {
    case Find:
    case Spray:
    case Eraser:
    case Freehand:
      if ( row * numCols() + col == selected)
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1 );
        else
          painter->drawPoint(0, 0);
      }
      break;
    //case Select:
    case FilledRect:
    case Rect:
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    case Line:
      if(btndown && isMarked(col, row))
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1);
        else
          painter->drawPoint(0, 0);
          //painter->fillRect( 0, 0, s, s, gray);
      }
      break;
    default:
      break;
  }

  //debug("KIconEditGrid::paintCell() - done");
}

void KIconEditGrid::setColor( int cell, uint color, bool update)
{
  KColorGrid::setColor(cell, color, update);
}

void KIconEditGrid::mousePressEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  //int cell = row * numCols() + col;

  if(!img->valid(col, row))
    return;

  btndown = true;
  start.setX(col);
  start.setY(row);

  if(ispasting)
  {
    ispasting = false;
    editPaste(true);
  }

  if(isselecting)
  {
    QPointArray a(pntarray.copy());
    pntarray.resize(0);
    drawPointArray(a, Mark);
    emit selecteddata(false);
  }

  switch( tool )
  {
    case Select:
      isselecting = true;
      break;
    default:
      break;
  }

}

void KIconEditGrid::mouseMoveEvent( QMouseEvent *e )
{
  if(!e)
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  int cell = row * numCols() + col;

  QPoint tmpp(col, row);
  if(tmpp == end)
    return;

  if(img->valid(col, row))
  {
    //debug("%d X %d", col, row);
    emit poschanged(col, row);
  }

  if(ispasting && !btndown && img->valid(col, row))
  {
    if( (col + cbsize.width()) > (numCols()-1) )
      insrect.setX(numCols()-insrect.width());
    else
      insrect.setX(col);
    if( (row + cbsize.height()) > (numRows()-1) )
      insrect.setY(numRows()-insrect.height());
    else
      insrect.setY(row);
    insrect.setSize(cbsize);
    //debug("Moving: %d x %d", insrect.width(), insrect.height());
    start = insrect.topLeft();
    end = insrect.bottomRight();
    drawRect(false);
    return;
  }

  if(!img->valid(col, row) || !btndown)
    return;

  end.setX(col);
  end.setY(row);

  if(isselecting)
  {
    drawRect(false);
    return;
  }

  switch( tool )
  {
    case Eraser:
      currentcolor = TRANSPARENT;
    case Freehand:
    {
      setColor( cell, currentcolor );
  //img->setPixel(col, row, currentcolor.pixel());

      if ( selected != cell )
      {
        modified = true;
        int prevSel = selected;
        selected = cell;
        updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = (colorAt(cell));
      }
      break;
    }
    case Find:
    {
      iconcolors.closestMatch(colorAt(cell));
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        updateCell( row, col, FALSE );
      }
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(false);
      break;
    }
    //case Select:
    case FilledRect:
    case Rect:
    {
      drawRect(false);
      break;
    }
    case Line:
    {
      drawLine(false);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      modified = true;
      break;
    }
    default:
      break;
  }

  p = *img;

  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

void KIconEditGrid::mouseReleaseEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  btndown = false;
  end.setX(col);
  end.setY(row);
  int cell = row * numCols() + col;

  switch( tool )
  {
    case Eraser:
      currentcolor = TRANSPARENT;
    case Freehand:
    {
      if(!img->valid(col, row))
        return;
      setColor( cell, currentcolor );
      //if ( selected != cell )
      //{
        //modified = true;
        int prevSel = selected;
        selected = cell;
        updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = colorAt(cell);
        p = *img;
      //}
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(true);
      break;
    }
    case FilledRect:
    case Rect:
    {
      drawRect(true);
      break;
    }
    case Line:
    {
      drawLine(true);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      break;
    }
    case FloodFill:
    {
      QApplication::setOverrideCursor(waitCursor);
      drawFlood(col, row, colorAt(cell));
      QApplication::restoreOverrideCursor();
      repaint(viewRect(), false);
      p = *img;
      break;
    }
    case Find:
    {
      currentcolor = colorAt(cell);
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        updateCell( row, col, FALSE );
      }

      break;
    }
    default:
      break;
  }
  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

QSize KIconEditGrid::sizeHint()
{
  //return QSize(totalWidth(), totalHeight()); // doesn't give expected result
  return QSize(cellSize()*numCols(), cellSize()*numRows()); // neither does this :-(
}

//void KIconEditGrid::setColorSelection( const QColor &color )
void KIconEditGrid::setColorSelection( uint c )
{
  currentcolor = c;
}

void KIconEditGrid::loadBlank( int w, int h )
{
  if(img != 0L)
    delete img;
  img = new QImage(w, h, 32);
  img->fill(TRANSPARENT);
  setNumRows(h);
  setNumCols(w);
  fill(TRANSPARENT);
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
}

void KIconEditGrid::load( QImage *image)
{
  if(image != 0L)
  {
    const QImage *tmp = image;
    img = new QImage(image->convertDepth(32));
    img->setAlphaBuffer(true);
    delete tmp;
  }
  else
  {
    QString msg = i18n("There was an error loading a blank image.\n");
    KMsgBox::message (this, i18n("Error"), msg.data());
    return;
  }

  setNumRows(img->height());
  setNumCols(img->width());

  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
      //uint gray = (qRgb(200, 200, 200) | OPAQUE_MASK);
      //uint bc = (TRANSPARENT | OPAQUE_MASK);
      //if(*l == gray || *l == bc || *l < 0xff000000)  // this is a hack but I couldn't save it as transparent otherwise
      if(*l < 0xff000000)  // this is a hack but I couldn't save it as transparent otherwise
      {
        *l = TRANSPARENT;
      }
      //debug("KIcnGrid::load: %d %%", (((y*ncols)+x)/imgsize) * 100);
      //debug("KIconEditGrid::load: RGB: %d %d %d", qRed(*l), qGreen(*l), qBlue(*l));
      setColor((y*numCols())+x, *l, false);
    }
    //debug("Row: %d", y);
    kapp->processEvents(200);
  }
  updateColors();
  emit changed(pixmap());
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
  repaint(viewRect(), false);
}

const QPixmap &KIconEditGrid::pixmap()
{
  if(!img->isNull())
    p = *img;
  //p.convertFromImage(*img, 0);
  return(p);
}

bool KIconEditGrid::zoom(Direction d)
{
  int f = (d == In) ? (cellSize()+1) : (cellSize()-1);
  QApplication::setOverrideCursor(waitCursor);
  emit scalingchanged(cellSize(), false);
  setCellSize( f );
  QApplication::restoreOverrideCursor();

  emit scalingchanged(cellSize(), true);
  if(d == Out && cellSize() <= 1)
    return false;
  return true;
}

void KIconEditGrid::checkClipboard()
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);
  delete tmp;
  if(ok)
    emit clipboarddata(true);
  else
    emit clipboarddata(false);
}

// the caller *must* delete the image afterwards
const QImage *KIconEditGrid::clipboardImage(bool &ok)
{
  ok = false;
  QString str = kapp->clipboard()->text();
  QBuffer buf(str);
  buf.open(IO_ReadOnly);
  QImageIO iio;
  iio.setIODevice(&buf);
  iio.setFormat("XPM");
  
  if(iio.read() && !iio.image().isNull())
    ok = true;

  buf.close();
  return new QImage(iio.image());
}

void KIconEditGrid::editSelectAll()
{
  start.setX(0);
  start.setY(0);
  end.setX(numCols()-1);
  end.setY(numRows()-1);
  isselecting = true;
  drawRect(false);
  emit newmessage(i18n("All selected"));
}

void KIconEditGrid::editClear()
{
  img->fill(TRANSPARENT);
  fill(TRANSPARENT);
  update();
  modified = true;
  p = *img;
  emit changed(p);
  emit newmessage(i18n("Cleared"));
}

// the returned image *must* be deleted by the caller.
QImage *KIconEditGrid::getSelection(bool cut)
{
  const QRect rect = pntarray.boundingRect();

  if( QT_VERSION >= 140 && !cut)
    return new QImage(img->copy((QRect&)rect));

  int nx = 0, ny = 0, nw = 0, nh = 0;
  rect.rect(&nx, &ny, &nw, &nh);
  debug("KIconEditGrid: Selection Rect: %d %d %d %d", nx, ny, nw, nh);

  QImage *tmp = new QImage(nw, nh, 32);

  QApplication::setOverrideCursor(waitCursor);
  for(int y = 0; y < nh; y++)
  {
    uint *l = ((uint*)img->scanLine(y+ny)+nx);
    uint *cl = (uint*)tmp->scanLine(y);
    for(int x = 0; x < nw; x++, l++, cl++)
    {
      *cl = *l;
      if(cut)
      {
        *l = TRANSPARENT;
        setColor( ((y+ny)*numCols()) + (x+nx), TRANSPARENT, false );
      }
    }
    kapp->processEvents();
  }

  QApplication::restoreOverrideCursor();

  QPointArray a(pntarray.copy());
  pntarray.resize(0);
  drawPointArray(a, Mark);
  emit selecteddata(false);
  if(cut)
  {
    updateColors();
    repaint(rect.x()*cellSize(), rect.y()*cellSize(),
            rect.width()*cellSize(), rect.height()*cellSize(), false);
    p = *img;
    emit changed(p);
    emit colorschanged(numColors(), data());
    emit newmessage(i18n("Selected area cutted"));
  }
  else
    emit newmessage(i18n("Selected area copied"));
  return tmp;
}

void KIconEditGrid::editCopy(bool cut)
{
  QImage *sel = getSelection(cut);

  sel->setAlphaBuffer(true);
  QImage tmp(sel->convertDepth(8));
  tmp.setAlphaBuffer(true);
  QString str = "";
  QBuffer buf(str);
  buf.open(IO_WriteOnly);
  QImageIO iio;
  iio.setImage(tmp);
  iio.setIODevice(&buf);
  iio.setFormat("XPM");
  iio.setDescription("Created by KDE Draw");
  
  if(iio.write())
  {
    QClipboard *cb = kapp->clipboard();
    cb->setText(str.data());
    //debug(str.data());
  }
  else
  {
    QString msg = i18n("There was an copying:\nto the clipboard!");
    KMsgBox::message(this, i18n("Warning"), msg.data(), KMsgBox::EXCLAMATION);
  }
  buf.close();
  delete sel;
  isselecting = false;
}

void KIconEditGrid::editPaste(bool paste)
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);

  if(ok)
  {
    if( (tmp->size().width() > img->size().width()) || (tmp->size().height() > img->size().height()) )
    {
      if(KMsgBox::yesNo(this, i18n("Warning"),
          i18n("The clipboard image is larger than the current image!\nPaste as new image?")) == 1)
      {
        editPasteAsNew();
      }
      delete tmp;
      return;
    }
    else if(!paste)
    {
      ispasting = true;
      cbsize = tmp->size();
      //debug("insrect size: %d x %d", insrect.width(), insrect.height());
      return;
      emit newmessage(i18n("Pasting"));
    }
    else
    {
      QImage *tmp2 = new QImage(tmp->convertDepth(32));
      tmp2->setAlphaBuffer(true);
      debug("KIconEditGrid: Pasting at: %d x %d", insrect.x(), insrect.y());

      QApplication::setOverrideCursor(waitCursor);

      for(int y = insrect.y(), ny = 0; y < numRows(), ny < insrect.height(); y++, ny++)
      {
        uint *l = ((uint*)img->scanLine(y)+insrect.x());
        uint *cl = (uint*)tmp2->scanLine(ny);
        for(int x = insrect.x(), nx = 0; x < numCols(), nx < insrect.width(); x++, nx++, l++, cl++)
        {
          //debug("img: %d x %d - tmp: %d x %d", x, y, nx, ny);
          //debug("RGB: %d %d %d", qRed((uint)*cl), qGreen(*cl), qBlue(*cl));
          *l = *cl;
          setColor((y*numCols())+x, (uint)*cl, false);
        }
        //kapp->processEvents(250);
      }
      updateColors();
      repaint(insrect.x()*cellSize(), insrect.y()*cellSize(),
              insrect.width()*cellSize(), insrect.height()*cellSize(), false);

      QApplication::restoreOverrideCursor();

      delete tmp2;
      modified = true;
      p = *img;
      emit changed(QPixmap(p));
      emit sizechanged(numCols(), numRows());
      emit colorschanged(numColors(), data());
      emit newmessage(i18n("Done pasting"));
    }
  }
  else
  {
    QString msg = i18n("Invalid pixmap data in clipboard!\n");
    KMsgBox::message(this, i18n("Warning"), msg.data());
  }
  delete tmp;
}

void KIconEditGrid::editPasteAsNew()
{
  bool ok = false;
  const QImage *tmp = clipboardImage(ok);

  if(ok)
  {
    if(img)
      delete img;
    img = new QImage(*tmp);
    img->detach();
    load(img);
    modified = true;
    repaint(viewRect(), false);

    p = *img;
    emit changed(QPixmap(p));
    emit sizechanged(numCols(), numRows());
    emit colorschanged(numColors(), data());
    emit newmessage(i18n("Done pasting"));
  }
  else
  {
    QString msg = i18n("Invalid pixmap data in clipboard!\n");
    KMsgBox::message(this, i18n("Warning"), msg.data());
  }
  delete tmp;
}

//#if QT_VERSION >= 140
void KIconEditGrid::editResize()
{
  debug("KIconGrid::editResize");
#if QT_VERSION >= 140
  KResize *rs = new KResize(this, 0, true, QSize(numCols(), numRows()));
  if(rs->exec())
  {
    const QSize s = rs->getSize();
    QImage *tmp = img;
    img = new QImage(img->smoothScale(s.width(), s.height()));
    load(img);

    delete tmp;
    modified = true;
  }
  delete rs;
#endif
}
//#endif

void KIconEditGrid::setSize(const QSize s)
{
  debug("::setSize: %d x %d", s.width(), s.height());
  QImage *tmp = img;
  img = new QImage(s.width(), s.height(), 32);
  img->fill(TRANSPARENT);
  load(img);
  delete tmp;
  
}

void KIconEditGrid::createCursors()
{
  KIconLoader *loader = kapp->getIconLoader();
  QBitmap bmp, mask(22, 22);
  QPixmap pix;

  cursor_normal = QCursor(arrowCursor);

  pix = loader->loadIcon("paintbrush.xpm");
  if(pix.isNull())
  {
    cursor_paint = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: paintbrush.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_paint = QCursor(bmp, mask, 6, 18);
  }

  pix = loader->loadIcon("flood.xpm");
  if(pix.isNull())
  {
    cursor_flood = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: flood.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_flood = QCursor(bmp, mask, 5, 16);
  }

  pix = loader->loadIcon("aim.xpm");
  if(pix.isNull())
  {
    cursor_aim = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask();
    bmp = pix;
    cursor_aim = QCursor(bmp, mask);
  }

  pix = loader->loadIcon("spraycan.xpm");
  if(pix.isNull())
  {
    cursor_spray = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    bmp = pix;
    cursor_spray = QCursor(bmp, mask);
  }

  pix = loader->loadIcon("eraser.xpm");
  if(pix.isNull())
  {
    cursor_erase = cursor_normal;
    debug("KIconEditGrid: Error loading pixmap: aim.xpm"); 
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    bmp = pix;
    cursor_erase = QCursor(bmp, mask, 5, 16);
  }
}

void KIconEditGrid::setTool(DrawTool t)
{
  btndown = false;
  tool = t;

  if(tool != Select)
    isselecting = false;

  switch( tool )
  {
    case Select:
      isselecting = true;
    case Line:
      //KMsgBox::message(this, i18n("Warning"), i18n("Sorry - not implemented."));
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    case FilledRect:
    case Rect:
      setCursor(cursor_aim);
      break;
    case Freehand:
      setCursor(cursor_paint);
      break;
    case Spray:
      setCursor(cursor_spray);
      break;
    case Eraser:
      setCursor(cursor_erase);
      currentcolor = TRANSPARENT;
      break;
    case FloodFill:
      setCursor(cursor_flood);
      break;
    case Find:
      debug("Tool == Find");
      setCursor(arrowCursor);
      debug("Tool == Find");
      break;
    default:
      break;
  }
}

void KIconEditGrid::drawFlood(int x, int y, uint oldcolor)
{
  if((!img->valid(x, y)) || (colorAt((y * numCols())+x) != oldcolor) || (colorAt((y * numCols())+x) == currentcolor))
    return;

  *((uint*)img->scanLine(y) + x) = currentcolor;
  setColor((y*numCols())+x, currentcolor, false);

  drawFlood(x, y-1, oldcolor);
  drawFlood(x, y+1, oldcolor);
  drawFlood(x-1, y, oldcolor);
  drawFlood(x+1, y, oldcolor);

}

void KIconEditGrid::drawSpray(QPoint point)
{
  int x = (point.x()-5);
  int y = (point.y()-5);
  //debug("drawSpray() - %d X %d", x, y);
  
  pntarray.resize(0);
  int points = 0;
  for(int i = 1; i < 4; i++, points++)
  {
    int dx = (rand() % 10);
    int dy = (rand() % 10);
    pntarray.putPoints(points, 1, x+dx, y+dy);
  }

  drawPointArray(pntarray, Draw);
}

void KIconEditGrid::drawEllipse(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint(pntarray.boundingRect());
    return;
  }

  QPointArray a(pntarray.copy());
  int x = start.x(), y = start.y(), cx, cy;
  //cx = (x > end.x()) ? (x - end.x()) : (end.x() - x);
  if(x > end.x())
  {
    cx = x - end.x();
    x = x - cx;
  }
  else
    cx = end.x() - x;
  if(y > end.y())
  {
    cy = y - end.y();
    y = y - cy;
  }
  else
    cy = end.y() - y;
  //cy = (y > end.y()) ? (y - end.y()) : (end.y() - y);
  int d = (cx > cy) ? cx : cy;

  //debug("%d, %d - %d %d", x, y, d, d);
  pntarray.resize(0);
  drawPointArray(a, Mark);
  //repaint();
  if(tool == Circle || tool == FilledCircle)
    pntarray.makeEllipse(x, y, d, d);
  else if(tool == Ellipse || tool == FilledEllipse)
    pntarray.makeEllipse(x, y, cx, cy);

  if((tool == FilledEllipse) || (tool == FilledCircle))
  {
    int s = pntarray.size();
    int points = s;
    for(int i = 0; i < s; i++)
    {
      int x = pntarray[i].x();
      int y = pntarray[i].y();
      for(int j = 0; j < s; j++)
      {
         if((pntarray[j].y() == y) && (pntarray[j].x() > x))
         {
           for(int k = x; k < pntarray[j].x(); k++, points++)
             pntarray.putPoints(points, 1, k, y);
           break;
         }
      }
    }
  }

  drawPointArray(pntarray, Mark);
}

void KIconEditGrid::drawRect(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint();
    return;
  }

  QPointArray a(pntarray.copy());
  int x = start.x(), y = start.y(), cx, cy;
  //cx = (x > end.x()) ? (x - end.x()) : (end.x() - x);
  if(x > end.x())
  {
    cx = x - end.x();
    x = x - cx;
  }
  else
    cx = end.x() - x;
  if(y > end.y())
  {
    cy = y - end.y();
    y = y - cy;
  }
  else
    cy = end.y() - y;

  //debug("%d, %d - %d %d", x, y, cx, cy);
  pntarray.resize(0);
  drawPointArray(a, Mark); // remove previous marking

  int points = 0;

  if(tool == FilledRect)
  {
    for(int i = x; i < x+cx; i++)
    {
      for(int j = y; j < y+cy; j++, points++)
        pntarray.putPoints(points, 1, i, j);
    }
  }
  else
  {
    for(int i = x; i <= x+cx; i++, points++)
      pntarray.putPoints(points, 1, i, y);
    for(int i = y; i <= y+cy; i++, points++)
      pntarray.putPoints(points, 1, x, i);
    for(int i = x; i <= x+cx; i++, points++)
      pntarray.putPoints(points, 1, i, y+cy);
    for(int i = y; i <= y+cy; i++, points++)
      pntarray.putPoints(points, 1, x+cx, i);
  }

  drawPointArray(pntarray, Mark);
  if(tool == Select && pntarray.size() > 0 && !ispasting)
    emit selecteddata(true);
}

void KIconEditGrid::drawLine(bool drawit)
{
  if(drawit)
  {
    drawPointArray(pntarray, Draw);
    p = *img;
    emit changed(p);
    //repaint();
    return;
  }

  QPointArray a(pntarray.copy());
  pntarray.resize(0);
  drawPointArray(a, Mark); // remove previous marking

  int x, y, dx, dy, delta; // = start.x(), y = start.y(), cx = end.x(), cy = end.y();
  dx = end.x() - start.x();
  dy = end.y() - start.y();
  x = start.x();
  y = start.y();

  delta = QMAX(abs(dx), abs(dy));
  if (delta > 0) {
    dx /= delta;
    dy /= delta;
    for(int i = 0; i <= delta; i++) {
      pntarray.putPoints(i, 1, x, y);
      x += dx;
      y += dy;
    }
  }
  drawPointArray(pntarray, Mark);
}

void KIconEditGrid::drawPointArray(QPointArray a, DrawAction action)
{
  QRect rect = a.boundingRect();
  bool update = false;

  int s = a.size(); //((rect.size().width()) * (rect.size().height()));
  for(int i = 0; i < s; i++)
  {
    int x = a[i].x();
    int y = a[i].y();
    //if(img->valid(x, y) && !QSize(x, y).isNull() && rect.contains(QPoint(x, y)))
    if(img->valid(x, y) && rect.contains(QPoint(x, y)))
    {
      //debug("x: %d - y: %d", x, y);
      switch( action )
      {
        case Draw:
        {
          *((uint*)img->scanLine(y) + x) = currentcolor; //colors[cell]|OPAQUE;
          int cell = y * numCols() + x;
          setColor( cell, currentcolor, false );
          modified = true;
          update = true;
          //updateCell( y, x, FALSE );
          break;
        }
        case Mark:
        case UnMark:
          updateCell( y, x, true );
          break;
        default:
          break;
      }
    }
  }
  if(update)
  {
    updateColors();
    repaint(rect.x()*cellSize()-1, rect.y()*cellSize()-1,
        rect.width()*cellSize()+1, rect.height()*cellSize()+1, false);
    pntarray.resize(0);
  }

}

bool KIconEditGrid::isMarked(QPoint point)
{
  return isMarked(point.x(), point.y());
}

bool KIconEditGrid::isMarked(int x, int y)
{
  if(((y * numCols()) + x) == selected)
    return true;

  int s = pntarray.size();
  for(int i = 0; i < s; i++)
  {
    if(y == pntarray[i].y() && x == pntarray[i].x())
      return true;
  }

  return false;
}

void KIconEditGrid::mapToKDEPalette()
{
#if QT_VERSION > 140
  *img = img->convertDepthWithPalette(32, iconpalette, 42);
  load(img);
  return;
#endif

  QApplication::setOverrideCursor(waitCursor);
  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
      if(*l != TRANSPARENT)
      {
        if(!iconcolors.contains(*l))
          *l = iconcolors.closestMatch(*l);
      }
    }
  }
  load(img);
  modified = true;
  QApplication::restoreOverrideCursor();
}

void KIconEditGrid::grayScale()
{
  for(int y = 0; y < numRows(); y++)
  {
    uint *l = (uint*)img->scanLine(y);
    for(int x = 0; x < numCols(); x++, l++)
    {
      if(*l != TRANSPARENT)
      {
        uint c = (qBlue(*l) + qRed(*l) + qGreen(*l))/3;
        *l = QColor(c, c, c).rgb()|OPAQUE_MASK;
      }
    }
  }
  load(img);
  modified = true;
}


