 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

SUBDIRS = doc
bin_PROGRAMS = 	kghostview
kghostview_SOURCES = marklist.cpp copyright.cpp interpreter.cpp messages.cpp \
info.cpp print.cpp goto.cpp viewcontrol.cpp main.cpp kghostview.cpp \
kpswidget.cpp ps.c  zoom.cpp
noinst_HEADERS =  marklist.h copyright.h interpreter.h messages.h info.h \
print.h goto.h viewcontrol.h kghostview.h kpswidget.h ps.h  \
zoom.h
kghostview_LDADD =         $(LIB_KFILE)
kghostview_LDFLAGS = $(all_libraries) $(KDE_RPATH)

BUILTSOURCES = marklist.moc copyright.moc interpreter.moc messages.moc \
info.moc print.moc goto.moc viewcontrol.moc kghostview.moc kpswidget.moc \
zoom.moc
DISTCLEANFILES = marklist.moc copyright.moc interpreter.moc messages.moc \
info.moc print.moc goto.moc viewcontrol.moc kghostview.moc kpswidget.moc \
zoom.moc

EXTRA_DIST = kghostview.kdelnk kghostview.xpm mini-kghostview.xpm

zoom.cpp:zoom.moc
zoom.moc: zoom.h
	$(MOC) zoom.h -o zoom.moc

marklist.cpp:marklist.moc
marklist.moc: marklist.h
	$(MOC) marklist.h -o marklist.moc

copyright.cpp:copyright.moc
copyright.moc: copyright.h
	$(MOC) copyright.h -o copyright.moc

interpreter.cpp:interpreter.moc
interpreter.moc: interpreter.h
	$(MOC) interpreter.h -o interpreter.moc

messages.cpp:messages.moc
messages.moc: messages.h
	$(MOC) messages.h -o messages.moc

info.cpp:info.moc
info.moc: info.h
	$(MOC) info.h -o info.moc

print.cpp:print.moc
print.moc: print.h
	$(MOC) print.h -o print.moc

goto.cpp:goto.moc
goto.moc: goto.h
	$(MOC) goto.h -o goto.moc

viewcontrol.cpp:viewcontrol.moc
viewcontrol.moc: viewcontrol.h
	$(MOC) viewcontrol.h -o viewcontrol.moc

kghostview.cpp:kghostview.moc
kghostview.moc: kghostview.h
	$(MOC) kghostview.h -o kghostview.moc

kpswidget.cpp:kpswidget.moc
kpswidget.moc: kpswidget.h
	$(MOC) kpswidget.h -o kpswidget.moc

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
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kghostview_SOURCES) && mv messages.po ../po/kghostview.pot
