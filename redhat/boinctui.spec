# Generate RPM using:
#   $ cd ~
#   $ rpmdev-setuptree
#   $ git clone https://github.com/suleman1971/boinctui
#   $ patch -p0 < boinctui-makefile.patch
#   $ cd boinctui
#   $ autoconf
#   $ ./configure --without-gnutls
#   $ make srctgz
#   $ cd ..
#   $ mv boinctui_2.5.0.orig.tar.gz rpmbuild/SOURCES/
#   $ rm -rf boinctui*
#   $ cd rpmbuild/SPECS
#   $ rpmbuild -bb boinctui.spec
#   $ ls ../RPMS/

Name:           boinctui
Version:        2.5.1
Release:        1%{?dist}
Summary:        Fullscreen Text Mode Manager For BOINC Client
Group:          Applications/Communications

License:        GPLv3+
URL:            https://github.com/suleman1971/boinctui
# Source0:        http://download.sourceforge.net/project/boinctui/boinctui_%{version}.tar.gz
Source0:        ../boinctui_%{version}.tar.gz

BuildRequires:  autoconf
BuildRequires:  gcc
BuildRequires:  ncurses-devel
BuildRequires:  expat-devel
BuildRequires:  openssl-devel
BuildRequires:  gcc-c++
Requires:       ncurses
Requires:       expat
Requires:       openssl

%description
 boinctui is a fullscreen text mode control tool for BOINC client
 It can manage local and remote clients (via boinc RPC), and allows
 you to switch between  clients with a hot key.
 boinctui uses curses library and provides the following features:
  * Fullscreen curses based text user interface
  * Switch between several BOINC clients hosts via hot key
  * View task list (run, queue, suspend e.t.c state)
  * View message list
  * Suspend/Resume/Abort tasks
  * Update/Suspend/Resume/Reset/No New Task/Allow New Task for projects
  * Toggle activity state GPU and CPU tasks
  * Run benchmarks
  * Manage BOINC client on remote hosts via boinc_gui protocol

%prep

%setup -q -n %{name}-%{version}

%build
%__autoconf
%configure --without-gnutls
make %{?_smp_mflags}
%make_build


%install
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1
#install -m 0644 debian/%{name}.1 %{buildroot}%{_mandir}/man1/
#gzip %{buildroot}%{_mandir}/man1/%{name}.1
%make_install


%clean
rm -rf %{buildroot}


%files
%attr(-, root, root) %{_bindir}/%{name}
%attr(-, root, root) %{_mandir}/man1/%{name}.1.gz
%attr(0644, root, root) %{_docdir}/%{name}/changelog

#%doc changelog



%license gpl-3.0.txt



%changelog
* Mon Aug 05 2019 Timothy Mullican <timothy.j.mullican@gmail.com> 2.5.0-1
- Generate new RPM SPEC file to conform with best practices
* Tue Feb 12 2013 Sergey Suslov <suleman1971@gmail.com> 2.2.1-0
- Initial version of the package
