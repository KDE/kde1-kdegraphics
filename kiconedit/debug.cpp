
#include "debug.h"

void myMessageOutput( QtMsgType type, const char *msg )    
{
  switch ( type )
  {
     case QtDebugMsg:
#ifdef DEBUG
       fprintf( stderr, "Debug: %s\n", msg );
#endif
       break;
     case QtWarningMsg:
       fprintf( stderr, "Warning: %s\n", msg );
       break;
     case QtFatalMsg:
       fprintf( stderr, "Fatal: %s\n", msg );
       abort();
       // dump core on purpose
  }
}


