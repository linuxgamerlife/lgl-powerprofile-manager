<div align="center">

# LGL Power Profile Manager

**A graphical way to manage tuned power profiles on Fedora and RHEL-based systems.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Release](https://img.shields.io/badge/release-2.0.0-purple)](CHANGELOG.md)
[![Fedora](https://img.shields.io/badge/Fedora-44-blue?logo=fedora&logoColor=white)](https://fedoraproject.org)
[![RPM](https://img.shields.io/badge/RPM-available-brightgreen?logo=redhat&logoColor=white)](https://github.com/linuxgamerlife/lgl-powerprofile-manager/releases)
[![GTK](https://img.shields.io/badge/GTK-4-green?logo=gtk&logoColor=white)](https://www.gtk.org)
[![COPR](https://img.shields.io/badge/COPR-lgl--toolkit-blue?logo=fedora&logoColor=white)](https://copr.fedorainfracloud.org/coprs/linuxgamerlife/lgl-toolkit/)
[![Support on Ko-fi](https://img.shields.io/badge/Support_on-Ko--fi-FF5E5B?logo=ko-fi&logoColor=white)](https://ko-fi.com/linuxgamerlife)

</div>

---

## Overview

LGL Power Profile Manager is a GTK 4 desktop application for switching between
tuned performance profiles without using the terminal. It shows the active
profile, lists every available profile with a human-friendly label and
description, and applies profiles through Polkit.

- See the active tuned profile and service state at a glance
- Browse profiles with labels, descriptions, and when-to-use guidance
- Apply a profile with one click through `pkexec`
- Read a full reference of every built-in profile, including when to avoid it
- Keep a bounded command log with asynchronous execution and timeouts
- Hide the window to the system tray and restore it from the tray menu
- Follow your GTK theme, dark mode, accent colour, fonts, and user CSS live
- Switch between the application title bar and the system title bar
- Open the bundled help with `F1` or the main menu

> **Note:** When a tuned profile is applied it overrides power-profiles-daemon
> settings. If your profile is reverted, mask power-profiles-daemon so the two
> services stop competing. The Status tab and `HELP.md` have the exact commands.

---

## How It Works

1. Open the **Status** tab to see the active profile and service state. It
   refreshes every five seconds.
2. Open the **Profiles** tab and select a profile from the card list.
3. Choose **Apply Profile** in the title bar, from the main menu, or press
   `Ctrl+Enter`. Administrative actions run through Polkit, which asks for
   authorization.
4. Close the window to hide it in the system tray when a compatible tray host is
   available. Use **Show Window** in the tray menu to bring it back, and **Quit**
   to exit fully.
5. The **Reference** tab explains what each profile is best for and when to
   avoid it. **Log** records command output. Press `F1` for the full help.

If `tuned-adm` is not installed, the application opens the **Setup** tab with
installation guidance instead of failing.

---

## Features

| Feature | What it does |
|---|---|
| **Live status** | Shows the active profile and tuned service state |
| **Profile list** | Lists tuned profiles with labels and descriptions |
| **Profile apply** | Applies the selected profile through `pkexec` |
| **Profile reference** | Explains best use cases and when to avoid each profile |
| **Command log** | Records applied commands and their output with bounded history |
| **Automatic refresh** | Updates status every five seconds without overwriting your selection |
| **System tray** | Provides Refresh Status, Show Window, and Quit actions |
| **Single instance** | Launching the app again restores the existing window |
| **Theme support** | Follows GTK colours, dark mode, accent colour, fonts, and user CSS |
| **Title-bar modes** | Switches between the app header bar and the system title bar |
| **Keyboard access** | F10 and Alt toggle the main menu; F5 refreshes; F1 opens Help |
| **Setup guidance** | Shows Fedora and RHEL installation steps when tuned is missing |
| **Help window** | Shows the bundled `HELP.md` with the app's monospace font |
| **About dialog** | Shows version, project links, licence, and support information |

---

## Install

The current release is **2.0.0**.

LGL Power Profile Manager is part of the
[LGL Toolkit COPR](https://copr.fedorainfracloud.org/coprs/linuxgamerlife/lgl-toolkit/).

### Recommended - Fedora COPR

Enable the LGL Toolkit COPR and install the application:

```bash
sudo dnf copr enable linuxgamerlife/lgl-toolkit
sudo dnf install lgl-powerprofile-manager
```

Install and enable tuned if it is not already present:

```bash
sudo dnf install tuned
sudo systemctl enable --now tuned
```

Future releases will then be installed through normal Fedora system updates.

### Manual downloads

RPM release artifacts are also available from
[GitHub Releases](https://github.com/linuxgamerlife/lgl-powerprofile-manager/releases).

The project can also be built directly from source.

---

## What's New in 2.0.0

- The maintained application is now a complete GTK 4 and gtkmm release
- Card-based Status, Profiles, Log, Reference, and Setup pages
- Application and system title-bar modes, with `Ctrl+Shift+M`
- Keyboard-accessible main menu, standard shortcuts, and a tray menu
- Bundled `HELP.md` with an in-app Help window (`F1`) and Help menu entry
- Tested core logic for profile data, tuned output parsing, settings, theme
  helpers, and validation
- The application ID is now `com.linuxgamerlife.lgl-powerprofile-manager`
  (previously `io.github.linuxgamerlife.lgl-powerprofile-manager`). Re-pin the
  application if it was pinned to the old desktop file.
- The old Qt implementation is retained under `legacy/qt/` as read-only
  historical reference

Earlier release notes are in [CHANGELOG.md](CHANGELOG.md).

---

## Known Issues

### Close button after tray restoration on KWin Wayland

The tray lifecycle is shared with LGL Scheduler Manager. On some KWin Wayland
sessions, the title-bar close button can ignore clicks after restoring the
window with the tray menu's **Show Window** action. The hamburger menu, tabs,
and body controls remain usable.

Workaround: double-click the title bar to maximize the window, then double-click
again to restore it. The close button then works for the rest of the application
session.

---

## Keyboard

| Shortcut | Action |
|---|---|
| Ctrl+Q | Quit |
| Ctrl+Shift+M | Switch between the application and system title bars |
| Ctrl+Enter | Apply the selected profile |
| F5 | Refresh status and the profile list |
| F1 | Open Help |
| F10 | Open or close the main menu |
| Alt (tap) | Open or close the main menu |

Window shortcuts are registered as named actions, so they also appear beside
their menu entries.

---

## Profile Order

Profiles are ordered by the most common desktop and gaming use case. Custom
profiles installed outside the built-in set appear after the known ones.

1. `balanced` - Safe Default
2. `desktop` - Daily Driver
3. `latency-performance` - Light Gaming
4. `accelerator-performance` - Gaming Performance
5. `throughput-performance` - Heavy Tasks
6. `network-latency` - Low Latency Network
7. `virtual-guest` - VM Guest
8. `virtual-host` - VM Host
9. `balanced-battery` - Battery Saving
10. `powersave` - Max Battery
11. `hpc-compute` - HPC / Scientific
12. `network-throughput` - High Throughput Network
13. `optimize-serial-console` - Headless / Serial
14. `intel-sst` - Intel SST
15. `aws` - AWS EC2

A longer guide is in [docs/tuned-profiles.md](docs/tuned-profiles.md).

---

## Note on power-profiles-daemon

When a tuned profile is applied it overrides power-profiles-daemon settings. If
you notice your profile being reverted, disable power-profiles-daemon:

```bash
# Disable
sudo systemctl mask power-profiles-daemon

# Re-enable
sudo systemctl unmask power-profiles-daemon
```

---

## Build from Source

LGL Power Profile Manager is a GTK 4 and libadwaita application built by the
root CMake configuration. Its maintained code is in `src/`, resources in
`data/`, and tests in `tests/`. It requires gtkmm 4.20, libadwaita 1.6, and
JSON-GLib or newer.

Install the dependencies on Fedora 44:

```bash
sudo dnf install cmake gcc-c++ gtkmm4.0-devel libadwaita-devel json-glib-devel
sudo dnf install tuned
sudo systemctl enable --now tuned
```

Clone, build, and test:

```bash
git clone https://github.com/linuxgamerlife/lgl-powerprofile-manager.git
cd lgl-powerprofile-manager
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Run it directly from the build directory:

```bash
./build/lgl-powerprofile-manager
```

Start hidden in the system tray with:

```bash
./build/lgl-powerprofile-manager --start-minimized
```

Install the uninstalled build with:

```bash
sudo cmake --install build
```

The old Qt implementation is retained under `legacy/qt/` as read-only
historical reference. It is not built, tested, packaged, or a runtime
dependency.

---

## Project Layout

- `src/` - maintained GTK application and widget-free core logic
- `data/` - GResource manifest, CSS, and bundled artwork
- `tests/` - GLib unit tests registered with CTest
- `packaging/` - RPM spec, desktop entry, AppStream metadata, and icons
- `docs/` - current project documentation
- `HELP.md` - in-app and packaged user help
- `legacy/qt/` - historical Qt implementation; not built or packaged

---

## Standard compliance

This release follows the shared LGL GTK desktop application standard. One
deviation is recorded:

- F10 and Alt are handled by explicit key controllers, as in the scheduler app,
  instead of `MenuButton::set_primary`. This keeps the tray hide and restore
  lifecycle reliable. F10 still opens and closes the main menu.

---

## License

MIT - see [LICENSE](LICENSE) for details.

---

<div align="center">
Made for <a href="https://fedoraproject.org">Fedora</a> · by <a href="https://www.youtube.com/@linuxgamerlife">LinuxGamerLife</a>
</div>
