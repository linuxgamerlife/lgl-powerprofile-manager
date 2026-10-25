# Changelog

## [2.0.0] - 2026-10-25

### Changed
- Migrated the maintained application from Qt 6 to GTK 4 (gtkmm)
- Application and AppStream ID changed to `com.linuxgamerlife.lgl-powerprofile-manager`; re-pin the application if it was pinned before
- Rebuilt the Status, Profiles, Log, and Reference pages with the shared LGL card layout
- Added a Setup page shown when `tuned-adm` is not installed
- Closing the window now hides it in the system tray only when a compatible tray host is available; otherwise it quits

### Added
- Application and system title-bar modes, with Ctrl+Shift+M
- Keyboard-accessible main menu with F10 and Alt, and the standard shortcuts
- Bundled `HELP.md` with an in-app Help window (F1) and a Help menu entry
- Refreshed README, HELP.md, and project documentation for the 2.0.0 release
- Asynchronous tuned commands with timeouts and bounded output
- Live theme following, dark mode, accent colour, desktop monospace font, and user CSS reload
- Settings in `~/.config/lgl-powerprofile-manager/settings.ini`
- Snapshot smoke test support
- GLib unit tests for profile data, tuned parsing, settings, theme helpers, and validation
- Root Makefile, COPR Makefile, and Fedora CI workflow
- Card-based profile reference with best-for and avoid-when guidance

### Build
- The root CMake configuration builds the GTK application from `src/`, `data/`, and `tests/`
- Packaging installs the binary, desktop entry, AppStream metadata, and hicolor icons
- Qt is no longer a build, test, runtime, or packaging dependency
- The old Qt implementation is archived under `legacy/qt/` as read-only historical reference

---

## [1.1.2] - 2026-06-07

### Fixed
- Prevent multiple instances of the application from launching simultaneously

---

## [1.1.1] - 2026-06-07

### Build
- Version bump to force COPR rebuild

---

## [1.1.0] - 2026-06-07

### Fixed
- Window close button now minimizes to system tray instead of quitting the application
- One-time tray notification shown on first minimize informing the user how to quit

### Changed
- System tray icon now displays the application icon instead of a colored status square
- Updated application icon to new design
- Removed unused `makeColorIcon` helper and its QPainter/QPixmap dependencies

### Build
- Added CMake install rules for binary, desktop file, metainfo, and hicolor icons (48, 64, 128, 256)
- Moved spec, desktop file, metainfo, and icons into packaging/ subdirectory
- Added AppStream metainfo file
- Added RPM spec file for COPR SCM builds
- Bumped version to 1.1.0

---

## [1.0.0] - 2026-04-05

### Initial Release

#### Core Features
- Qt6 QMainWindow-based GUI running as normal user
- pkexec used to apply tuned profiles with elevated privileges
- Auto-refresh of active profile status every 5 seconds (without overwriting pending user selection)
- System tray icon with square status indicator and active profile in tooltip

#### Tabs
- **Status** — Shows active profile and tuned service status. Includes note on power-profiles-daemon conflict with mask/unmask commands
- **Profiles** — Scrollable radio button list with human-friendly labels and descriptions. Apply button triggers pkexec tuned-adm
- **Log** — Output log from tuned-adm commands with colour-coded messages
- **Reference** — Full profile table with label and description columns
- **Setup** — Shown automatically when tuned-adm is not found on PATH

#### Profile Data
- 15 built-in tuned profiles with human-friendly labels and descriptions
- Profiles ordered by desktop/gaming relevance rather than alphabetically
- Same order maintained in both Profiles tab and Reference tab
- Custom/unknown profiles from tuned-adm list appended at the end

#### Build
- CMake 3.16+, C++20, Qt6 Core + Widgets
- Flat source layout (no src/ subdirectory) matching project conventions
- .clang-format and .clang-tidy configured
