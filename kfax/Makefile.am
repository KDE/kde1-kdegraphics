# $Id$

LDFLAGS	=	$(all_libraries)
LDADD   =       -lkfm -lkdecore -lkdeui -lqt -lXext -lX11 ./libtiffax/libtif.a  -lm $(LIBSOCKET) $(KDE_RPATH)
INCLUDES=	$(all_includes) -I./libtiffax

####### Files
noinst_HEADERS = kfax.h faxexpand.h print.h options.h about.h

kfax_SOURCES   = kfax.cpp viewfax.cpp faxexpand.cpp faxinit.cpp faxinput.cpp\
		 fax2tiff.c fax2ps.c print.cpp options.cpp about.cpp

SRCMETA	       = kfax.moc print.moc options.moc about.moc
DISTCLEANFILES = $(SRCMETA)

bin_PROGRAMS   = kfax

SUBDIRS = libtiffax docs

appsdir = $(kde_appsdir)/Graphics
apps_DATA = KFax.kdelnk

picsdir = $(kde_datadir)/kfax/pics
pics_DATA = kfax.tif kfaxlogo.xpm

icondir = $(kde_icondir)
icon_DATA = kfax.xpm

install-data-local: mini-kfax.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA)  mini-kfax.xpm $(kde_minidir)/kfax.xpm

messages:
	@XGETTEXT@ -C -ki18n *.cpp -x../../kdelibs/po/kde.pot && mv messages.po ../po/kfax.pot

uninstall:
	rm -f $(kde_minidir)/kfax.xpm

####### Explicit dependencies


####### Meta objects

kfax.cpp: kfax.moc
kfax.moc: kfax.h
	$(MOC) kfax.h -o kfax.moc

about.cpp: about.moc
about.moc: about.h
	$(MOC) about.h -o about.moc

print.cpp: print.moc
print.moc: print.h
	$(MOC) print.h -o print.moc

options.cpp: options.moc
options.moc: options.h
	$(MOC) options.h -o options.moc









