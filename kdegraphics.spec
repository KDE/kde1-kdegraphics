%define version Beta3
%define name kdegraphics
Name: %{name}
Summary: K Desktop Environment - Graphic Applications
Version: %{version}
Release: 1 
Source: ftp.kde.org:/pub/kde/unstable/CVS/snapshots/current/kdegraphics-%{version}.tar.gz
Group: X11/KDE/Graphics
BuildRoot: /tmp/realhot_%{name}
Copyright: GPL
Requires: qt >= 1.31
Packager: Magnus Pfeffer <pfeffer@unix-ag.uni-kl.de>
Distribution: KDE
Vendor: The KDE Team

%description
Graphic applications for the K Desktop Environment.

Included with this package are:

kdvi: displays TeX's device independent (.dvi) files
kfax: displays fax files
kfract: a fractal generator
kghostview: displays postscript (.ps) files
kpaint: a simple drawing program
kview: displays numerous graphic file formats 

%prep
rm -rf $RPM_BUILD_ROOT

%setup -n kdegraphics

%build
export KDEDIR=/opt/kde
./configure --prefix=$KDEDIR --with-install-root=$RPM_BUILD_ROOT
make

%install
make install

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.%{name}
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f ../file.list.%{name}
