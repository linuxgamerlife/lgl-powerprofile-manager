# Changelog

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
