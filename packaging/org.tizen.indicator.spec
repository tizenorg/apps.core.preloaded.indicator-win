%define PREFIX    /usr/apps/%{name}
%define RESDIR    %{PREFIX}/res
%define PREFIXRW  /opt/apps/%{name}

Name:       org.tizen.indicator
Summary:    indicator window
Version:    0.1.5
Release:    1
Group:      utils
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-system-runtime-info)
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
BuildRequires:  pkgconfig(status-monitor)

BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools

Requires(post): /usr/bin/vconftool

%description
indicator window.

%prep
%setup -q

%build
LDFLAGS+="-Wl,--rpath=%{PREFIX}/lib -Wl,--as-needed";export LDFLAGS
cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DCMAKE_INSTALL_PREFIXRW=%{PREFIXRW}
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
	vconftool set -t int memory/private/%{name}/home_pressed 0 -i -g 6518
	vconftool set -t bool memory/private/%{name}/started 0 -i -u 5000
}

change_dir_permission()
{
    chown $INHOUSE_ID:$INHOUSE_ID $@ 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to change the owner of $@"
    fi  
    chmod 775 $@ 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to change the perms of $@"
    fi  
}

change_file_executable()
{
    chmod +x $@ 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to change the perms of $@"
    fi  
}

init_vconf
change_dir_permission %{PREFIXRW}/data
change_file_executable /etc/init.d/indicator
mkdir -p /etc/rc.d/rc5.d/
mkdir -p /etc/rc.d/rc3.d/
ln -sf  /etc/init.d/indicator /etc/rc.d/rc5.d/S01indicator
ln -sf  /etc/init.d/indicator /etc/rc.d/rc3.d/S44indicator

%postun
/sbin/ldconfig 
rm -f /etc/rc.d/rc5.d/S01indicator
rm -f /etc/rc.d/rc3.d/S44indicator

%files
%defattr(-,root,root,-)
%{PREFIX}/bin/*
%{RESDIR}/locale/*
%{RESDIR}/icons/*
%{RESDIR}/edje/*
%{PREFIXRW}/data
/usr/share/packages/%{name}.xml
/etc/init.d/indicator
