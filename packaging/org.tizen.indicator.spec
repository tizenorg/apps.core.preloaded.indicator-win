%define PREFIX    "/opt/apps/org.tizen.indicator"
%define RESDIR    "/opt/apps/org.tizen.indicator/res"
%define DATADIR    "/opt/apps/org.tizen.indicator/data"

Name:       org.tizen.indicator
Summary:    indicator window
Version:    0.1.0
Release:    1
Group:      utils
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz
Source101:  indicator.service
Source1001: org.tizen.indicator.manifest 

BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(libprivilege-control)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(utilX)

BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools

Requires(post): /usr/bin/vconftool

%description
indicator window.

%prep
%setup -q

%build
cp %{SOURCE1001} .
LDFLAGS+="-Wl,--rpath=%{PREFIX}/lib -Wl,--as-needed";export LDFLAGS
cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/%{_sysconfdir}/rc.d/rc5.d/
mkdir -p %{buildroot}/%{_sysconfdir}/rc.d/rc3.d/
ln -s ../../init.d/indicator %{buildroot}/%{_sysconfdir}/rc.d/rc5.d/S01indicator
ln -s ../../init.d/indicator %{buildroot}/%{_sysconfdir}/rc.d/rc3.d/S44indicator

install -d %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants
install -m0644 %{SOURCE101} %{buildroot}%{_libdir}/systemd/user/
ln -sf ../indicator.service %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants/indicator.service

%clean
rm -rf %{buildroot}


%post
vconftool set -t int memory/radio/state 0 -i -g 6518
vconftool set -t int memory/music/state 0 -i -g 6518
vconftool set -t int memory/indicator/home_pressed 0 -i -g 6518

%postun
/sbin/ldconfig 
rm -f /etc/rc.d/rc5.d/S01indicator
rm -f /etc/rc.d/rc3.d/S44indicator


%files
%manifest org.tizen.indicator.manifest
%defattr(-,root,root,-)
%attr(775,app,app) /opt/apps/org.tizen.indicator/data
/opt/apps/org.tizen.indicator/bin/*
/opt/apps/org.tizen.indicator/res/locale/*
/opt/apps/org.tizen.indicator/res/icons/*
/opt/apps/org.tizen.indicator/res/edje/*
%attr(755,-,-) %{_sysconfdir}/init.d/indicator
/usr/share/applications/indicator.desktop
%{_sysconfdir}/rc.d/rc5.d/S01indicator
%{_sysconfdir}/rc.d/rc3.d/S44indicator
%{_libdir}/systemd/user/core-efl.target.wants/indicator.service
%{_libdir}/systemd/user/indicator.service
