# $Id$

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =               ./libtiffax/libtif.a  -lm $(LIB_KFILE) 
INCLUDES=	-I./libtiffax $(all_includes)

####### Files
noinst_HEADERS = kfax.h faxexpand.h print.h options.h urldlg.h

kfax_SOURCES   = kfax.cpp viewfax.cpp faxexpand.cpp faxinit.cpp faxinput.cpp\
		 fax2tiff.c fax2ps.c print.cpp options.cpp urldlg.cpp

SRCMETA	       = kfax.moc print.moc options.moc urldlg.moc
DISTCLEANFILES = $(SRCMETA)

bin_PROGRAMS   = kfax

SUBDIRS = libtiffax docs

appsdir = $(kde_appsdir)/Graphics
apps_DATA = KFax.kdelnk

picsdir = $(kde_datadir)/kfax/pics
pics_DATA = kfax.tif kfaxlogo.xpm

icondir = $(kde_icondir)
icon_DATA = kfax.xpm

EXTRA_DIST = $(apps_DATA) $(pics_DATA) $(icon_DATA) mini-kfax.xpm

install-data-local: mini-kfax.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA)  mini-kfax.xpm $(kde_minidir)/kfax.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kfax_SOURCES) printer.h && mv messages.po ../po/kfax.pot

uninstall:
	rm -f $(kde_minidir)/kfax.xpm

####### Explicit dependencies


####### Meta objects

kfax.cpp: kfax.moc
kfax.moc: kfax.h
	$(MOC) kfax.h -o kfax.moc


print.cpp: print.moc
print.moc: print.h
	$(MOC) print.h -o print.moc

urldlg.cpp: urldlg.moc
urldlg.moc: urldlg.h
	$(MOC) urldlg.h -o urldlg.moc

options.cpp: options.moc
options.moc: options.h
	$(MOC) options.h -o options.moc









