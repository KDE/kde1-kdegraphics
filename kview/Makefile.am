
##
## Makefile for the KView image viewer 
##
## $Id$

INCLUDES = $(all_includes)

SUBDIRS = doc pics

LDFLAGS= $(all_libraries) $(KDE_RPATH)

LDADD = -lkfm -lkdeui -lkdecore -lqt -lX11 -lXext $(LIBJPEG) $(LIBSOCKET) $(LIBPNG)

# uncomment this if you like to use the unsecure filename changer
#LINEEDIT =-DLINEEDIT 

###### No need to edit below this line #############

FORMATSRC= jpeg.cpp kpng.cpp eps.cpp netpbm.cpp

######

kview_SOURCES = main.cpp wview.cpp viewport.cpp formats.cpp \
	QwViewport.cpp kerror.cpp fileman.cpp confighandler.cpp \
	$(FORMATSRC)

bin_PROGRAMS = kview

lnkdir = $(kde_appsdir)/Graphics
lnk_DATA = kview.kdelnk

icondir = $(kde_icondir)
icon_DATA = kview.xpm

install-local-data:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) ./mini-kview.xpm $(kde_minidir)/kview.xpm

uninstall-local:
	rm -f $(kde_minidir)/kview.xpm

backup:
	(cd .. ; tar zcvf kview-project.tgz kview)
	mv ../kview-project.tgz /mount/lan/PROJBAK 


