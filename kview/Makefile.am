SUBDIRS = layout doc
bin_PROGRAMS= kview

#CORBA_SRC = kmicoapp.h kmicoapp.cpp
CORBA_SRC=

BUILT_SOURCES = moc_canvas.cpp moc_filter.cpp moc_filtmenu.cpp moc_viewer.cpp moc_ilistdlg.cpp

INCLUDES = $(all_includes) -I/usr/local/mico/include -I kimgio -Ilayout
kview_SOURCES = canvas.cpp kview.cpp main.cpp \
	viewer.cpp canvas.h kview.h \
	version.h viewer.h filter.h filter.cpp filtlist.h filtlist.cpp \
	colour.h colour.cpp filtmenu.h filtmenu.cpp ilistdlg.h ilistdlg.cpp \
	numdlg.h numdlg.cpp kcproc.h kcproc.cpp \
	$(CORBA_SRC) $(BUILT_SOURCES)

lnkdir = $(kde_appsdir)/Graphics
lnk_DATA = kview.kdelnk

EXTRA_DIST = $(lnk_DATA)

#kview_LDADD = $(all_libraries) -L/usr/local/mico/lib -lkfile -lkdeui -lkdecore -lkfm -lqt -lX11 -lXext -ljpeg -lmico2.0.8 -ldl

kview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kview_LDADD = layout/liblayout.a \
	-lkfile -lkfm -lkdeui -lkdecore -lkimgio -lqt -lXext -lX11 \
		$(KIMGIO_LIBS) -lm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kview_SOURCES) && mv messages.po ../po/kview.pot

srcdoc:
	kdoc -H -d $(HOME)/web/src/kview *.h -lqt -lkdecore -lkdeui -lkfile 

###KMAKE-start (don't edit or delete this block)
moc_canvas.cpp: canvas.h
	$(MOC) canvas.h -o moc_canvas.cpp

moc_filter.cpp: filter.h
	$(MOC) filter.h -o moc_filter.cpp

moc_filtmenu.cpp: filtmenu.h
	$(MOC) filtmenu.h -o moc_filtmenu.cpp

moc_kmicoapp.cpp: kmicoapp.h
	$(MOC) kmicoapp.h -o moc_kmicoapp.cpp

moc_viewer.cpp: viewer.h
	$(MOC) viewer.h -o moc_viewer.cpp


moc_ilistdlg.cpp: ilistdlg.h
	$(MOC) ilistdlg.h -o moc_ilistdlg.cpp

###KMAKE-end
