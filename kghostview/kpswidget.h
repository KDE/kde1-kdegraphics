#ifndef KPSWIDGET_H
#define KPSWIDGET_H

#include <qfiledlg.h>
#include <stdlib.h>
#include <math.h>
#include <qapp.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpaintd.h>
#include <qscrbar.h>
#include <qbitmap.h>
#include <qsocknot.h>
#define	 GC GC_QQQ				// avoid type mismatch
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <ctype.h>


#define SCROLLBAR_SIZE 16

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef None
#define None -1
#endif

#ifndef XlibSpecificationRelease
typedef char *XPointer;
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef XlibSpecificationRelease
typedef char *XPointer;
#endif

#include <signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#include <fcntl.h>
/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define O_NONBLOCK O_NDELAY
#endif

#include <errno.h>
/* BSD 4.3 errno.h does not declare errno */
extern int errno;
/* Both error returns are checked for non-blocking I/O. */
/* Manufacture the other error code if only one exists. */
#if !defined(EWOULDBLOCK) && defined(EAGAIN)
#define EWOULDBLOCK EAGAIN
#endif
#if !defined(EAGAIN) && defined(EWOULDBLOCK)
#define EAGAIN EWOULDBLOCK
#endif

#define NUM_ARGS 100;
#if !defined(BUFSIZ)
#define BUFSIZ 512;
#endif

extern int  num_ghosts;
extern FILE *psfile;
extern QString filename;  
extern QString  oldfilename;   
extern int  current_page;  
extern time_t  mtime; 	  
extern struct document *doc;  
extern struct document *olddoc;
extern Atom gs_colors, ghostview, next, gs_page, done;
extern Bool busy;
extern int  interpreter_input;

extern Display *gs_display;
extern Window gs_window;
extern Window clipWindow;
extern Window mwin;


struct record_list {
    FILE *fp;
    long begin;
    unsigned int len;
    Bool seek_needed;
    Bool close;
    struct record_list *next;
};

class KPSWidget : public QWidget
{
    Q_OBJECT

public:
	KPSWidget(QWidget *parent);
	~KPSWidget();
	
	Bool sendPS(FILE *fp, long begin,unsigned int len,Bool close);
	void layout();
	void disableInterpreter();
	void enableInterpreter();
	Bool isInterpreterReady();
	Bool isInterpreterRunning();
	Bool nextPage();
	
	void scrollLeft();
	void scrollRight();
	void scrollUp();
	void scrollDown();
	void scrollTop();
	
	int orient_angle;
	int orientation;
	Bool	antialias;
	Bool	changed;
	Bool disable_start;
	QString filename;
	int llx, lly, urx, ury;
	int left_margin, right_margin, bottom_margin, top_margin;
	int foreground, background_pixel;
	float xdpi, ydpi;
		
public slots:
  void gs_input();
  void slotVertScroll( int value );
  void slotHorScroll( int value );
  
protected:
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void timerEvent(QTimerEvent *);
	
	void setup();
	void startInterpreter();
	void stopInterpreter();
	void interpreterFailed();
	Bool computeSize();
	
private:
	QSocketNotifier *sn;
	QScrollBar *vertScrollBar;
	QScrollBar *horScrollBar;
	QWidget *fullView;
	
	int clip_x, clip_y, clip_width, clip_height;
	int scroll_x_offset, scroll_y_offset;
	int fullWidth;
	int fullHeight;
	
	char *gs_call[100];
	int gs_arg;
	int interpreter_pid;
	struct record_list *ps_input;	
	char *input_buffer;	
	unsigned int bytes_left;	
	char *input_buffer_ptr; 
	unsigned int buffer_bytes_left; 
	int	interpreter_input_id;
	int interpreter_input;
	int interpreter_output_id;
	int interpreter_error_id;
	int	interpreter_output; 
	int	interpreter_error;	
	char *buf;
	int ret;
	int	std_in[2];
	int	std_out[2];
	int	std_err[2];
	
}; 

#endif // KPSWIDGET_H


