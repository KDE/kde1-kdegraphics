#include <unistd.h>
#include <stdio.h>
#include <qimage.h>

#include "eps.h"

#define OK (1==1)
#define FAIL (1==0)

#define CMDBUFLEN   4096
#define BUFLEN 200
char buf[BUFLEN+1];

#define BBOX "%%BoundingBox:"
#define BBOX_LEN strlen(BBOX)

int bbox (const char *fileName, int *x1, int *y1, int *x2, int *y2)
{
  FILE * file;
  int a, b, c, d;
  int ret = FALSE;

  file = fopen (fileName, "r");

  do {
    if (! fgets (buf, BUFLEN, file)) break;
      
    if (strncmp (buf, BBOX, BBOX_LEN) == 0)
    {
      ret = sscanf (buf, "%s %d %d %d %d", buf, &a, &b, &c, &d);    
      if (ret == 5) {
        ret = TRUE;
        break;
      } 
    }  
  } while (1==1);

  *x1 = a;
  *y1 = b;
  *x2 = c;
  *y2 = d;

  fclose (file);
  return ret;
}  

void read_ps_epsf (QImageIO *image)
{
  FILE * file1, * file2;
  char * tmpFileName;
  int x1, y1, x2, y2;

  QImage myimage;

  char cmdBuf [CMDBUFLEN];

  tmpFileName = tmpnam(NULL); 

  if (! bbox (image->fileName(), &x1, &y1, &x2, &y2)) return;
 
  // x1, y1 -> translation
  // x2, y2 -> new size

  x2 -= x1;
  y2 -= y1;
  
  sprintf (cmdBuf, "gs -sOutputFile=%s -q -g%dx%d -dNOPAUSE -sDEVICE=ppm -c 0 0 moveto 1000 0 lineto 1000 1000 lineto 0 1000 lineto 1 1 254 255 div setrgbcolor fill 0 0 0 setrgbcolor - -c showpage quit",
         tmpFileName, x2, y2);    

  file1 = popen (cmdBuf, "w");
  
  if (! file1) return;

  fprintf (file1, "\n%d %d translate\n", -x1, -y1);

  // open src file

  file2 = fopen (image->fileName(), "r");

  if (file2)
  {
    do {
      if (! fgets (buf, BUFLEN, file2)) break;  

      fputs (buf, file1);
    } while (TRUE);

    if (file2) fclose (file2);  
  }
  pclose (file1);

  myimage.load (tmpFileName);
  myimage.createHeuristicMask();

  unlink (tmpFileName);

  image->setImage (myimage);
  image->setStatus (0);

  return;
}
