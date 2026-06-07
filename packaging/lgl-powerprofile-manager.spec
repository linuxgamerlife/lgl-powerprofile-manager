Name:           lgl-powerprofile-manager
Version:        1.1.1
Release:        1%{?dist}
Summary:        Qt6 GUI for managing tuned power profiles

License:        MIT
URL:            https://github.com/linuxgamerlife/lgl-powerprofile-manager
Source0:        %{url}/archive/refs/tags/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  desktop-file-utils
BuildRequires:  appstream

Requires:       tuned
Requires:       polkit

%description
LGL Power Profile Manager is a Qt6 desktop application for switching between
tuned performance profiles on Fedora and RHEL-based systems.

It runs as a normal user and uses pkexec to apply profiles with elevated
privileges. The application minimizes to the system tray when closed so it
stays available without occupying taskbar space. Features include a system
tray icon, auto-refresh of the active profile, a scrollable profile list
with descriptions, a command log, and a reference tab covering all built-in
tuned profiles.


%prep
%autosetup -n %{name}-%{version}


%build
%cmake
%cmake_build


%install
%cmake_install


%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop
appstreamcli validate --no-net %{buildroot}%{_datadir}/metainfo/%{name}.metainfo.xml


%files
%license LICENSE
%doc README.md CHANGELOG.md
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/metainfo/%{name}.metainfo.xml
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/icons/hicolor/256x256/apps/%{name}.png


%changelog
* Sat Jun 07 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.1.1-1
- Version bump to force COPR rebuild

* Sat Jun 07 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.1.0-1
- Minimize to tray on window close instead of quitting
- Show one-time tray notification on first minimize
- Replace tray color-square icon with actual application icon
- Moved packaging files into packaging/ subdirectory
- Added AppStream metainfo
- Added multi-size hicolor icon set (48, 64, 128, 256)

* Sat Apr 05 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.0-1
- Initial release
