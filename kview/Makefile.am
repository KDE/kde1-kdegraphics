SUBDIRS = layout doc
bin_PROGRAMS= kview

#CORBA_SRC = kmicoapp.h kmicoapp.cpp
CORBA_SRC=

BUILT_SOURCES =  moc_canvas.cpp moc_filter.cpp moc_filtmenu.cpp moc_ilistdlg.cpp moc_kview.cpp moc_prefdlg.cpp moc_viewer.cpp moc_kaccelmenuwatch.cpp

DISTCLEANFILES = $(BUILT_SOURCES)

INCLUDES = $(all_includes) -I/usr/local/mico/include -I$(srcdir)/layout
kview_SOURCES = canvas.cpp kview.cpp main.cpp \
	viewer.cpp canvas.h kview.h \
	version.h viewer.h filter.h filter.cpp filtlist.h filtlist.cpp \
	colour.h colour.cpp filtmenu.h filtmenu.cpp ilistdlg.h ilistdlg.cpp \
	numdlg.h numdlg.cpp kcproc.h kcproc.cpp khelpidx.h khelpidx.cpp \
	prefdlg.h prefdlg.cpp kaccelmenuwatch.h kaccelmenuwatch.cpp \
	$(CORBA_SRC) $(BUILT_SOURCES)

lnkdir = $(kde_appsdir)/Graphics
lnk_DATA = kview.kdelnk

EXTRA_DIST = $(lnk_DATA)

kview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kview_LDADD = layout/liblayout.a $(LIB_KFILE) $(LIB_KIMGIO) 

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kview_SOURCES) && mv messages.po ../po/kview.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kview kview *.h -lqt -lkdecore -lkdeui -lkfile 

###KMAKE-start (don't edit or delete this block)

moc_canvas.cpp: canvas.h
	$(MOC) $(srcdir)/canvas.h -o moc_canvas.cpp

moc_filter.cpp: filter.h
	$(MOC) $(srcdir)/filter.h -o moc_filter.cpp

moc_filtmenu.cpp: filtmenu.h
	$(MOC) $(srcdir)/filtmenu.h -o moc_filtmenu.cpp

moc_ilistdlg.cpp: ilistdlg.h
	$(MOC) $(srcdir)/ilistdlg.h -o moc_ilistdlg.cpp

moc_kmicoapp.cpp: kmicoapp.h
	$(MOC) $(srcdir)/kmicoapp.h -o moc_kmicoapp.cpp

moc_kview.cpp: kview.h
	$(MOC) $(srcdir)/kview.h -o moc_kview.cpp

moc_prefdlg.cpp: prefdlg.h
	$(MOC) $(srcdir)/prefdlg.h -o moc_prefdlg.cpp

moc_viewer.cpp: viewer.h
	$(MOC) $(srcdir)/viewer.h -o moc_viewer.cpp

moc_kaccelmenuwatch.cpp: kaccelmenuwatch.h
	$(MOC) $(srcdir)/kaccelmenuwatch.h -o moc_kaccelmenuwatch.cpp

###KMAKE-end
