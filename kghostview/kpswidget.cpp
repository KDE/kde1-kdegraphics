
#include "kpswidget.h"
#include "kpswidget.moc"
#include <kmisc.h>

static Bool broken_pipe = False;

KPSWidget::KPSWidget( QWidget *parent ) : QWidget( parent )
{
	//printf("KPSWidget::KPSWidget\n");
	
  	fullView = new QWidget( this );
	CHECK_PTR( fullView );
	gs_window = fullView->winId();
	gs_display = fullView->x11Display();
	fullView->hide();
	fullView->resize(0,0);
	
	horScrollBar = new QScrollBar( QScrollBar::Horizontal, this );
	CHECK_PTR( horScrollBar );
	connect( horScrollBar, SIGNAL( valueChanged(int) ),
		SLOT( slotHorScroll(int) ) );
	
	vertScrollBar = new QScrollBar( QScrollBar::Vertical, this );
	CHECK_PTR( vertScrollBar );
	connect( vertScrollBar, SIGNAL(valueChanged(int)),
		SLOT( slotVertScroll(int) ) );

	// Initialise

	llx = 0;
	lly = 0;
	urx = 0;
	ury = 0;
	buf = (char *)malloc(BUFSIZ);
	left_margin = 0;
	right_margin = 0;
	bottom_margin = 0;
	top_margin = 0;
	foreground = 0;
	background_pixel = 1;
	fullWidth=0;
	fullHeight=0;
	scroll_x_offset=0;
	scroll_y_offset=0;
	antialias=False;
	xdpi=75.0;
	ydpi=75.0;
	disable_start = False;
	interpreter_pid = -1;
	input_buffer = NULL;
	bytes_left = 0;
	input_buffer_ptr = NULL;
	buffer_bytes_left = 0;
	ps_input = NULL;
	interpreter_input = -1;
	interpreter_output = -1;
	interpreter_error = -1;
	interpreter_input_id = None;
	interpreter_output_id = None;
	interpreter_error_id = None;
	orientation = 1;
	changed = False;
	busy = False;
	setCursor( arrowCursor );

	// Compute fullView widget size

	layout();
}

KPSWidget::~KPSWidget()
{
	stopInterpreter();
}

//*********************************************************************************
//
//	PUBLIC METHODS
//
//*********************************************************************************


void KPSWidget::disableInterpreter()
{
	//printf("KPSWidget::disableInterpreter\n");

	disable_start = True;
	stopInterpreter();
}


void KPSWidget::enableInterpreter()
{
	//printf("KPSWidget::enableInterpreter\n");

	disable_start = False;
	startInterpreter();
}


Bool KPSWidget::isInterpreterReady()
{
	//printf("KPSWidget::isInterpreterReady\n");

	return interpreter_pid != -1 && !busy && ps_input == NULL;
}


Bool KPSWidget::isInterpreterRunning()
{
	//printf("KPSWidget::isInterpreterRunning\n");

	return interpreter_pid != -1;
}

Bool KPSWidget::nextPage()
{
	//printf("KPSWidget::nextPage\n");

    XEvent ev;

    if (interpreter_pid < 0) return False;
    if (mwin == None) return False;

    if (!busy) {
		busy = True;
		startTimer(200);
		setCursor( waitCursor );
		
		ev.xclient.type = ClientMessage;
		ev.xclient.display = gs_display;
		ev.xclient.window = mwin;
		ev.xclient.message_type =
			next;
		ev.xclient.format = 32;
		
		XSendEvent(gs_display, mwin, False, 0, &ev);
		XFlush(gs_display);

		return True;
		
    } else {
		return False;
		
    }
}

Bool KPSWidget::sendPS(FILE *fp, long begin,unsigned int len,Bool close)
{
	//printf("KPSWidget::sendPS\n");

	struct record_list *ps_new;
	
	//printf("sendPS input = %d\n", interpreter_input);
	
    if (interpreter_input < 0) return False;

    ps_new = (struct record_list *) malloc(sizeof (struct record_list));
    ps_new->fp = fp;
    ps_new->begin = begin;
    ps_new->len = len;
    ps_new->seek_needed = True;
    ps_new->close = close;
    ps_new->next = NULL;

    if (input_buffer == NULL) {
		input_buffer = (char *) malloc(BUFSIZ);
    }

    if (ps_input == NULL) {
		input_buffer_ptr = input_buffer;
		bytes_left = len;
		buffer_bytes_left = 0;
		ps_input = ps_new;
		interpreter_input_id = 1;
		sn->setEnabled(True);
		
		//printf("enabled INPUT\n");

    } else {
		struct record_list *p = ps_input;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = ps_new;
    }
    
    return False;
}    

//*********************************************************************************
//
//	QT WIDGET REIMPLIMENTED METHODS
//
//*********************************************************************************


void KPSWidget::timerEvent(QTimerEvent *)
{
	/* bitBlt(this, clip_x, clip_y,
		fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
		CopyROP); */
	
	if ( fullView->width() > clip_width && fullView->height() > clip_height ) {
		
		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
			CopyROP);

	} else if ( fullView->width() > clip_width && fullView->height() < clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width,
			fullView->height(),
			CopyROP);

	} else if ( fullView->width() < clip_width && fullView->height() > clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(), clip_height,
			CopyROP);

	} else {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(),
			fullView->height(),
			CopyROP);

	}	
}

void KPSWidget::paintEvent(QPaintEvent *)
{
	if(!busy && psfile) {
		if ( fullView->width() > clip_width && fullView->height() > clip_height ) {

			bitBlt(this, clip_x, clip_y,
				fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
				CopyROP);

		} else if ( fullView->width() > clip_width && fullView->height() < clip_height ) {

			bitBlt(this, clip_x, clip_y,
				fullView, scroll_x_offset, scroll_y_offset, clip_width,
				fullView->height(),
				CopyROP);

		} else if ( fullView->width() < clip_width && fullView->height() > clip_height ) {

			bitBlt(this, clip_x, clip_y,
				fullView, scroll_x_offset, scroll_y_offset, fullView->width(), clip_height,
				CopyROP);

		} else {

			bitBlt(this, clip_x, clip_y,
				fullView, scroll_x_offset, scroll_y_offset, fullView->width(),
				fullView->height(),
				CopyROP);

		}
	}
	
	//debug( "paint event " );	
}

void KPSWidget::resizeEvent(QResizeEvent *)
{
	//printf("KPSWidget::resizeEvent\n");
	
	//printf("fullview width = %d height=%d \n", fullView->width(),
	//fullView->height() );

 if( width() > fullView->width() &&  height() > fullView->height() ) {
		horScrollBar->hide();
		vertScrollBar->hide();
		clip_x= (int)( width()- fullView->width() )/2;
		clip_y= (int)( height()- fullView->height() )/2;
		clip_width= fullView->width();
		clip_height= fullView->height();
		
	} else if ( width() > fullView->width()+SCROLLBAR_SIZE ) {
		horScrollBar->hide();
		vertScrollBar->show();
		clip_x= (int)( width()-( fullView->width()+SCROLLBAR_SIZE) )/2;
		clip_y= 0;
		clip_width= width()-SCROLLBAR_SIZE;
		clip_height= height();
		
	}else if ( height() > fullView->height()+SCROLLBAR_SIZE ) {
		horScrollBar->show();
		vertScrollBar->hide();
		clip_x= 0;
		clip_y= (int)( height()-( fullView->height()+SCROLLBAR_SIZE ))/2;
		clip_width= width();
		clip_height= height()-SCROLLBAR_SIZE;
	}else {
		horScrollBar->show();
		vertScrollBar->show();
		clip_x= 0;
		clip_y= 0;
		clip_width= width()-SCROLLBAR_SIZE;
		clip_height= height()-SCROLLBAR_SIZE;
	} 
	
    horScrollBar->setGeometry( 0, height()-SCROLLBAR_SIZE,
    	clip_width, SCROLLBAR_SIZE);
    if(fullView->width()- width() > 0) {
		horScrollBar->setRange( 0, fullView->width()- width() );
	} else {
		horScrollBar->setRange( 0, 0 );
	}
	horScrollBar->setSteps( (int)( fullView->width()/50),  width() );
	
	vertScrollBar->setGeometry(width()-SCROLLBAR_SIZE, 0,
		SCROLLBAR_SIZE, clip_height);
	if( fullView->height()- height() > 0 ) {
		vertScrollBar->setRange( 0,  fullView->height()- height() );
	} else {
		vertScrollBar->setRange( 0,  0 );
	}
	vertScrollBar->setSteps( (int)( fullView->height()/50),  height() );	 
}

void KPSWidget::layout()
{
	//printf("KPSWidget::layout\n");

    Bool different_size = computeSize();
    if (different_size) {
    	
    	//printf("Changed  layout %d, %d\n", fullWidth, fullHeight);
    	
    	fullView->setGeometry(0, 0, fullWidth, fullHeight);
    	resize(width(), height());
    	repaint();
    	setup();
    	
    }
}

/**********************************************************************************
 *
 *	SLOTS
 *
 **********************************************************************************/

void KPSWidget::scrollRight()
{
	int new_value;
	
	new_value = horScrollBar->value()+50;
	if(new_value > horScrollBar->maxValue())
		new_value = horScrollBar->maxValue();
	
	horScrollBar->setValue( new_value );
}

void KPSWidget::scrollLeft()
{
	int new_value;
	
	new_value = horScrollBar->value()-50;
	if(new_value < horScrollBar->minValue())
		new_value = horScrollBar->minValue();
	
	horScrollBar->setValue( new_value );
}

void KPSWidget::scrollDown()
{
	int new_value;
	
	new_value = vertScrollBar->value()+50;
	if(new_value > vertScrollBar->maxValue())
		new_value = vertScrollBar->maxValue();
	
	vertScrollBar->setValue( new_value );
}

void KPSWidget::scrollUp()
{
	int new_value;
	
	new_value = vertScrollBar->value()-50;
	if(new_value < vertScrollBar->minValue())
		new_value = vertScrollBar->minValue();
	
	vertScrollBar->setValue( new_value );
}

void KPSWidget::scrollTop()
{
	vertScrollBar->setValue( vertScrollBar->minValue() );
}

void KPSWidget::slotVertScroll(int value)
{
	scroll_y_offset =  value;
	
	/* bitBlt(this, clip_x, clip_y,
		fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
		CopyROP);	 */
		
	if ( fullView->width() > clip_width && fullView->height() > clip_height ) {
		
		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
			CopyROP);

	} else if ( fullView->width() > clip_width && fullView->height() < clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width,
			fullView->height(),
			CopyROP);

	} else if ( fullView->width() < clip_width && fullView->height() > clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(), clip_height,
			CopyROP);

	} else {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(),
			fullView->height(),
			CopyROP);

	}
}

void KPSWidget::slotHorScroll(int value)
{
	scroll_x_offset = value;
	
	/* bitBlt(this, clip_x, clip_y,
		fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
		CopyROP); */
		
	if ( fullView->width() > clip_width && fullView->height() > clip_height ) {
		
		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width, clip_height,
			CopyROP);

	} else if ( fullView->width() > clip_width && fullView->height() < clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, clip_width,
			fullView->height(),
			CopyROP);

	} else if ( fullView->width() < clip_width && fullView->height() > clip_height ) {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(), clip_height,
			CopyROP);

	} else {

		bitBlt(this, clip_x, clip_y,
			fullView, scroll_x_offset, scroll_y_offset, fullView->width(),
			fullView->height(),
			CopyROP);

	}
}

//*********************************************************************************
//
//	PRIVATE METHODS
//
//*********************************************************************************


Bool KPSWidget::computeSize()
{
	//printf("KPSWidget::computeSize\n");

	int newWidth=0, newHeight=0;
	Bool change;
	
	switch (orientation) {
	case 1: //PORTRAIT
		orient_angle=0;
		newWidth = (int) ((urx - llx) / 75.0 * xdpi + 0.5);
	    newHeight = (int) ((ury - lly) / 75.0 * ydpi + 0.5);
	    break;
	case 2: //UPSIDEDOWN
		orient_angle=180;
	    newWidth = (int) ((urx - llx) / 75.0 * xdpi + 0.5);
	    newHeight = (int) ((ury - lly) / 75.0 * ydpi + 0.5);
	    break;
	case 3: //LANDSCAPE
		orient_angle=90;
		newWidth = (int) ((ury - lly) / 75.0 * xdpi + 0.5);
	    newHeight = (int) ((urx - llx) / 75.0 * ydpi + 0.5);
	    break;
	case 4: //SEASCAPE
		orient_angle=270;
	    newWidth = (int) ((ury - lly) / 75.0 * xdpi + 0.5);
	    newHeight = (int) ((urx - llx) / 75.0 * ydpi + 0.5);
	    break;
	}
	
	//printf(" newWidth = %d, new Height = %d\n", newWidth, newHeight );
	
	change = (newWidth != fullWidth) || (newHeight != fullHeight);
	
	//if (change) printf("Layout has changed\n");
	
	fullWidth = newWidth;
	fullHeight = newHeight;
	return( change );
}

void KPSWidget::setup()
{
	//printf("KPSWidget::setup\n");

	XSetWindowAttributes xswa;
	
	xswa.backing_store = Always;
	XChangeWindowAttributes(gs_display, gs_window,
				CWBackingStore, &xswa);

	ghostview = (Atom) XInternAtom(gs_display, "GHOSTVIEW", False);
	gs_colors = (Atom) XInternAtom(gs_display, "GHOSTVIEW_COLORS", False);
	next = (Atom) XInternAtom(gs_display, "NEXT", False);
	gs_page = (Atom) XInternAtom(gs_display, "PAGE", False);
	done = (Atom) XInternAtom(gs_display, "DONE", False);
	
	sprintf(buf, "0 %d %d %d %d %d %g %g %d %d %d %d",
	    orient_angle,
	    llx, lly,
	    urx, ury,
	    xdpi, ydpi,
	    left_margin, bottom_margin,
	    right_margin, top_margin);
	
	//printf("%s\n", buf);
	
	XChangeProperty(gs_display, gs_window,
	    ghostview,
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, strlen(buf));

	sprintf(buf, "%s %d %d", "Color",
		(int)BlackPixel(gs_display, DefaultScreen(gs_display)),
		(int)WhitePixel(gs_display, DefaultScreen(gs_display)) );
	//printf("%s\n", buf);
	
	XChangeProperty(gs_display, gs_window,
		gs_colors,
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, strlen(buf));

	XSync(gs_display, False);  // Be sure to update properties 
} 

void KPSWidget::startInterpreter()
{
	//printf("KPSWidget::startInterpreter\n");

	stopInterpreter();
	
	if (disable_start) return;
	
	gs_arg=0;
	gs_call[gs_arg++] = "gs";
	if(antialias) {
		gs_call[gs_arg++] = "-sDEVICE=x11alpha";
		gs_call[gs_arg++] = "-dNOPLATFONTS";
	} else {
		gs_call[gs_arg++] = "-sDEVICE=x11";
	}
	gs_call[gs_arg++] = "-dNOPAUSE";
	gs_call[gs_arg++] = "-dQUIET";
	gs_call[gs_arg++] = "-dSAFER";
	gs_call[gs_arg++] = "-";
	gs_call[gs_arg++] = NULL;
	
	if (filename == NULL) {
		ret = pipe(std_in);
		if (ret == -1) {
	    	perror("Could not create pipe");
	    	exit(1);
		}
    } else if (strcmp(filename, "-")) {
		std_in[0] = open(filename, O_RDONLY, 0);
		//printf("opened %s\n", filename.data());
    }

 
    ret = pipe(std_out);
    if (ret == -1) {
		perror("Could not create pipe");
		exit(1);
    }

    ret = pipe(std_err);
    if (ret == -1) {
		perror("Could not create pipe");
		exit(1);
    } 
	
	changed = False;
	busy = True;
	startTimer(200);
	setCursor( waitCursor );
	
    interpreter_pid = vfork();
    if (interpreter_pid == 0) {
    
  		::close(std_out[0]);
		::close(std_err[0]);
		::dup2(std_out[1], 1);
		::close(std_out[1]);
		::dup2(std_err[1], 2);
		::close(std_err[1]); 
		sprintf(buf, "%d", (int) gs_window);
		setenv("GHOSTVIEW", buf, True);
		setenv("DISPLAY", XDisplayString(gs_display), True);
		if (filename == NULL) {
			::close(std_in[1]);
			::dup2(std_in[0], 0);
			::close(std_in[0]);
		} else if (strcmp(filename, "-")) {
			::dup2(std_in[0], 0);
			::close(std_in[0]);
		}
 	
		execvp(gs_call[0], gs_call);
	  	sprintf(buf, "Exec of this shit failed");
		perror(buf);
		_exit(1); 
		
    } else {
    	if (filename == NULL) {
  			int result;
			::close(std_in[0]);

			result = fcntl(std_in[1], F_GETFL, 0);
			result = result | O_NONBLOCK;
			result = fcntl(std_in[1], F_SETFL, result);

			interpreter_input = std_in[1];
			interpreter_input_id = None;

			sn = new QSocketNotifier( interpreter_input, QSocketNotifier::Write );
			sn->setEnabled(False);
			QObject::connect( sn, SIGNAL( activated(int) ),
				this, SLOT( gs_input() ) );
				
		} else if (strcmp(filename, "-")) {
	    	::close(std_in[0]);
		}
		
	
		::close(std_out[1]);
		interpreter_output = std_out[0];

		::close(std_err[1]);
		interpreter_error = std_err[0];
    }
    XClearArea(gs_display, gs_window,
	       0, 0, fullWidth, fullHeight, False);
}

void KPSWidget::stopInterpreter()
{
	//printf("KPSWidget::stopInterpreter\n");

	if (interpreter_pid >= 0) {
		::kill(interpreter_pid, SIGTERM);
		::wait(0);
		interpreter_pid = -1;
		//printf("Killing gs process\n");
	}
    
	if (interpreter_input >= 0) {
		::close(interpreter_input);
		//printf("closing interpreter_input\n");
		interpreter_input = -1;
		if (interpreter_input_id != None) {
			sn->~QSocketNotifier();
			//printf("Destroy socket notifier\n");
			interpreter_input_id = None;
		}
		while (ps_input) {
			struct record_list *ps_old = ps_input;
			ps_input = ps_old->next;
			if (ps_old->close) fclose(ps_old->fp);
			free((char *)ps_old);
		}
	}
    
    busy=False;
    killTimers();
    setCursor( arrowCursor );
    
    /*********************************************
	if (interpreter_output >= 0) {
		close(interpreter_output);
		interpreter_output = -1;
		XtRemovegs_input(interpreter_output_id);
	}
	if (interpreter_error >= 0) {
		close(interpreter_error);
		interpreter_error = -1;
		XtRemovegs_input(interpreter_error_id);
	}
    **********************************************/
    
}

void KPSWidget::interpreterFailed()
{
	//printf("Interpreter Failed\n");
	stopInterpreter();	
} 

SIGVAL catchPipe(int)
{
    broken_pipe = True;
#ifdef SIGNALRETURNSINT
    return 0;
#endif
}

void KPSWidget::gs_input()
{
	//printf("KPSWidget::gs_input\n");

	int bytes_written;
	SIGVAL (*oldsig)(int);
	oldsig = signal(SIGPIPE, catchPipe);

	do {
		if (buffer_bytes_left == 0) {
			if (ps_input && bytes_left == 0) {
				struct record_list *ps_old = ps_input;
				ps_input = ps_old->next;
				if (ps_old->close) fclose(ps_old->fp);
				free((char *)ps_old);
			}

			if (ps_input && ps_input->seek_needed) {
				if (ps_input->len > 0)
					fseek(ps_input->fp, ps_input->begin, SEEK_SET);
				ps_input->seek_needed = False;
				bytes_left = ps_input->len;
			}

			if (bytes_left > BUFSIZ) {
		
				
				buffer_bytes_left =
				fread(input_buffer, sizeof (char), BUFSIZ, ps_input->fp);
			} else if (bytes_left > 0) {
				buffer_bytes_left =
				fread(input_buffer, sizeof (char), bytes_left, ps_input->fp);
			} else {
				buffer_bytes_left = 0;
			}
			
			if (bytes_left > 0 && buffer_bytes_left == 0) {
				interpreterFailed();	
			}
			input_buffer_ptr = input_buffer;
			bytes_left -= buffer_bytes_left;
			
		}

		if (buffer_bytes_left > 0) {
			bytes_written =
			write(interpreter_input, input_buffer_ptr, buffer_bytes_left);
			
			if (broken_pipe) {
				broken_pipe = False;
				interpreterFailed();		
			} else if (bytes_written == -1) {
				if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
					interpreterFailed();	
				}
			} else {
				buffer_bytes_left -= bytes_written;
				input_buffer_ptr += bytes_written;
			}
		}
		
	} while(ps_input && buffer_bytes_left == 0);

	signal(SIGPIPE, oldsig);
	
	if (ps_input == NULL && buffer_bytes_left == 0) {
		if (interpreter_input_id != None) {
			 sn->setEnabled(False);
			 setCursor( arrowCursor );
			 //printf("Disabled INPUT\n");
			 interpreter_input_id = None;
		}
	}
	
}  


