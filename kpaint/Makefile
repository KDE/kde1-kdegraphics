# Generated automatically from Makefile.in by configure.
# $Id$

##
## Makefile for kpaint
##

#
# Configuration
#
prefix 	= 	/home/kulow/prod/kde
INCDIRS	=	-I/usr/openwin/include -I/home/kulow/prod/qt-1.2//include -I/home/kulow/prod/kde/include -I..
LIBDIR	=	-L/usr/openwin/lib -L/home/kulow/prod/qt-1.2//lib -L/home/kulow/prod/kde/lib -Ltools
MOC	=	/home/kulow/prod/qt-1.2/bin/moc
INSTALL	=   	/home/kulow/bin/install -c
CXX	= 	c++
OPTS	=	$(CXXFLAGS)
DEFS	=	-DKPDEBUG -DAPPHELPCONTENTSURL="\"$(prefix)/doc/HTML/kpaint/kpaint.html\"" -DHAVE_CONFIG_H
LDLIBS	=	-lkdecore -lkdeui -lkptools -lgif -lqt -lXext -lm -lX11
METADIR	=	metas

# Uncomment this is you are using Qt 1.2 beta 3 or higher
#DEFS+=	-DNEWQT

#
# End Configuration
#

CCFLAGS=	$(OPTS) $(DEFS) $(INCDIRS)
LDFLAGS=	$(LIBDIR)

OBJ=	main.o \
	app.o \
	canvas.o \
	kpaint.o \
	manager.o \
	canvassize.o \
	properties.o \
	QwViewport.o \
	KTopWidget.o \
	KToolbar.o \
	KStatusbar.o \
	gif.o jpeg.o

METAS=	canvas.moc \
	kpaint.moc \
	manager.moc \
	properties.moc \
	QwViewport.moc \
	KTopWidget.moc \
	KToolbar.moc \
	KStatusbar.moc

OBJS= $(addprefix objs/,$(OBJ))

.PHONY:	all clean tools install uninstall

#
# Implicit rules
#

objs/%.o:	%.cpp
	$(CXX) -c $(CCFLAGS) -o $@ $<

%.moc:	%.h
	$(MOC) -o $(METADIR)/$@ $<

all:	kpaint

install:
	$(INSTALL) -d $(prefix)/bin
	$(INSTALL) -d $(prefix)/doc/HTML/kpaint
	$(INSTALL) kpaint $(prefix)/bin
	$(INSTALL) doc/HTML/kpaint.html $(prefix)/doc/HTML/kpaint

uninstall:
	rm -f $(prefix)/bin/kpaint
	rm -rf $(prefix)/doc/HTML/kpaint

tools:
	$(MAKE) -C tools all

kpaint:	tools $(METAS) $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	$(MAKE) -C tools clean
	-rm -f $(OBJS) $(METADIR)/*.moc

realclean:  clean
	$(MAKE) -C tools realclean
	-rm -f kpaint



