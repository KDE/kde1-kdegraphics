 
APPSDIR = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files


bin_PROGRAMS = 	kcmkeys
kcmkeys_SOURCES = main.cpp kcmkeys.cpp display.cpp \
 kaccel.cpp kkeydialog.cpp kckey.c kglobalaccel.cpp
noinst_HEADERS =  kcmkeys.h \
kaccel.h \
kkeydialog.h kckey.h kglobalaccel.h display.h 
kcmkeys_LDADD = $(all_libraries) -lkfm -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)
kcmkeys_LDFLAGS = $(LDFLAGS) $(KDE_RPATH)

BUILTSOURCES = kcmkeys.moc \
kaccel.moc kkeydialog.moc kglobalaccel.moc display.moc 
DISTCLEANFILES = kcmkeys.moc kaccel.moc kkeydialog.moc kglobalaccel.moc

display.cpp:display.moc
display.moc: display.h
	$(MOC) display.h -o display.moc

kglobalaccel.cpp:kglobalaccel.moc
kglobalaccel.moc: kglobalaccel.h
	$(MOC) kglobalaccel.h -o kglobalaccel.moc

kkeydialog.cpp:kkeydialog.moc
kkeydialog.moc: kkeydialog.h
	$(MOC) kkeydialog.h -o kkeydialog.moc

kaccel.cpp:kaccel.moc
kaccel.moc: kaccel.h
	$(MOC) kaccel.h -o kaccel.moc
	
kcmkeys.cpp:kcmkeys.moc
kcmkeys.moc: kcmkeys.h
	$(MOC) kcmkeys.h -o kcmkeys.moc


install-data-local: 


uninstall-local:

messages:
	@XGETTEXT@ -C -ktranslate -ki18n -x ../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kghostview.pot
