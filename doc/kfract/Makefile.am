
INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = -lkdeui -lkdecore  -lqt -lXext -lX11 $(LIBSOCKET)

kfract_SOURCES = kfract.C kfractdlgs.C drawview.C fract.C \
                mandel.C julia.C main.C
DISTCLEANFILES = kfract.moc kfractdlgs.moc 
bin_PROGRAMS   = kfract

noinst_HEADERS = kfract.h kfractdlgs.h drawview.h fract.h julia.h lambda.h \
				mandel.h version.h

lnk_DATA = kfract.kdelnk
lnkdir = $(kde_appsdir)/Graphics

icon_DATA = kfract.xpm
icondir = $(kde_icondir)

docs_DATA = index.html kfract.gif
docsdir = $(kde_htmldir)/en/kfract

EXTRA_DIST = $(lnk_DATA) $(icon_DATA) $(docs_DATA) mini-kfract.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) ./mini-kfract.xpm $(kde_minidir)/kfract.xpm

uninstall:
	rm -f $(kde_minidir)/kfract.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kfract_SOURCES) && mv messages.po ../po/kfract.pot

kfract.C:kfract.moc
kfract.moc: kfract.h
	$(MOC) kfract.h -o kfract.moc

kfractdlgs.C:kfractdlgs.moc
kfractdlgs.moc: kfractdlgs.h
	$(MOC) kfractdlgs.h -o kfractdlgs.moc

