# $Id$
##

# my developement libraries
my_libraries=/home/joerg/kde/kdelibs/lib
my_includes=/home/joerg/kde/kdelibs/include

INCLUDES=	$(all_includes)
#INCLUDES= -I$(my_includes) $(X_INCLUDES) $(QT_INCLUDES)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
#LDFLAGS = -L$(my_libraries) -L$(x_libraries) -rpath $(my_libraries) -rpath $(x_libraries)
LDADD	= -lkfile -lkfm -lkdeui -lkdecore $(LIB_KIMGIO) -lgif  -lXext  -lm $(LIB_QT)


appsdir = $(kde_appsdir)/Graphics
apps_DATA = kpaint.kdelnk

icondir = $(kde_icondir)
icon_DATA = kpaint.xpm

kpaint_SOURCES = main.cpp app.cpp canvas.cpp kpaint.cpp manager.cpp \
	formats.cpp infodialog.cpp palette.cpp palettedialog.cpp \
	formatdialog.cpp canvassize.cpp properties.cpp gif.cpp \
	QwViewport.cpp tool.cpp ellipse.cpp circle.cpp line.cpp \
	spraycan.cpp rectangle.cpp pen.cpp areaselect.cpp depthdialog.cpp \
	roundangle.cpp navwidget.cpp sidebar.cpp view.cpp colorbar.cpp \
	mainview.cpp text.cpp

noinst_HEADERS = QwViewport.h areaselect.h canvas.h canvassize.h circle.h \
	depthdialog.h ellipse.h formatdialog.h formats.h gif.h infodialog.h \
	kpaint.h kundo.h line.h main.h manager.h navwidget.h palette.h \
	palettedialog.h pen.h properties.h rectangle.h roundangle.h sidebar.h \
	spraycan.h tool.h version.h view.h colorbar.h mainview.h text.h

DISTCLEANFILES = canvas.moc app.moc kpaint.moc manager.moc infodialog.moc \
	depthdialog.moc properties.moc formatdialog.moc palette.moc \
	palettedialog.moc QwViewport.moc tool.moc spraycan.moc circle.moc \
	navwidget.moc sidebar.moc view.moc rectangle.moc roundangle.moc \
	pen.moc ellipse.moc colorbar.moc mainview.moc areaselect.moc text.moc

bin_PROGRAMS = kpaint

SUBDIRS = doc toolpics images

EXTRA_DIST = $(apps_DATA) $(icon_DATA) mini-kpaint.xpm

install-data-local: mini-kpaint.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kpaint.xpm $(kde_minidir)/kpaint.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kpaint.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../po/kpaint.pot

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

sidebar.cpp:sidebar.moc
sidebar.moc: sidebar.h
	$(MOC) sidebar.h -o sidebar.moc

view.cpp:view.moc
view.moc: view.h
	$(MOC) view.h -o view.moc

navwidget.cpp:navwidget.moc
navwidget.moc: navwidget.h
	$(MOC) navwidget.h -o navwidget.moc

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

circle.cpp:circle.moc
circle.moc: circle.h
	$(MOC) circle.h -o circle.moc

ellipse.cpp:ellipse.moc
ellipse.moc: ellipse.h
	$(MOC) ellipse.h -o ellipse.moc

pen.cpp:pen.moc
pen.moc: pen.h
	$(MOC) pen.h -o pen.moc

line.cpp:line.moc
line.moc: line.h
	$(MOC) line.h -o line.moc

roundangle.cpp:roundangle.moc
roundangle.moc: roundangle.h
	$(MOC) roundangle.h -o roundangle.moc

rectangle.cpp:rectangle.moc
rectangle.moc: rectangle.h
	$(MOC) rectangle.h -o rectangle.moc


colorbar.cpp: colorbar.moc
colorbar.moc: colorbar.h
	$(MOC) colorbar.h -o colorbar.moc

mainview.cpp: mainview.moc
mainview.moc: mainview.h
	$(MOC) mainview.h -o mainview.moc

areaselect.cpp: areaselect.moc
areaselect.moc: areaselect.h
	$(MOC) areaselect.h -o areaselect.moc

text.cpp:text.moc
text.moc: text.h
	$(MOC) text.h -o text.moc
