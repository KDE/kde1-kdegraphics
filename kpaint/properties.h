#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <qtabdlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qcombo.h>
#include <qbrush.h>
#include <qpen.h>


class fillPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  fillPropertiesWidget(QWidget *parent);
  QBrush *getBrush(void);

public slots:
    void setFillColour();
private:
  QComboBox *patternBox;
  QLabel *patternLabel;
  QLabel *customPatternLabel;
  QLineEdit *customPatternField;
  QPushButton *browseButton;
  QPushButton *setFillColourButton;
   QColor fillColour;
};


class linePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  linePropertiesWidget(QWidget *parent);
  QPen *getPen(void);
public slots:
    void setLineColour();
private:
  QComboBox *styleBox;
  QLabel *styleLabel;
  QComboBox *widthBox;
  QLabel *widthLabel;
  QComboBox *endTypeBox;
  QLabel *endTypeLabel;
  QComboBox *joinStyleBox;
  QLabel *joinStyleLabel;
  QPushButton *setLineColourButton;
   QColor lineColour;
};

class propertiesDialog  : public QTabDialog
{
public:
  propertiesDialog(QWidget *parent, const char *name);

  QBrush *getBrush(void);
  QPen *getPen(void);

private:
  linePropertiesWidget *lineProps;
  fillPropertiesWidget *fillProps;
};

#endif // PROPERTIES_H
