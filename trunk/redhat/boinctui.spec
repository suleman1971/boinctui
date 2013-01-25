Name:		boinctui
Version:	2.2.1
Release:	0%{?dist}
Summary:	curses based manager for Boinc client
Group:		Applications/Communications
License:	GPL-3.0+
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
#Source0:	boinctui-%{version}.tgz
Source0:	boinctui_%{version}.tar.gz

#BuildRequires:	autoconf
#Requires:	libcurses libexpat

%description
boinctui is a fullscreen text based control tool for BOINC client.
based on curses library

%prep
#echo ----- %{SOURCE0}
#cp %{SOURCE0} %{buildroot}
#%setup -q -n %{name}
%setup -q -n %{name}-%{version}

%build
autoconf
./configure
make clean
make

%install
make DESTDIR=%{buildroot} install

%files
%{_bindir}/%{name}

