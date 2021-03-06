// -*- c++ -*-

#ifndef FORMATS_H
#define FORMATS_H

#include <qimage.h>
#include <qlist.h>
#include <qstrlist.h>

// This file defines the format records for the supported file formats
// and the manager to install them

class FormatRecord {
public:
  const char *formatName;
  unsigned int flags; // Internal? / Read? / Write?
  char *magic; // NULL for formats kpaint should not register
  char *glob;
  char *suffix; // Default suffix for this type
  image_io_handler read_format; // NULL for internal formats
  image_io_handler write_format; // NULL for internal formats

  // Constants for flags
  static const unsigned int InternalFormat= 1;
  static const unsigned int ReadFormat= 2;
  static const unsigned int WriteFormat= 4;
};

class FormatManager {
 public:
   FormatManager(void);
   virtual ~FormatManager();
   
   const char *allImagesGlob(void);
   QStrList *formats(void);
   const char *glob(const char *format);
   const char *suffix(const char *format);
   // const char *description(const char *format);
   // const unsigned int flags(const char *format);
   
 protected:
   QList <FormatRecord> list;
   virtual void init(FormatRecord formatlist[]);
   QStrList names;
   QString globAll;
};

#endif // FORMATS_H
