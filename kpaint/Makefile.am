# $Id$
##

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD	=	-lkfm -lkdeui -lkdecore -lgif -lqt -lXext -lX11 $(LIBJPEG) -lm $(LIBSOCKET)


appsdir = $(kde_appsdir)/Graphics
apps_DATA = kpaint.kdelnk

icondir = $(kde_icondir)
icon_DATA = kpaint.xpm

kpaint_SOURCES = main.cpp app.cpp canvas.cpp kpaint.cpp manager.cpp \
	formats.cpp infodialog.cpp palette.cpp palettedialog.cpp \
	formatdialog.cpp canvassize.cpp properties.cpp gif.cpp jpeg.cpp \
	QwViewport.cpp tool.cpp ellipse.cpp circle.cpp line.cpp spraycan.cpp \
	rectangle.cpp pen.cpp areaselect.cpp depthdialog.cpp

DISTCLEANFILES = canvas.moc app.moc kpaint.moc manager.moc infodialog.moc \
	depthdialog.moc properties.moc formatdialog.moc palette.moc \
	palettedialog.moc QwViewport.moc tool.moc spraycan.moc

bin_PROGRAMS = kpaint

SUBDIRS = doc toolpics

install-local-data: mini-kpaint.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kpaint.xpm $(kde_minidir)/kpaint.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kpaint.xpm

messages:
	@XGETTEXT@ -C --keyword=translate -x ../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kpaint.pot

canvas.cpp:canvas.moc
canvas.moc: canvas.h
	$(MOC) canvas.h -o canvas.moc

kpaint.cpp:kpaint.moc
kpaint.moc: kpaint.h
	$(MOC) kpaint.h -o kpaint.moc

app.cpp:app.moc
app.moc: app.h
	$(MOC) app.h -o app.moc

manager.cpp:manager.moc
manager.moc: manager.h
	$(MOC) manager.h -o manager.moc

infodialog.cpp:infodialog.moc
infodialog.moc: infodialog.h
	$(MOC) infodialog.h -o infodialog.moc

depthdialog.cpp:depthdialog.moc
depthdialog.moc: depthdialog.h
	$(MOC) depthdialog.h -o depthdialog.moc

properties.cpp:properties.moc
properties.moc: properties.h
	$(MOC) properties.h -o properties.moc

formatdialog.cpp:formatdialog.moc
formatdialog.moc: formatdialog.h
	$(MOC) formatdialog.h -o formatdialog.moc

palette.cpp:palette.moc
palette.moc: palette.h
	$(MOC) palette.h -o palette.moc

palettedialog.cpp:palettedialog.moc
palettedialog.moc: palettedialog.h
	$(MOC) palettedialog.h -o palettedialog.moc

QwViewport.cpp:QwViewport.moc
QwViewport.moc: QwViewport.h
	$(MOC) QwViewport.h -o QwViewport.moc

tool.cpp:tool.moc
tool.moc: tool.h
	$(MOC) tool.h -o tool.moc

spraycan.cpp:spraycan.moc
spraycan.moc: spraycan.h
	$(MOC) spraycan.h -o spraycan.moc


