Name:           lgl-powerprofile-manager
Version:        2.0.0
Release:        1%{?dist}
Summary:        GTK 4 interface for managing tuned power profiles

License:        MIT
URL:            https://github.com/linuxgamerlife/lgl-powerprofile-manager
# Generated from the checked-out commit by `make source` or `make srpm`.
# COPR SCM builds do not require a Git tag.
Source0:        %{name}-%{version}.tar.gz

# The root CMakeLists.txt builds the maintained GTK 4 application from src/.

BuildRequires:  cmake >= 3.20
BuildRequires:  cmake-rpm-macros
BuildRequires:  gcc-c++
BuildRequires:  ninja-build
BuildRequires:  pkgconfig(gtkmm-4.0) >= 4.20
BuildRequires:  pkgconfig(libadwaita-1) >= 1.6
BuildRequires:  pkgconfig(json-glib-1.0)
BuildRequires:  glib2-devel
BuildRequires:  desktop-file-utils
BuildRequires:  appstream
Requires:       polkit
Recommends:     tuned

%description
LGL Power Profile Manager is a GTK 4 desktop application for switching between
tuned performance profiles on Fedora and RHEL-based systems.

It runs as a normal user and uses pkexec to apply profiles with elevated
privileges. It shows the active profile, lists available profiles with
human-friendly labels and descriptions, keeps a command log, and provides a
reference page covering every built-in tuned profile. When a compatible system
tray host is available, closing the window hides the application there.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
%cmake_build

%check
%ctest

%install
%cmake_install

desktop-file-validate \
    %{buildroot}%{_datadir}/applications/com.linuxgamerlife.lgl-powerprofile-manager.desktop
appstreamcli validate --no-net \
    %{buildroot}%{_datadir}/metainfo/com.linuxgamerlife.lgl-powerprofile-manager.metainfo.xml

%files
%license LICENSE
%doc README.md CHANGELOG.md HELP.md docs/tuned-profiles.md
%{_bindir}/lgl-powerprofile-manager
%{_datadir}/applications/com.linuxgamerlife.lgl-powerprofile-manager.desktop
%{_datadir}/metainfo/com.linuxgamerlife.lgl-powerprofile-manager.metainfo.xml
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/icons/hicolor/256x256/apps/%{name}.png

%changelog
* Sun Oct 25 2026 LinuxGamerLife <222456170+linuxgamerlife@users.noreply.github.com> - 2.0.0-1
- Migrate the maintained application from Qt 6 to GTK 4
- Add standard LGL theming, title bars, menu, shortcuts, tray, settings, and tests
- Add an in-app Help window (F1) with bundled HELP.md
- Refresh the README and project documentation for the 2.0.0 release
- Use com.linuxgamerlife.lgl-powerprofile-manager as the application and AppStream ID
- Publish through the LGL Toolkit COPR
- Build the GTK application from the repository root
- Keep the Qt implementation under legacy/qt as read-only historical reference

* Sun Jun 07 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.1.2-1
- Prevent multiple instances from launching simultaneously

* Sun Jun 07 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.1.1-1
- Version bump to force COPR rebuild

* Sun Jun 07 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.1.0-1
- Minimize to tray on window close instead of quitting
- Show one-time tray notification on first minimize
- Replace tray color-square icon with actual application icon
- Moved packaging files into packaging/ subdirectory
- Added AppStream metainfo
- Added multi-size hicolor icon set (48, 64, 128, 256)

* Sun Apr 05 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.0-1
- Initial release
