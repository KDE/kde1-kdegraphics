
INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = -lkdeui -lkdecore  -lqt -lXext -lX11 $(LIBSOCKET)

kfract_SOURCES = kfract.C kfractdlgs.C drawview.C fract.C \
                mandel.C julia.C main.C
DISTCLEANFILES = kfract.moc kfractdlgs.moc 
bin_PROGRAMS   = kfract

lnk_DATA = kfract.kdelnk
lnkdir = $(kde_appsdir)/Graphics

icon_DATA = kfract.xpm
icondir = $(kde_icondir)

docs_DATA = index.html kfract.gif
docsdir = $(kde_htmldir)/en/kfract

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) ./mini-kfract.xpm $(kde_minidir)/kfract.xpm

uninstall:
	rm -f $(kde_minidir)/kfract.xpm

kfract.C:kfract.moc
kfract.moc: kfract.h
	$(MOC) kfract.h -o kfract.moc

kfractdlgs.C:kfractdlgs.moc
kfractdlgs.moc: kfractdlgs.h
	$(MOC) kfractdlgs.h -o kfractdlgs.moc


