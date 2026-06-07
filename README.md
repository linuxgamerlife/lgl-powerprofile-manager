# LGL Power Profile Manager

A Qt6 GUI for managing tuned power profiles on Fedora and RHEL-based systems.

## Overview

LGL Power Profile Manager provides a simple, desktop-friendly interface for switching between tuned profiles. It runs as a normal user and uses `pkexec` to apply profiles with elevated privileges. The application minimizes to the system tray when closed so it stays available without occupying taskbar space.

## Features

- View the currently active tuned profile
- Browse all available profiles with human-friendly labels and descriptions
- Apply profiles with a single click via pkexec
- System tray icon — click to show/hide, right-click for quick actions and quit
- Minimizes to tray on window close (one-time notification on first minimize)
- Reference tab with a full profile guide
- Auto-refreshes active profile status every 5 seconds
- Setup tab shown when tuned is not installed

## Installation

### From COPR (recommended)

```bash
sudo dnf copr enable linuxgamerlife/lgl-powerprofile-manager
sudo dnf install lgl-powerprofile-manager
```

### From source

#### Dependencies (Fedora)

```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++ tuned
sudo systemctl enable --now tuned
```

#### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Binary will be at `build/lgl-powerprofile-manager`.

#### Install

```bash
sudo cmake --install build
```

## Note on power-profiles-daemon

When a tuned profile is applied it will override power-profiles-daemon settings. If you notice your profile being reverted, you may need to disable power-profiles-daemon.

```bash
# Disable
sudo systemctl mask power-profiles-daemon

# Re-enable
sudo systemctl unmask power-profiles-daemon
```

## Profile Order

Profiles are ordered by most common desktop/gaming use case:

1. balanced — Safe Default
2. desktop — Daily Driver
3. latency-performance — Light Gaming
4. accelerator-performance — Gaming Performance
5. throughput-performance — Heavy Tasks
6. network-latency — Low Latency Network
7. virtual-guest — VM Guest
8. virtual-host — VM Host
9. balanced-battery — Battery Saving
10. powersave — Max Battery
11. hpc-compute — HPC / Scientific
12. network-throughput — High Throughput Network
13. optimize-serial-console — Headless / Serial
14. intel-sst — Intel SST
15. aws — AWS EC2

## License

MIT — see LICENSE file.
