 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

SUBDIRS = doc
bin_PROGRAMS = 	kghostview
kghostview_SOURCES = marklist.cpp copyright.cpp interpreter.cpp messages.cpp \
info.cpp print.cpp goto.cpp viewcontrol.cpp main.cpp kghostview.cpp \
kpswidget.cpp ps.c kaccel.cpp kkeydialog.cpp kckey.c kglobalaccel.cpp zoom.cpp
kghostview_METASOURCES = USE_AUTOMOC
noinst_HEADERS =  marklist.h copyright.h interpreter.h messages.h info.h \
print.h goto.h viewcontrol.h kghostview.h kpswidget.h ps.h kaccel.h \
kkeydialog.h kckey.h kglobalaccel.h zoom.h
kghostview_LDADD = $(all_libraries) -lkfile -lkfm -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)
kghostview_LDFLAGS = $(LDFLAGS) $(KDE_RPATH)
DISTCLEANFILES = $(kghostview_METASOURCES)

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) kghostview.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) kghostview.xpm $(kde_icondir)/kghostview.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kghostview.xpm $(kde_minidir)/kghostview.xpm

uninstall-local:
	rm -f $(APPSDIR)/kghostview.kdelnk
	rm -f $(kde_icondir)/kghostview.xpm
	rm -f $(kde_minidir)/kghostview.xpm

messages:
	@XGETTEXT@ -C -ktranslate -ki18n -x ../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kghostview.pot
