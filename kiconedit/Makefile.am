#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

APPSDIR = $(kde_appsdir)/Graphics
INCLUDES = $(all_includes)
SUBDIRS = pics doc

bin_PROGRAMS = kiconedit

kiconedit_SOURCES = utils.cpp jpeg.cpp main.cpp kiconedit.cpp kicongrid.cpp kiconcolors.cpp \
	kcolorgrid.cpp kiconfiledlg.cpp kiconcombiview.cpp kiconfileview.cpp kicondetaillist.cpp \
	kicontools.cpp kicon.cpp kresize.cpp knew.cpp debug.cpp properties.cpp xview.cpp \
	kiconeditslots.cpp kiconconfig.cpp kdropsite.cpp kdragsource.cpp

kiconedit_METASOURCES = USE_AUTOMOC

# the library search path. 
kiconedit_LDFLAGS = $(all_libraries)
kiconedit_LDADD = -lkfile -lkfm -lkdeui -lkdecore -lkimgio \
	-lqt -lXext -lX11 $(KIMGIO_LIBS) $(LIBSOCKET)

CLEANFILES = $(kiconedit_METASOURCES)
DISTCLEANFILES = Makefile *~ .deps/* .libs/*

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) kiconedit.kdelnk $(APPSDIR)

uninstall-local:
	-rm -f $(APPSDIR)/kiconedit.kdelnk

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kiconedit_SOURCES) && mv messages.po ../po/kiconedit.pot



docs:
	cd doc && make docs
