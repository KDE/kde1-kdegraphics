 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

SUBDIRS = doc
bin_PROGRAMS = 	kghostview
kghostview_SOURCES = marklist.cpp copyright.cpp interpreter.cpp messages.cpp \
info.cpp print.cpp goto.cpp viewcontrol.cpp main.cpp kghostview.cpp \
kpswidget.cpp ps.c kaccel.cpp kkeydialog.cpp kckey.c kglobalaccel.cpp zoom.cpp
noinst_HEADERS =  marklist.h copyright.h interpreter.h messages.h info.h \
print.h goto.h viewcontrol.h kghostview.h kpswidget.h ps.h kaccel.h \
kkeydialog.h kckey.h kglobalaccel.h zoom.h
kghostview_LDADD = $(all_libraries) -lkfile -lkfm -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)
kghostview_LDFLAGS = $(LDFLAGS) $(KDE_RPATH)

BUILTSOURCES = marklist.moc copyright.moc interpreter.moc messages.moc \
info.moc print.moc goto.moc viewcontrol.moc kghostview.moc kpswidget.moc \
kaccel.moc kkeydialog.moc kglobalaccel.moc zoom.moc
DISTCLEANFILES = marklist.moc copyright.moc interpreter.moc messages.moc \
info.moc print.moc goto.moc viewcontrol.moc kghostview.moc kpswidget.moc \
zoom.moc

zoom.cpp:zoom.moc
zoom.moc: zoom.h
	$(MOC) zoom.h -o zoom.moc

kglobalaccel.cpp:kglobalaccel.moc
kglobalaccel.moc: kglobalaccel.h
	$(MOC) kglobalaccel.h -o kglobalaccel.moc

kkeydialog.cpp:kkeydialog.moc
kkeydialog.moc: kkeydialog.h
	$(MOC) kkeydialog.h -o kkeydialog.moc

kaccel.cpp:kaccel.moc
kaccel.moc: kaccel.h
	$(MOC) kaccel.h -o kaccel.moc

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
	@XGETTEXT@ -C -ktranslate -ki18n -x ../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kghostview.pot
