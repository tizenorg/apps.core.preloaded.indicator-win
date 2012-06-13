%define PREFIX    "/opt/apps/org.tizen.indicator"
%define RESDIR    "/opt/apps/org.tizen.indicator/res"
%define DATADIR    "/opt/apps/org.tizen.indicator/data"

Name:       org.tizen.indicator
Summary:    indicator window
Version:    0.1.0
Release:    1
Group:      utils
License:    Samsung Proprietary License
Source0:    %{name}-%{version}.tar.gz
Source1001: packaging/org.tizen.indicator.manifest 

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

%clean
rm -rf %{buildroot}

%post
INHOUSE_ID="5000"

init_vconf()
{
	vconftool set -t int memory/radio/state 0 -i -g 6518
	vconftool set -t int memory/music/state 0 -i -g 6518
	vconftool set -t int memory/indicator/home_pressed 0 -i -g 6518
}

change_file_executable()
{
    chmod +x $@ 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to change the perms of $@"
    fi  
}

init_vconf
change_file_executable /etc/init.d/indicator
mkdir -p /etc/rc.d/rc5.d/
mkdir -p /etc/rc.d/rc3.d/
ln -s  /etc/init.d/indicator /etc/rc.d/rc5.d/S01indicator
ln -s  /etc/init.d/indicator /etc/rc.d/rc3.d/S44indicator

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
/etc/init.d/indicator
/usr/share/applications/indicator.desktop
