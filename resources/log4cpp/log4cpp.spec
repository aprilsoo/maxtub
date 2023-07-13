%define  RELEASE 4
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}

%define lib_name log4cpp
%define manualdir       /var/www/html/manual/%{name}

Name: log4cpp
Version: 1.1
Release: %rel

Summary: Log for C++
License: LGPL
Group: Development/Libraries
Vendor:  Bastiaan Bakker <bastiaan.bakker@lifeline.nl>
Packager: Cedric Le Goater <cedric@legoater.com>
Url: http://log4cpp.sourceforge.net/

Source: ftp://download.sourceforge.net/pub/sourceforge/log4cpp/%name-%version.tar.gz

Prefix: %_prefix
BuildRoot: %_tmppath/%name-%version-root

%description
Log for C++ is a library of classes for flexible logging to files, syslog,
and other destinations. It is modeled after the Log for Java library and
stays as close to its API as is reasonable.

%package devel
Summary: development tools for Log for C++
Group: Development/Libraries
Requires: %name = %version

%description devel
The %name-devel package contains the static libraries and header files
needed for development with %name.

%package doc
Summary: HTML formatted API documention for Log for C++
Group: Development/Libraries
#can't set doc package to noarch without setting the others as well.
#BuildArch: noarch
%{!?_without_doxygenrpm:BuildRequires: doxygen}

%description doc
The %name-doc package contains HTML formatted API documention generated by
the popular doxygen documentation generation tool.

%prep
%{__rm} -rf $RPM_BUILD_ROOT

%setup
CC=%{__cc} CXX=%{__cxx} ./configure --prefix=%{prefix} --enable-doxygen 

%build
%{__make}

%install
%{__rm} -rf $RPM_BUILD_ROOT

%{__make} prefix=$RPM_BUILD_ROOT%{prefix}docdir=$RPM_BUILD_ROOT/%{manualdir} install
%clean
%{__rm} -rf $RPM_BUILD_ROOT

%ifnos solaris2.8 solaris2.9 solaris2.10
%post -p /sbin/ldconfig
%endif

%post devel
if test "x$RPM_INSTALL_PREFIX0" != "x" ; then
    %{__perl} -pi -e"s|^libdir='[^\']*'|libdir='$RPM_INSTALL_PREFIX0/lib'|" $RPM_INSTALL_PREFIX0/lib/liblog4cpp.la
    %{__perl} -pi -e"s|^prefix=\"[^\"]*\"|prefix=\"$RPM_INSTALL_PREFIX0\"|" $RPM_INSTALL_PREFIX0/bin/log4cpp-config
fi

%ifnos solaris2.8 solaris2.9 solaris2.10 
%postun -p /sbin/ldconfig
%endif

%files
%defattr(-,root,root,755)
%attr(755,root,root) %prefix/lib/lib*.so.*
%doc AUTHORS COPYING INSTALL NEWS README THANKS ChangeLog

%files devel
%defattr(-,root,root,755)
%prefix/include/*
%prefix/man/*
%attr(755,root,root) %prefix/bin/log4cpp-config
%attr(755,root,root) %prefix/lib/lib*.so
%attr(644,root,root) %prefix/lib/*.*a
%attr(644,root,root) %prefix/lib/pkgconfig/log4cpp.pc
%attr(644,root,root) %prefix/share/aclocal/*.m4

%files doc
%defattr(-,root,root)
%doc %{manualdir}

